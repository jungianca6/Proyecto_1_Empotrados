#ifndef DATABASE_H
#define DATABASE_H

/*
 * Inicializa la base de datos de usuarios.
 *
 * Crea el directorio /var/lib/robot si no existe y
 * crea la tabla users si todavía no existe.
 *
 * Retorna:
 *   0  -> éxito
 *  -1  -> error
 */
int db_init(void);

/*
 * Registra un nuevo usuario.
 *
 * La contraseña recibida se convierte a MD5 antes
 * de almacenarse.
 *
 * Retorna:
 *   0  -> usuario creado
 *   1  -> usuario ya existe
 *  -1  -> error de base de datos
 */
int db_create_user(const char *username, const char *password);

/*
 * Verifica las credenciales de un usuario.
 *
 * Retorna:
 *   1 -> usuario y contraseña correctos
 *   0 -> credenciales incorrectas
 *  -1 -> error de base de datos
 */
int db_validate_user(const char *username, const char *password);

/*
 * Cierra cualquier recurso de la base de datos.
 *
 * Actualmente la implementación abre/cierra SQLite
 * por operación, por lo que esta función existe como
 * parte de la API pero no necesita hacer nada.
 */
void db_close(void);

#endif