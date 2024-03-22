#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>  

#define PORT 12345
#define BUFFER_SIZE 1024 

uint64_t get_current_time_milliseconds() {
    struct timespec current_time;
    clock_gettime(CLOCK_REALTIME, &current_time);
    return (uint64_t)(current_time.tv_sec * 1000) + (current_time.tv_nsec / 1000000);
}

void receive_packet(int server_socket) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t bytes_received = recv(server_socket, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_received <= 0) {
        perror("Error receiving packet or connection closed");
        exit(EXIT_FAILURE);
    }

    uint64_t timestamp_from_packet;
    char *timestamp_start = strstr(buffer, "<timestamp>");
    char *timestamp_end = strstr(buffer, "</timestamp>");

    if (timestamp_start && timestamp_end) {
        timestamp_start += strlen("<timestamp>"); 
        *timestamp_end = '\0'; 
        timestamp_from_packet = atoll(timestamp_start);
    } else {
        fprintf(stderr, "Invalid XML format\n");
        exit(EXIT_FAILURE);
    }

    uint64_t current_timestamp_milliseconds = get_current_time_milliseconds();
    int delay = (int)(current_timestamp_milliseconds - timestamp_from_packet);

    //printf("Timestamp from packet: %" PRIu64 "\n", timestamp_from_packet);
    //printf("Current timestamp: %" PRIu64 "\n", current_timestamp_milliseconds);
    //printf("Delay: %d ms\n", delay);
    printf("D| %d \n", delay);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_length = sizeof(client_address);

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 1) == -1) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    printf("Listening on 127.0.0.1:%d\n", PORT);

    if ((client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_length)) == -1) {
        perror("Error accepting connection");
        exit(EXIT_FAILURE);
    }

    printf("Connection established with %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    while (1) {
        receive_packet(client_socket);
    }

    close(client_socket);
    close(server_socket);

    return 0;
}
