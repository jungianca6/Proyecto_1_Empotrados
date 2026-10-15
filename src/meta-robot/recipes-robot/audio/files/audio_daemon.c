#define _DEFAULT_SOURCE

#include "robot_audio.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>

#ifndef AUDIO_SOCKET_PATH
#define AUDIO_SOCKET_PATH "/run/robot-audio.sock"
#endif
#define COMMAND_SIZE 1024

static volatile sig_atomic_t running = 1;

static void stop_daemon(int signal_number)
{
    (void)signal_number;
    running = 0;
}

static int write_response(int client, const char *response)
{
    size_t length = strlen(response);
    return write(client, response, length) == (ssize_t)length ? 0 : -1;
}

static void process_command(int client, char *command)
{
    char *argument;

    command[strcspn(command, "\r\n")] = '\0';
    argument = strchr(command, ' ');
    if (argument != NULL) {
        *argument++ = '\0';
    }

    if (strcmp(command, "PLAY") == 0 && argument != NULL) {
        if (audio_play(argument) == 0) {
            write_response(client, "OK\n");
        } else {
            write_response(client, "ERROR\n");
        }
    } else if (strcmp(command, "NOTIFY") == 0 && argument != NULL) {
        if (audio_play_notification(argument) == 0) {
            write_response(client, "OK\n");
        } else {
            write_response(client, "ERROR\n");
        }
    } else if (strcmp(command, "PAUSE") == 0) {
        write_response(client, audio_pause() == 0 ? "OK\n" : "ERROR\n");
    } else if (strcmp(command, "RESUME") == 0) {
        write_response(client, audio_resume() == 0 ? "OK\n" : "ERROR\n");
    } else if (strcmp(command, "STOP") == 0) {
        write_response(client, audio_stop() == 0 ? "OK\n" : "ERROR\n");
    } else if (strcmp(command, "VOLUME") == 0 && argument != NULL) {
        char *end = NULL;
        unsigned long value = strtoul(argument, &end, 10);
        if (*argument != '\0' && end != argument && *end == '\0' &&
            value <= 100) {
            write_response(client, audio_set_volume((unsigned int)value) == 0 ?
                           "OK\n" : "ERROR\n");
        } else {
            write_response(client, "ERROR\n");
        }
    } else if (strcmp(command, "STATE") == 0) {
        dprintf(client, "STATE %d\n", audio_get_state());
    } else {
        write_response(client, "ERROR\n");
    }
}

int main(void)
{
    struct sockaddr_un address;
    struct sigaction action = {0};
    int server;

    action.sa_handler = stop_daemon;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
    signal(SIGPIPE, SIG_IGN);

    if (audio_init("default") < 0) {
        fprintf(stderr, "No se pudo inicializar el audio\n");
        return EXIT_FAILURE;
    }

    server = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server < 0) {
        perror("socket");
        audio_shutdown();
        return EXIT_FAILURE;
    }

    unlink(AUDIO_SOCKET_PATH);
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, AUDIO_SOCKET_PATH, sizeof(address.sun_path) - 1);
    if (bind(server, (struct sockaddr *)&address, sizeof(address)) < 0 ||
        listen(server, 8) < 0) {
        perror("bind/listen");
        close(server);
        audio_shutdown();
        unlink(AUDIO_SOCKET_PATH);
        return EXIT_FAILURE;
    }
    chmod(AUDIO_SOCKET_PATH, 0660);

    while (running) {
        int client = accept(server, NULL, NULL);
        if (client < 0) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }

        {
            char command[COMMAND_SIZE];
            ssize_t bytes = read(client, command, sizeof(command) - 1);
            if (bytes > 0) {
                command[bytes] = '\0';
                process_command(client, command);
            }
        }
        close(client);
    }

    close(server);
    unlink(AUDIO_SOCKET_PATH);
    audio_shutdown();
    return EXIT_SUCCESS;
}
