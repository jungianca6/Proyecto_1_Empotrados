#include "database.h"

#include <sqlite3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define DB_DIRECTORY "/var/lib/robot"
#define DB_PATH      "/var/lib/robot/users.db"

/*
 * ============================================================
 * MD5
 * ============================================================
 *
 * Implementación pequeña de MD5 para no agregar otra
 * dependencia al sistema.
 *
 * MD5 se utiliza aquí únicamente para cumplir con el
 * esquema sencillo de almacenamiento de contraseñas
 * planteado para el proyecto.
 *
 * No debe considerarse una opción adecuada para
 * almacenamiento de contraseñas en un sistema real.
 */

#include <stdint.h>

typedef struct {
    uint32_t state[4];
    uint32_t count[2];
    unsigned char buffer[64];
} MD5_CTX;

static void MD5Transform(uint32_t state[4], const unsigned char block[64]);

static void Encode(unsigned char *output,
                   const uint32_t *input,
                   unsigned int len)
{
    unsigned int i;
    unsigned int j;

    for (i = 0, j = 0; j < len; i++, j += 4) {
        output[j]     = (unsigned char)(input[i] & 0xff);
        output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
        output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
        output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
    }
}

static void Decode(uint32_t *output,
                   const unsigned char *input,
                   unsigned int len)
{
    unsigned int i;
    unsigned int j;

    for (i = 0, j = 0; j < len; i++, j += 4) {
        output[i] =
            ((uint32_t)input[j]) |
            (((uint32_t)input[j + 1]) << 8) |
            (((uint32_t)input[j + 2]) << 16) |
            (((uint32_t)input[j + 3]) << 24);
    }
}

static unsigned char PADDING[64] = {
    0x80
};

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

#define F(x,y,z) (((x) & (y)) | ((~x) & (z)))
#define G(x,y,z) (((x) & (z)) | ((y) & (~z)))
#define H(x,y,z) ((x) ^ (y) ^ (z))
#define I(x,y,z) ((y) ^ ((x) | (~z)))

#define ROTATE_LEFT(x,n) (((x) << (n)) | ((x) >> (32-(n))))

#define FF(a,b,c,d,x,s,ac) { \
    (a) += F((b),(c),(d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT((a),(s)); \
    (a) += (b); \
}

#define GG(a,b,c,d,x,s,ac) { \
    (a) += G((b),(c),(d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT((a),(s)); \
    (a) += (b); \
}

#define HH(a,b,c,d,x,s,ac) { \
    (a) += H((b),(c),(d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT((a),(s)); \
    (a) += (b); \
}

#define II(a,b,c,d,x,s,ac) { \
    (a) += I((b),(c),(d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT((a),(s)); \
    (a) += (b); \
}

static void MD5Init(MD5_CTX *context)
{
    context->count[0] = 0;
    context->count[1] = 0;

    context->state[0] = 0x67452301;
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;
}

static void MD5Update(MD5_CTX *context,
                      const unsigned char *input,
                      unsigned int input_len)
{
    unsigned int i;
    unsigned int index;
    unsigned int part_len;

    index = (unsigned int)((context->count[0] >> 3) & 0x3F);

    if ((context->count[0] += ((uint32_t)input_len << 3))
        < ((uint32_t)input_len << 3)) {
        context->count[1]++;
    }

    context->count[1] += ((uint32_t)input_len >> 29);

    part_len = 64 - index;

    if (input_len >= part_len) {
        memcpy(&context->buffer[index], input, part_len);

        MD5Transform(context->state, context->buffer);

        for (i = part_len; i + 63 < input_len; i += 64)
            MD5Transform(context->state, &input[i]);

        index = 0;
    } else {
        i = 0;
    }

    memcpy(&context->buffer[index], &input[i], input_len - i);
}

static void MD5Final(unsigned char digest[16], MD5_CTX *context)
{
    unsigned char bits[8];
    unsigned int index;
    unsigned int pad_len;

    Encode(bits, context->count, 8);

    index = (unsigned int)((context->count[0] >> 3) & 0x3f);

    pad_len = (index < 56) ? (56 - index) : (120 - index);

    MD5Update(context, PADDING, pad_len);
    MD5Update(context, bits, 8);

    Encode(digest, context->state, 16);

    memset(context, 0, sizeof(*context));
}

static void MD5Transform(uint32_t state[4],
                         const unsigned char block[64])
{
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    uint32_t x[16];

    Decode(x, block, 64);

    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];

    /* Round 1 */
    FF(a,b,c,d,x[0], S11, 0xd76aa478);
    FF(d,a,b,c,x[1], S12, 0xe8c7b756);
    FF(c,d,a,b,x[2], S13, 0x242070db);
    FF(b,c,d,a,x[3], S14, 0xc1bdceee);
    FF(a,b,c,d,x[4], S11, 0xf57c0faf);
    FF(d,a,b,c,x[5], S12, 0x4787c62a);
    FF(c,d,a,b,x[6], S13, 0xa8304613);
    FF(b,c,d,a,x[7], S14, 0xfd469501);
    FF(a,b,c,d,x[8], S11, 0x698098d8);
    FF(d,a,b,c,x[9], S12, 0x8b44f7af);
    FF(c,d,a,b,x[10], S13, 0xffff5bb1);
    FF(b,c,d,a,x[11], S14, 0x895cd7be);
    FF(a,b,c,d,x[12], S11, 0x6b901122);
    FF(d,a,b,c,x[13], S12, 0xfd987193);
    FF(c,d,a,b,x[14], S13, 0xa679438e);
    FF(b,c,d,a,x[15], S14, 0x49b40821);

    /* Round 2 */
    GG(a,b,c,d,x[1], S21, 0xf61e2562);
    GG(d,a,b,c,x[6], S22, 0xc040b340);
    GG(c,d,a,b,x[11], S23, 0x265e5a51);
    GG(b,c,d,a,x[0], S24, 0xe9b6c7aa);
    GG(a,b,c,d,x[5], S21, 0xd62f105d);
    GG(d,a,b,c,x[10], S22, 0x02441453);
    GG(c,d,a,b,x[15], S23, 0xd8a1e681);
    GG(b,c,d,a,x[4], S24, 0xe7d3fbc8);
    GG(a,b,c,d,x[9], S21, 0x21e1cde6);
    GG(d,a,b,c,x[14], S22, 0xc33707d6);
    GG(c,d,a,b,x[3], S23, 0xf4d50d87);
    GG(b,c,d,a,x[8], S24, 0x455a14ed);
    GG(a,b,c,d,x[13], S21, 0xa9e3e905);
    GG(d,a,b,c,x[2], S22, 0xfcefa3f8);
    GG(c,d,a,b,x[7], S23, 0x676f02d9);
    GG(b,c,d,a,x[12], S24, 0x8d2a4c8a);

    /* Round 3 */
    HH(a,b,c,d,x[5], S31, 0xfffa3942);
    HH(d,a,b,c,x[8], S32, 0x8771f681);
    HH(c,d,a,b,x[11], S33, 0x6d9d6122);
    HH(b,c,d,a,x[14], S34, 0xfde5380c);
    HH(a,b,c,d,x[1], S31, 0xa4beea44);
    HH(d,a,b,c,x[4], S32, 0x4bdecfa9);
    HH(c,d,a,b,x[7], S33, 0xf6bb4b60);
    HH(b,c,d,a,x[10], S34, 0xbebfbc70);
    HH(a,b,c,d,x[13], S31, 0x289b7ec6);
    HH(d,a,b,c,x[0], S32, 0xeaa127fa);
    HH(c,d,a,b,x[3], S33, 0xd4ef3085);
    HH(b,c,d,a,x[6], S34, 0x04881d05);
    HH(a,b,c,d,x[9], S31, 0xd9d4d039);
    HH(d,a,b,c,x[12], S32, 0xe6db99e5);
    HH(c,d,a,b,x[15], S33, 0x1fa27cf8);
    HH(b,c,d,a,x[2], S34, 0xc4ac5665);

    /* Round 4 */
    II(a,b,c,d,x[0], S41, 0xf4292244);
    II(d,a,b,c,x[7], S42, 0x432aff97);
    II(c,d,a,b,x[14], S43, 0xab9423a7);
    II(b,c,d,a,x[5], S44, 0xfc93a039);
    II(a,b,c,d,x[12], S41, 0x655b59c3);
    II(d,a,b,c,x[3], S42, 0x8f0ccc92);
    II(c,d,a,b,x[10], S43, 0xffeff47d);
    II(b,c,d,a,x[1], S44, 0x85845dd1);
    II(a,b,c,d,x[8], S41, 0x6fa87e4f);
    II(d,a,b,c,x[15], S42, 0xfe2ce6e0);
    II(c,d,a,b,x[6], S43, 0xa3014314);
    II(b,c,d,a,x[13], S44, 0x4e0811a1);
    II(a,b,c,d,x[4], S41, 0xf7537e82);
    II(d,a,b,c,x[11], S42, 0xbd3af235);
    II(c,d,a,b,x[2], S43, 0x2ad7d2bb);
    II(b,c,d,a,x[9], S44, 0xeb86d391);

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    memset(x, 0, sizeof(x));
}

/*
 * Calcula el MD5 de una cadena y lo devuelve como
 * texto hexadecimal de 32 caracteres.
 */
static void md5_string(const char *input, char output[33])
{
    MD5_CTX context;
    unsigned char digest[16];

    MD5Init(&context);

    MD5Update(
        &context,
        (const unsigned char *)input,
        (unsigned int)strlen(input)
    );

    MD5Final(digest, &context);

    for (int i = 0; i < 16; i++) {
        snprintf(
            &output[i * 2],
            3,
            "%02x",
            digest[i]
        );
    }

    output[32] = '\0';
}

/*
 * ============================================================
 * SQLite
 * ============================================================
 */

static int ensure_database_directory(void)
{
    struct stat st;

    if (stat(DB_DIRECTORY, &st) == 0) {
        if (!S_ISDIR(st.st_mode))
            return -1;

        return 0;
    }

    if (mkdir(DB_DIRECTORY, 0755) != 0 && errno != EEXIST)
        return -1;

    return 0;
}

static int open_database(sqlite3 **db)
{
    if (ensure_database_directory() != 0)
        return -1;

    if (sqlite3_open(DB_PATH, db) != SQLITE_OK) {
        sqlite3_close(*db);
        *db = NULL;
        return -1;
    }

    return 0;
}

int db_init(void)
{
    sqlite3 *db = NULL;
    char *error_message = NULL;

    if (open_database(&db) != 0)
        return -1;

    const char *sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL UNIQUE,"
        "password_hash TEXT NOT NULL"
        ");";

    int rc = sqlite3_exec(
        db,
        sql,
        NULL,
        NULL,
        &error_message
    );

    if (rc != SQLITE_OK) {
        fprintf(
            stderr,
            "SQLite: error creando tabla users: %s\n",
            error_message ? error_message : "desconocido"
        );

        sqlite3_free(error_message);
        sqlite3_close(db);
        return -1;
    }

    sqlite3_close(db);

    return 0;
}

int db_create_user(const char *username, const char *password)
{
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;

    if (!username || !password)
        return -1;

    if (db_init() != 0)
        return -1;

    if (open_database(&db) != 0)
        return -1;

    char password_hash[33];

    md5_string(password, password_hash);

    const char *sql =
        "INSERT INTO users (username, password_hash) "
        "VALUES (?, ?);";

    if (sqlite3_prepare_v2(
            db,
            sql,
            -1,
            &stmt,
            NULL) != SQLITE_OK) {

        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_text(
        stmt,
        1,
        username,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        stmt,
        2,
        password_hash,
        -1,
        SQLITE_TRANSIENT
    );

    int rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    if (rc == SQLITE_DONE)
        return 0;

    if (rc == SQLITE_CONSTRAINT)
        return 1;

    return -1;
}

int db_validate_user(const char *username, const char *password)
{
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;

    if (!username || !password)
        return -1;

    if (open_database(&db) != 0)
        return -1;

    char password_hash[33];

    md5_string(password, password_hash);

    const char *sql =
        "SELECT password_hash "
        "FROM users "
        "WHERE username = ?;";

    if (sqlite3_prepare_v2(
            db,
            sql,
            -1,
            &stmt,
            NULL) != SQLITE_OK) {

        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_text(
        stmt,
        1,
        username,
        -1,
        SQLITE_TRANSIENT
    );

    int rc = sqlite3_step(stmt);

    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);

        if (rc == SQLITE_DONE)
            return 0;

        return -1;
    }

    const unsigned char *stored_hash =
        sqlite3_column_text(stmt, 0);

    int valid = 0;

    if (stored_hash &&
        strcmp(
            (const char *)stored_hash,
            password_hash
        ) == 0) {

        valid = 1;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return valid;
}

void db_close(void)
{
    /*
     * No hay conexión persistente que cerrar.
     * Cada operación abre y cierra SQLite.
     */
}