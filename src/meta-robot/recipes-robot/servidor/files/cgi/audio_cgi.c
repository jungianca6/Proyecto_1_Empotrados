#define _DEFAULT_SOURCE

#include "audio_cgi.h"

#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

const char *audio_cgi_query_value(const char *name)
{
    const char *query = getenv("QUERY_STRING");
    size_t name_length = strlen(name);
    static char value[512];

    if (query == NULL) {
        return NULL;
    }
    while (*query != '\0') {
        if (strncmp(query, name, name_length) == 0 &&
            query[name_length] == '=') {
            const char *start = query + name_length + 1;
            const char *end = strchr(start, '&');
            size_t length = end == NULL ? strlen(start) : (size_t)(end - start);
            if (length >= sizeof(value)) {
                return NULL;
            }
            memcpy(value, start, length);
            value[length] = '\0';
            return value;
        }
        query = strchr(query, '&');
        if (query == NULL) {
            break;
        }
        ++query;
    }
    return NULL;
}

int audio_cgi_call(const char *command, const char *argument)
{
    pid_t child = fork();
    int status;

    if (child == 0) {
        if (argument == NULL) {
            execl("/usr/bin/audioctl", "audioctl", command, (char *)NULL);
        } else {
            execl("/usr/bin/audioctl", "audioctl", command, argument,
                  (char *)NULL);
        }
        _exit(127);
    }
    if (child < 0 || waitpid(child, &status, 0) < 0) {
        return -1;
    }
    return WIFEXITED(status) && WEXITSTATUS(status) == 0 ? 0 : -1;
}
