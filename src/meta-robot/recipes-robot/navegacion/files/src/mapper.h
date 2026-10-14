#ifndef MAPPER_H
#define MAPPER_H

/*
 * Mapa incremental por odometría estimada (dead reckoning).
 *
 * Este módulo NO toca hardware: recibe eventos (cambio de movimiento,
 * obstáculo detectado) con su marca de tiempo y mantiene una pose
 * estimada y una grilla 2D. Por eso se puede ejecutar en el robot,
 * en un simulador o reproduciendo un log grabado (map_replay).
 *
 * Marco de referencia: origen en el punto de arranque, +x a la derecha,
 * +y hacia adelante (norte del mapa), ángulos antihorarios. El robot
 * arranca mirando hacia +y.
 */

#include <stdint.h>
#include <stdio.h>

typedef enum {
    MAP_MOVE_NONE = 0,
    MAP_MOVE_FORWARD,
    MAP_MOVE_BACKWARD,
    MAP_MOVE_TURN_LEFT,
    MAP_MOVE_TURN_RIGHT
} map_motion_t;

enum {
    MAP_UNKNOWN  = 0,
    MAP_VISITED  = 1,
    MAP_OBSTACLE = 2
};

typedef struct {
    double cell_cm;           /* lado de cada celda                              */
    int    grid_w, grid_h;    /* tamaño de la grilla en celdas                   */
    double speed_cm_s;        /* avance/retroceso a plena potencia               */
    double turn_rate_deg_s;   /* velocidad angular al girar sobre una rueda      */
    double pivot_offset_cm;   /* centro del chasis -> rueda que queda quieta     */
    double robot_radius_cm;   /* radio del robot (área limpiada por pasada)      */
    double front_obstacle_cm; /* distancia centro->obstáculo cuando dispara IR f */
    double side_obstacle_cm;  /* distancia centro->obstáculo cuando dispara IR l */
    int    turn_sign;         /* +1 normal; -1 si los giros salen invertidos     */
} mapper_params_t;

void mapper_params_default(mapper_params_t *p);

/* Lee líneas "clave=valor". Retorna 0 si lo leyó, -1 si no existe el archivo. */
int  mapper_params_load(mapper_params_t *p, const char *path);

int  mapper_init(const mapper_params_t *p);
void mapper_free(void);

/* Integra el movimiento hasta t_ms y luego cambia al nuevo movimiento. */
void mapper_set_motion(uint64_t t_ms, map_motion_t m);

/* Integra el movimiento actual hasta t_ms. */
void mapper_advance(uint64_t t_ms);

/* Marca obstáculos según los sensores digitales (frontal / lateral izquierdo). */
void mapper_obstacle(uint64_t t_ms, int front, int left);

/* Traduce el nombre de estado de la FSM ("MOVING_FORWARD", ...) a movimiento. */
map_motion_t mapper_motion_from_name(const char *state_name);

int  mapper_dirty(void);

/* Escribe JSON de forma atómica (archivo temporal + rename). 0 ok, -1 error. */
int  mapper_write_json(const char *path);

void mapper_get_params(mapper_params_t *p);
void mapper_get_pose(double *x_cm, double *y_cm, double *theta_deg);
const uint8_t *mapper_grid(int *w, int *h);

/* Depuración: dibuja el mapa en texto. */
void mapper_dump_ascii(FILE *f);

#endif /* MAPPER_H */
