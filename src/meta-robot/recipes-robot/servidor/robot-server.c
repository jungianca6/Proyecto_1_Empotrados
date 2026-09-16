#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_PORT 8080
#define BUFFER_SIZE 8192
#define TOKEN_SIZE 33

static volatile sig_atomic_t running = 1;
static char session_token[TOKEN_SIZE];
static int autonomous_mode;
static int motor_command;

static void stop_server(int signal_number)
{
    (void)signal_number;
    running = 0;
}

static int get_port(void)
{
    const char *value = getenv("ROBOT_SERVER_PORT");
    char *end;
    long port;

    if (value == NULL || *value == '\0') {
        return DEFAULT_PORT;
    }
    port = strtol(value, &end, 10);
    if (*end != '\0' || port < 1 || port > 65535) {
        return DEFAULT_PORT;
    }
    return (int)port;
}

static int configured_credentials_match(const char *body)
{
    const char *user = getenv("ROBOT_SERVER_USER");
    const char *password = getenv("ROBOT_SERVER_PASSWORD");
    if (user == NULL) {
        user = "admin";
    }
    if (password == NULL) {
        password = "admin";
    }
        return strstr(body, "\"user\"") != NULL && strstr(body, user) != NULL &&
            strstr(body, "\"password\"") != NULL && strstr(body, password) != NULL;
}

static void make_token(void)
{
    FILE *random_file = fopen("/dev/urandom", "rb");
    unsigned char bytes[16];
    static const char hex[] = "0123456789abcdef";
    size_t index;

    if (random_file != NULL && fread(bytes, sizeof(bytes), 1, random_file) == 1) {
        fclose(random_file);
        for (index = 0; index < sizeof(bytes); ++index) {
            session_token[index * 2] = hex[bytes[index] >> 4];
            session_token[index * 2 + 1] = hex[bytes[index] & 0x0f];
        }
        session_token[32] = '\0';
        return;
    }
    if (random_file != NULL) {
        fclose(random_file);
    }
    (void)snprintf(session_token, sizeof(session_token), "%08x%08x%08x%08x",
                   (unsigned)getpid(), (unsigned)time(NULL), (unsigned)clock(), (unsigned)get_port());
}

static int authenticated(const char *request)
{
    char header[64];
    (void)snprintf(header, sizeof(header), "Authorization: Bearer %s", session_token);
    return session_token[0] != '\0' && strstr(request, header) != NULL;
}

static void send_response(int client, int status, const char *content_type, const char *body)
{
    const char *reason = status == 200 ? "OK" : status == 401 ? "Unauthorized" : "Bad Request";
    char response[BUFFER_SIZE];
    int length = snprintf(response, sizeof(response),
                          "HTTP/1.1 %d %s\r\nContent-Type: %s\r\nContent-Length: %zu\r\n"
                          "Connection: close\r\nAccess-Control-Allow-Origin: *\r\n\r\n%s",
                          status, reason, content_type, strlen(body), body);
    (void)send(client, response, (size_t)length, 0);
}

static void handle_request(int client, const char *request)
{
    const char *body = strstr(request, "\r\n\r\n");
    const char *path_start = strchr(request, ' ');
    const char *path_end;
    char path[128];
    size_t path_length;

    body = body == NULL ? "" : body + 4;
    if (path_start == NULL) {
        send_response(client, 400, "application/json", "{\"status\":\"error\",\"message\":\"Solicitud invalida\"}");
        return;
    }
    ++path_start;
    path_end = strchr(path_start, ' ');
    if (path_end == NULL) {
        send_response(client, 400, "application/json", "{\"status\":\"error\",\"message\":\"Solicitud invalida\"}");
        return;
    }
    path_length = (size_t)(path_end - path_start);
    if (path_length >= sizeof(path)) {
        send_response(client, 400, "application/json", "{\"status\":\"error\",\"message\":\"Ruta demasiado larga\"}");
        return;
    }
    (void)memcpy(path, path_start, path_length);
    path[path_length] = '\0';

    if (strcmp(path, "/") == 0) {
        static const char page[] = "<!doctype html><html lang=es><meta charset=utf-8>"
            "<title>Robot aspiradora</title><h1>Robot aspiradora</h1>"
            "<p>Servidor operativo. Use <code>/api/login</code> y <code>/api/status</code>.</p>";
        send_response(client, 200, "text/html; charset=utf-8", page);
    } else if (strcmp(path, "/api/login") == 0) {
        if (!configured_credentials_match(body)) {
            send_response(client, 401, "application/json", "{\"status\":\"error\",\"message\":\"Credenciales invalidas\"}");
            return;
        }
        make_token();
        char response[128];
        (void)snprintf(response, sizeof(response), "{\"status\":\"ok\",\"token\":\"%s\"}", session_token);
        send_response(client, 200, "application/json", response);
    } else if (!authenticated(request)) {
        send_response(client, 401, "application/json", "{\"status\":\"error\",\"message\":\"Autenticacion requerida\"}");
    } else if (strcmp(path, "/api/status") == 0) {
        char response[256];
        (void)snprintf(response, sizeof(response),
                       "{\"status\":\"ok\",\"mode\":\"%s\",\"motor\":%d,\"sensors\":{\"front\":null,\"side\":null},\"leds\":{}}",
                       autonomous_mode ? "autonomous" : "manual", motor_command);
        send_response(client, 200, "application/json", response);
    } else if (strcmp(path, "/api/mode") == 0) {
        autonomous_mode = strstr(body, "autonomous") != NULL;
        motor_command = 0;
        send_response(client, 200, "application/json", "{\"status\":\"ok\"}");
    } else if (strcmp(path, "/api/motors") == 0) {
        motor_command = strstr(body, "stop") != NULL ? 0 : 1;
        send_response(client, 200, "application/json", "{\"status\":\"ok\"}");
    } else {
        send_response(client, 400, "application/json", "{\"status\":\"error\",\"message\":\"Ruta inexistente\"}");
    }
}

int main(void)
{
    int server;
    int option = 1;
    struct sockaddr_in address;
    struct sigaction signal_action;

    (void)memset(&signal_action, 0, sizeof(signal_action));
    signal_action.sa_handler = stop_server;
    (void)sigaction(SIGINT, &signal_action, NULL);
    (void)sigaction(SIGTERM, &signal_action, NULL);
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }
    (void)setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    (void)memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons((uint16_t)get_port());
    if (bind(server, (struct sockaddr *)&address, sizeof(address)) < 0 || listen(server, 8) < 0) {
        perror("bind/listen");
        close(server);
        return EXIT_FAILURE;
    }
    printf("robot-server escuchando en el puerto %d\n", get_port());
    while (running) {
        int client = accept(server, NULL, NULL);
        char request[BUFFER_SIZE];
        ssize_t received;

        if (client < 0) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }
        received = recv(client, request, sizeof(request) - 1, 0);
        if (received > 0) {
            char *headers_end;
            char *length_header;
            size_t required = 0;

            request[received] = '\0';
            headers_end = strstr(request, "\r\n\r\n");
            length_header = strstr(request, "Content-Length:");
            if (headers_end != NULL && length_header != NULL) {
                required = (size_t)(headers_end - request) + 4 +
                           (size_t)strtoul(length_header + 15, NULL, 10);
            }
            while (required > (size_t)received && (size_t)received < sizeof(request) - 1) {
                ssize_t more = recv(client, request + received, sizeof(request) - 1 - (size_t)received, 0);
                if (more <= 0) {
                    break;
                }
                received += more;
                request[received] = '\0';
            }
            handle_request(client, request);
        }
        close(client);
    }
    close(server);
    return EXIT_SUCCESS;
}