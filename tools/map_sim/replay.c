/*
 * map_replay: reconstruye el mapa a partir de un log de eventos
 * (/tmp/robot_events.log) grabado en el robot o en el simulador.
 *
 * Sirve para ajustar los parámetros de calibración sin volver a correr
 * el robot: se edita el .conf y se repite la reconstrucción.
 *
 * Uso: map_replay events.log [-c robot_map.conf] [-o map.json] [--ascii]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mapper.h"

static int jnum(const char *line, const char *key, double *out)
{
    char pat[48];
    const char *p;

    snprintf(pat, sizeof(pat), "\"%s\":", key);
    p = strstr(line, pat);
    if (!p)
        return 0;
    *out = strtod(p + strlen(pat), NULL);
    return 1;
}

static int jstr(const char *line, const char *key, char *out, size_t n)
{
    char pat[48];
    const char *p, *q;

    snprintf(pat, sizeof(pat), "\"%s\":\"", key);
    p = strstr(line, pat);
    if (!p)
        return 0;
    p += strlen(pat);
    q = strchr(p, '"');
    if (!q || (size_t)(q - p) >= n)
        return 0;
    memcpy(out, p, (size_t)(q - p));
    out[q - p] = '\0';
    return 1;
}

int main(int argc, char **argv)
{
    const char *log = NULL, *conf = NULL, *out = NULL;
    int ascii = 0;
    mapper_params_t mp;
    char line[512];
    double t = 0, last_t = 0;
    long n_state = 0, n_obst = 0;

    for (int i = 1; i < argc; i++) {
        if      (!strcmp(argv[i], "-c") && i + 1 < argc) conf = argv[++i];
        else if (!strcmp(argv[i], "-o") && i + 1 < argc) out = argv[++i];
        else if (!strcmp(argv[i], "--ascii"))            ascii = 1;
        else                                             log = argv[i];
    }
    if (!log) {
        fprintf(stderr, "uso: %s events.log [-c robot_map.conf] [-o map.json] [--ascii]\n", argv[0]);
        return 2;
    }

    mapper_params_default(&mp);
    if (conf && mapper_params_load(&mp, conf) != 0) {
        fprintf(stderr, "no se pudo leer %s\n", conf);
        return 1;
    }
    if (mapper_init(&mp) != 0)
        return 1;

    FILE *f = fopen(log, "r");
    if (!f) {
        fprintf(stderr, "no se pudo abrir %s\n", log);
        return 1;
    }

    while (fgets(line, sizeof(line), f)) {
        char ev[24], to[32];

        if (!jstr(line, "ev", ev, sizeof(ev)) || !jnum(line, "t", &t))
            continue;
        last_t = t;

        if (!strcmp(ev, "state") && jstr(line, "to", to, sizeof(to))) {
            mapper_set_motion((uint64_t)t, mapper_motion_from_name(to));
            n_state++;
        } else if (!strcmp(ev, "obstacle")) {
            double fr = 0, lf = 0;
            jnum(line, "front", &fr);
            jnum(line, "left", &lf);
            mapper_obstacle((uint64_t)t, (int)fr, (int)lf);
            n_obst++;
        }
    }
    fclose(f);
    mapper_advance((uint64_t)last_t);

    double x, y, deg;
    mapper_get_pose(&x, &y, &deg);
    printf("replay: %ld cambios de estado, %ld obstaculos, %.1f s; pose final (%.1f, %.1f) cm, %.1f grados\n",
           n_state, n_obst, last_t / 1000.0, x, y, deg);

    if (out && mapper_write_json(out) != 0) {
        fprintf(stderr, "no se pudo escribir %s\n", out);
        return 1;
    }
    if (ascii)
        mapper_dump_ascii(stdout);

    mapper_free();
    return 0;
}
