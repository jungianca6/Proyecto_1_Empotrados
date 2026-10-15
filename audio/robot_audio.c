#include "robot_audio.h"

#include <alsa/asoundlib.h>
#include <errno.h>
#include <mpg123.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define AUDIO_BUFFER_SIZE 16384
#define AUDIO_LATENCY_US 500000

typedef struct {
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    char *requested_path;
    char *device;
    bool shutdown;
    bool stop_requested;
    bool pause_requested;
    bool notification;
    unsigned int volume;
    audio_state_t state;
} audio_player_t;

static struct {
    pthread_mutex_t mutex;
    bool initialized;
    audio_player_t normal;
    audio_player_t notification;
} audio_context = {
    .mutex = PTHREAD_MUTEX_INITIALIZER
};

static void set_state(audio_player_t *player, audio_state_t state)
{
    pthread_mutex_lock(&player->mutex);
    player->state = state;
    pthread_mutex_unlock(&player->mutex);
}

static int configure_pcm(snd_pcm_t **pcm, const char *device,
                         long rate, int channels)
{
    int result;

    result = snd_pcm_open(pcm, device, SND_PCM_STREAM_PLAYBACK, 0);
    if (result < 0) {
        return result;
    }

    result = snd_pcm_set_params(
        *pcm,
        SND_PCM_FORMAT_S16_LE,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        (unsigned int)channels,
        (unsigned int)rate,
        1,
        AUDIO_LATENCY_US
    );
    if (result < 0) {
        snd_pcm_close(*pcm);
        *pcm = NULL;
    }

    return result;
}

static void scale_samples(int16_t *samples, size_t sample_count,
                          unsigned int volume)
{
    size_t index;

    if (volume >= 100) {
        return;
    }

    for (index = 0; index < sample_count; ++index) {
        samples[index] = (int16_t)(((int32_t)samples[index] * volume) / 100);
    }
}

static bool should_stop(audio_player_t *player)
{
    bool stop;

    pthread_mutex_lock(&player->mutex);
    stop = player->shutdown || player->stop_requested;
    pthread_mutex_unlock(&player->mutex);
    return stop;
}

static int wait_if_paused(audio_player_t *player, unsigned int *volume)
{
    int result = 0;

    pthread_mutex_lock(&player->mutex);
    while (player->pause_requested && !player->shutdown &&
           !player->stop_requested) {
        player->state = AUDIO_STATE_PAUSED;
        pthread_cond_wait(&player->condition, &player->mutex);
    }

    if (player->shutdown || player->stop_requested) {
        result = -ECANCELED;
    } else {
        player->state = AUDIO_STATE_PLAYING;
        *volume = player->volume;
    }
    pthread_mutex_unlock(&player->mutex);
    return result;
}

static void play_file(audio_player_t *player, const char *path)
{
    mpg123_handle *decoder = NULL;
    snd_pcm_t *pcm = NULL;
    unsigned char buffer[AUDIO_BUFFER_SIZE];
    int16_t *samples = (int16_t *)buffer;
    int error;
    int encoding;
    long rate;
    int channels;

    decoder = mpg123_new(NULL, &error);
    if (decoder == NULL || mpg123_open(decoder, path) != MPG123_OK) {
        if (decoder != NULL) {
            mpg123_delete(decoder);
        }
        set_state(player, AUDIO_STATE_STOPPED);
        return;
    }

    if (mpg123_getformat(decoder, &rate, &channels, &encoding) != MPG123_OK ||
        encoding != MPG123_ENC_SIGNED_16) {
        mpg123_close(decoder);
        mpg123_delete(decoder);
        set_state(player, AUDIO_STATE_STOPPED);
        return;
    }

    if (configure_pcm(&pcm, player->device, rate, channels) < 0) {
        mpg123_close(decoder);
        mpg123_delete(decoder);
        set_state(player, AUDIO_STATE_STOPPED);
        return;
    }

    set_state(player, AUDIO_STATE_PLAYING);
    while (!should_stop(player)) {
        size_t bytes_read = 0;
        unsigned int volume = 100;
        snd_pcm_sframes_t frames;

        if (wait_if_paused(player, &volume) < 0) {
            break;
        }

        error = mpg123_read(decoder, buffer, sizeof(buffer), &bytes_read);
        if (error != MPG123_OK && error != MPG123_DONE) {
            break;
        }

        if (bytes_read > 0) {
            scale_samples(samples, bytes_read / sizeof(int16_t), volume);
            frames = snd_pcm_writei(pcm, buffer,
                                    bytes_read / (sizeof(int16_t) * channels));
            if (frames < 0) {
                frames = snd_pcm_recover(pcm, (int)frames, 1);
                if (frames < 0) {
                    break;
                }
            }
        }

        if (error == MPG123_DONE) {
            break;
        }
    }

    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    mpg123_close(decoder);
    mpg123_delete(decoder);
    set_state(player, AUDIO_STATE_STOPPED);
}

static void *player_thread(void *argument)
{
    audio_player_t *player = argument;

    for (;;) {
        char *path;

        pthread_mutex_lock(&player->mutex);
        while (!player->shutdown && player->requested_path == NULL) {
            pthread_cond_wait(&player->condition, &player->mutex);
        }

        if (player->shutdown) {
            pthread_mutex_unlock(&player->mutex);
            break;
        }

        path = player->requested_path;
        player->requested_path = NULL;
        player->stop_requested = false;
        player->pause_requested = false;
        pthread_mutex_unlock(&player->mutex);

        play_file(player, path);
        free(path);
    }

    set_state(player, AUDIO_STATE_STOPPED);
    return NULL;
}

static int initialize_player(audio_player_t *player, const char *device,
                             bool notification)
{
    int result;

    memset(player, 0, sizeof(*player));
    player->device = strdup(device);
    player->volume = 100;
    player->notification = notification;
    player->state = AUDIO_STATE_STOPPED;
    if (player->device == NULL) {
        return -ENOMEM;
    }

    result = pthread_mutex_init(&player->mutex, NULL);
    if (result != 0) {
        free(player->device);
        return -result;
    }

    result = pthread_cond_init(&player->condition, NULL);
    if (result != 0) {
        pthread_mutex_destroy(&player->mutex);
        free(player->device);
        return -result;
    }

    result = pthread_create(&player->thread, NULL, player_thread, player);
    if (result != 0) {
        pthread_cond_destroy(&player->condition);
        pthread_mutex_destroy(&player->mutex);
        free(player->device);
        return -result;
    }

    return 0;
}

static int request_play(audio_player_t *player, const char *path)
{
    char *copy;

    if (path == NULL || path[0] == '\0') {
        return -EINVAL;
    }

    copy = strdup(path);
    if (copy == NULL) {
        return -ENOMEM;
    }

    pthread_mutex_lock(&player->mutex);
    free(player->requested_path);
    player->requested_path = copy;
    player->stop_requested = true;
    player->pause_requested = false;
    pthread_cond_signal(&player->condition);
    pthread_mutex_unlock(&player->mutex);
    return 0;
}

static int ensure_initialized(void)
{
    bool initialized;

    pthread_mutex_lock(&audio_context.mutex);
    initialized = audio_context.initialized;
    pthread_mutex_unlock(&audio_context.mutex);
    return initialized ? 0 : -ENODEV;
}

int audio_init(const char *device)
{
    int result;
    const char *selected_device = device != NULL ? device : "default";

    pthread_mutex_lock(&audio_context.mutex);
    if (audio_context.initialized) {
        pthread_mutex_unlock(&audio_context.mutex);
        return 0;
    }

    mpg123_init();
    result = initialize_player(&audio_context.normal, selected_device, false);
    if (result == 0) {
        result = initialize_player(&audio_context.notification,
                                   selected_device, true);
    }
    if (result != 0) {
        audio_context.normal.shutdown = true;
        audio_context.notification.shutdown = true;
        pthread_mutex_unlock(&audio_context.mutex);
        return result;
    }

    audio_context.initialized = true;
    pthread_mutex_unlock(&audio_context.mutex);
    return 0;
}

int audio_play(const char *path)
{
    if (ensure_initialized() < 0) {
        return -ENODEV;
    }
    return request_play(&audio_context.normal, path);
}

int audio_pause(void)
{
    if (ensure_initialized() < 0) {
        return -ENODEV;
    }
    pthread_mutex_lock(&audio_context.normal.mutex);
    audio_context.normal.pause_requested = true;
    pthread_mutex_unlock(&audio_context.normal.mutex);
    return 0;
}

int audio_resume(void)
{
    if (ensure_initialized() < 0) {
        return -ENODEV;
    }
    pthread_mutex_lock(&audio_context.normal.mutex);
    audio_context.normal.pause_requested = false;
    pthread_cond_signal(&audio_context.normal.condition);
    pthread_mutex_unlock(&audio_context.normal.mutex);
    return 0;
}

int audio_stop(void)
{
    if (ensure_initialized() < 0) {
        return -ENODEV;
    }
    pthread_mutex_lock(&audio_context.normal.mutex);
    audio_context.normal.stop_requested = true;
    audio_context.normal.pause_requested = false;
    pthread_cond_signal(&audio_context.normal.condition);
    pthread_mutex_unlock(&audio_context.normal.mutex);
    return 0;
}

int audio_set_volume(unsigned int percent)
{
    if (percent > 100) {
        return -EINVAL;
    }
    if (ensure_initialized() < 0) {
        return -ENODEV;
    }

    pthread_mutex_lock(&audio_context.normal.mutex);
    audio_context.normal.volume = percent;
    pthread_mutex_unlock(&audio_context.normal.mutex);

    pthread_mutex_lock(&audio_context.notification.mutex);
    audio_context.notification.volume = percent;
    pthread_mutex_unlock(&audio_context.notification.mutex);
    return 0;
}

audio_state_t audio_get_state(void)
{
    audio_state_t state;

    if (ensure_initialized() < 0) {
        return AUDIO_STATE_STOPPED;
    }

    pthread_mutex_lock(&audio_context.normal.mutex);
    state = audio_context.normal.state;
    pthread_mutex_unlock(&audio_context.normal.mutex);
    return state;
}

int audio_play_notification(const char *path)
{
    if (ensure_initialized() < 0) {
        return -ENODEV;
    }
    return request_play(&audio_context.notification, path);
}

static void destroy_player(audio_player_t *player)
{
    pthread_mutex_lock(&player->mutex);
    player->shutdown = true;
    pthread_cond_signal(&player->condition);
    pthread_mutex_unlock(&player->mutex);
    pthread_join(player->thread, NULL);
    free(player->requested_path);
    free(player->device);
    pthread_cond_destroy(&player->condition);
    pthread_mutex_destroy(&player->mutex);
}

void audio_shutdown(void)
{
    pthread_mutex_lock(&audio_context.mutex);
    if (!audio_context.initialized) {
        pthread_mutex_unlock(&audio_context.mutex);
        return;
    }

    audio_context.initialized = false;
    pthread_mutex_unlock(&audio_context.mutex);

    destroy_player(&audio_context.normal);
    destroy_player(&audio_context.notification);
    mpg123_exit();
}
