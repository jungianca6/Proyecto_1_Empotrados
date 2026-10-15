#ifndef ROBOT_AUDIO_H
#define ROBOT_AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AUDIO_STATE_STOPPED = 0,
    AUDIO_STATE_PLAYING = 1,
    AUDIO_STATE_PAUSED = 2
} audio_state_t;

int audio_init(const char *device);
int audio_play(const char *path);
int audio_pause(void);
int audio_resume(void);
int audio_stop(void);
int audio_set_volume(unsigned int percent);
audio_state_t audio_get_state(void);
int audio_play_notification(const char *path);
void audio_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
