#define _DEFAULT_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#ifndef AUDIO_SOCKET_PATH
#define AUDIO_SOCKET_PATH "/run/robot-audio.sock"
#endif

static int connect_audio(void)
{
    struct sockaddr_un address;
    int socket_fd;

    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        return -1;
    }
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, AUDIO_SOCKET_PATH, sizeof(address.sun_path) - 1);
    if (connect(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        close(socket_fd);
        return -1;
    }
    return socket_fd;
}

int main(int argc, char *argv[])
{
    char command[1024];
    char response[128];
    int socket_fd;
    int length;
    ssize_t bytes;

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Uso: %s play|notify|pause|resume|stop|volume|state [valor]\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "play") == 0 && argc == 3) {
        length = snprintf(command, sizeof(command), "PLAY %s\n", argv[2]);
    } else if (strcmp(argv[1], "notify") == 0 && argc == 3) {
        length = snprintf(command, sizeof(command), "NOTIFY %s\n", argv[2]);
    } else if (strcmp(argv[1], "volume") == 0 && argc == 3) {
        length = snprintf(command, sizeof(command), "VOLUME %s\n", argv[2]);
    } else if (strcmp(argv[1], "pause") == 0 && argc == 2) {
        length = snprintf(command, sizeof(command), "PAUSE\n");
    } else if (strcmp(argv[1], "resume") == 0 && argc == 2) {
        length = snprintf(command, sizeof(command), "RESUME\n");
    } else if (strcmp(argv[1], "stop") == 0 && argc == 2) {
        length = snprintf(command, sizeof(command), "STOP\n");
    } else if (strcmp(argv[1], "state") == 0 && argc == 2) {
        length = snprintf(command, sizeof(command), "STATE\n");
    } else {
        fprintf(stderr, "Argumentos invalidos\n");
        return EXIT_FAILURE;
    }

    if (length < 0 || (size_t)length >= sizeof(command)) {
        fprintf(stderr, "Comando demasiado largo\n");
        return EXIT_FAILURE;
    }

    socket_fd = connect_audio();
    if (socket_fd < 0) {
        perror("robot-audio.sock");
        return EXIT_FAILURE;
    }
    if (write(socket_fd, command, (size_t)length) != length) {
        close(socket_fd);
        return EXIT_FAILURE;
    }
    bytes = read(socket_fd, response, sizeof(response) - 1);
    close(socket_fd);
    if (bytes <= 0) {
        return EXIT_FAILURE;
    }
    response[bytes] = '\0';
    fputs(response, stdout);
    return strncmp(response, "OK", 2) == 0 ||
                   strncmp(response, "STATE", 5) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
