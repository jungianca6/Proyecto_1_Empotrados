#include "robot_audio.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void usage(const char *program)
{
    printf("Uso: %s <archivo.mp3> [notificacion.mp3]\n", program);
    printf("       %s --help\n", program);
}

int main(int argc, char *argv[])
{
    int result;

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        usage(argv[0]);
        return EXIT_SUCCESS;
    }
    if (argc < 2 || argc > 3) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    result = audio_init("default");
    if (result < 0) {
        fprintf(stderr, "audio_init fallo: %s\n", strerror(-result));
        return EXIT_FAILURE;
    }

    result = audio_set_volume(80);
    if (result < 0) {
        fprintf(stderr, "audio_set_volume fallo: %s\n", strerror(-result));
        audio_shutdown();
        return EXIT_FAILURE;
    }

    result = audio_play(argv[1]);
    if (result < 0) {
        fprintf(stderr, "audio_play fallo: %s\n", strerror(-result));
        audio_shutdown();
        return EXIT_FAILURE;
    }

    if (argc == 3) {
        result = audio_play_notification(argv[2]);
        if (result < 0) {
            fprintf(stderr, "audio_play_notification fallo: %s\n",
                    strerror(-result));
            audio_shutdown();
            return EXIT_FAILURE;
        }
    }

    printf("Reproduccion solicitada. Estado inicial: %d\n", audio_get_state());
    sleep(2);
    printf("Estado despues de 2 segundos: %d\n", audio_get_state());
    audio_pause();
    printf("Pausa solicitada. Estado: %d\n", audio_get_state());
    sleep(1);
    audio_resume();
    printf("Reanudacion solicitada. Estado: %d\n", audio_get_state());
    sleep(2);
    audio_stop();
    audio_shutdown();
    return EXIT_SUCCESS;
}
