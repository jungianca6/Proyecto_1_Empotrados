#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>     // getpid()
#include <sys/stat.h>   // mkdir()
#include <sys/types.h>  // tipos usados por mkdir()
#include "session.h"

#define SESSION_DIR "/tmp/sessions"
#define SESSION_TTL_SECONDS 3600 // 1 hora

static void generate_token(char *out, int len) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    srand((unsigned int)time(NULL) ^ (unsigned int)getpid());
    for (int i = 0; i < len - 1; i++) {
        out[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    out[len - 1] = '\0';
}

int session_create(const char *username, char *token_out, int token_out_len) {
    generate_token(token_out, token_out_len);

    mkdir(SESSION_DIR, 0700); // ignora error si ya existe

    char path[256];
    snprintf(path, sizeof(path), "%s/%s", SESSION_DIR, token_out);

    FILE *f = fopen(path, "w");
    if (!f) return -1;

    fprintf(f, "%s\n%ld\n", username, time(NULL) + SESSION_TTL_SECONDS);
    fclose(f);
    return 0;
}

int session_validate(const char *token) {
    if (!token || strlen(token) == 0) return 0;

    char path[256];
    snprintf(path, sizeof(path), "%s/%s", SESSION_DIR, token);

    FILE *f = fopen(path, "r");
    if (!f) return 0;

    char username[128];
    long expires;
    if (fscanf(f, "%127s\n%ld", username, &expires) != 2) {
        fclose(f);
        return 0;
    }
    fclose(f);

    return (time(NULL) < expires) ? 1 : 0;
}