#ifndef NAV_EVENTS_H
#define NAV_EVENTS_H

/*
 * Emisor de eventos de navegación + alimentación del mapa.
 *
 * Se engancha a la FSM sin modificarla: después de cada fsm_update()
 * compara el estado con el anterior y, si cambió, escribe una línea
 * JSON en el log de eventos y avisa al mapper.
 *
 * Valores por defecto (NULL):
 *   log_path  -> /tmp/robot_events.log
 *   map_path  -> /tmp/robot_map.json
 *   conf_path -> /etc/robot_map.conf   (cadena vacía "" = no leer conf)
 */

int  nav_events_init(const char *log_path, const char *map_path, const char *conf_path);

/* Llamar en cada iteración del lazo principal, después de fsm_update(). */
void nav_events_tick(void);

/* Escribe el mapa final y cierra el log. */
void nav_events_close(void);

#endif /* NAV_EVENTS_H */
