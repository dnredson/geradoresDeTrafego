#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <inttypes.h>
#include <endian.h>
#include <time.h>
#include <stdint.h>

#define PORT 12345
#define BUFFER_SIZE 1024

uint64_t get_current_time_milliseconds() {
    struct timespec current_time;
    clock_gettime(CLOCK_REALTIME, &current_time);
    return (uint64_t)(current_time.tv_sec * 1000) + (current_time.tv_nsec / 1000000);
}

ssize_t read_exact(int sockfd, void *buffer, size_t length) {
    size_t to_read = length;
    ssize_t bytes_read;
    char *buf = (char *)buffer;

    while (to_read > 0) {
        bytes_read = recv(sockfd, buf, to_read, 0);
        if (bytes_read <= 0) {
            return bytes_read;
        }
        to_read -= bytes_read;
        buf += bytes_read;
    }
    return length;
}

void *handle_connection(void *client_socket_ptr) {
    int client_socket = *((int *)client_socket_ptr);
    free(client_socket_ptr);

    char buffer[BUFFER_SIZE];

    while (1) {
        uint16_t network_content_type_length;
        if (read_exact(client_socket, &network_content_type_length, sizeof(network_content_type_length)) != sizeof(network_content_type_length)) {
            perror("Error reading Content-Type length");
            break;
        }

        uint16_t content_type_length = ntohs(network_content_type_length);
        if (content_type_length >= BUFFER_SIZE) {
            fprintf(stderr, "Content-Type length is too large\n");
            break;
        }

        if (read_exact(client_socket, buffer, content_type_length) != content_type_length) {
            perror("Error reading Content-Type");
            break;
        }
        buffer[content_type_length - 1] = '\0';
        printf("Content-Type: %s\n", buffer);

        uint64_t network_order_payload;
        if (read_exact(client_socket, &network_order_payload, sizeof(network_order_payload)) != sizeof(network_order_payload)) {
            perror("Error receiving packet");
            break;
        }

        uint64_t host_order_payload = be64toh(network_order_payload);
        uint64_t current_timestamp_milliseconds = get_current_time_milliseconds();
        int delay = (int)(current_timestamp_milliseconds - host_order_payload);

        printf("D| %d \n", delay);
    }

    close(client_socket);
    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_length = sizeof(client_address);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) == -1) { 
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    printf("Listening on 127.0.0.1:%d\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_length);
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        printf("Connection established with %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        int *new_sock = malloc(sizeof(int));
        *new_sock = client_socket;

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_connection, (void *)new_sock) != 0) {
            perror("Failed to create thread");
            free(new_sock);
        }

               
        pthread_detach(thread_id);
    }

    
    close(server_socket);

    return 0;
}

