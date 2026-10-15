#define _DEFAULT_SOURCE

#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    const char *device = argc > 1 ? argv[1] : "default";
    snd_pcm_t *pcm = NULL;
    int result;

    result = snd_pcm_open(&pcm, device, SND_PCM_STREAM_PLAYBACK, 0);
    if (result < 0) {
        fprintf(stderr, "No se pudo abrir '%s': %s\n", device, snd_strerror(result));
        return EXIT_FAILURE;
    }

    printf("Dispositivo abierto: %s\n", device);

    snd_pcm_close(pcm);
    return EXIT_SUCCESS;
}
