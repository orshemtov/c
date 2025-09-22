#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

static void send_response(int client_socket, const char *version, int status_code, const char *headers, const char *body)
{
    char response[BUFFER_SIZE];
    int body_length = body ? strlen(body) : 0;
    int response_length = snprintf(response, BUFFER_SIZE,
                                   "%s %d OK\r\n"
                                   "Content-Length: %d\r\n"
                                   "%s\r\n"
                                   "%s",
                                   version, status_code, body_length,
                                   headers ? headers : "",
                                   body ? body : "");
    write(client_socket, response, response_length);
}

void handle_request(int client_socket)
{
    char request[BUFFER_SIZE] = {0};
    ssize_t bytes_read;

    bytes_read = read(client_socket, request, BUFFER_SIZE - 1);
    if (bytes_read > 0)
    {
        request[bytes_read] = '\0';
        printf("%s\n", request);
    }
}

void handle_response(int client_socket)
{
    send_response(client_socket, "HTTP/1.1", 200, NULL, "Hello from my HTTP server!");
}

int main()
{
    int server_fd, client_socket;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 3) == -1)
    {
        perror("Listen failed");
        close(server_fd);
        return 1;
    }

    printf("Server listening on port %d\n", PORT);

    while (1)
    {
        socklen_t addr_len = sizeof(addr);
        client_socket = accept(server_fd, (struct sockaddr *)&addr, &addr_len);
        if (client_socket == -1)
        {
            perror("Accept failed");
            continue;
        }
        handle_request(client_socket);
        handle_response(client_socket);
        close(client_socket);
    }

    close(server_fd);

    return 0;
}