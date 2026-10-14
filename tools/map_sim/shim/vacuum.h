#ifndef SHIM_VACUUM_H
#define SHIM_VACUUM_H

/* Sólo para verificar sintaxis de main.c fuera del robot. */
int  vacuum_init(void);
void vacuum_cleanup(void);
void vacuum_on(void);
void vacuum_off(void);

#endif
