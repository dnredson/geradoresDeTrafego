#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <inttypes.h>

#define PORT 12345

uint64_t get_current_time_milliseconds() {
    struct timespec current_time;
    clock_gettime(CLOCK_REALTIME, &current_time);
    return (uint64_t)(current_time.tv_sec * 1000) + (current_time.tv_nsec / 1000000);
}

int main() {
    int server_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_length = sizeof(client_address);

    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding");
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d\n", PORT);

    while (1) {
        uint64_t network_order_payload;
        ssize_t bytes_received = recvfrom(server_socket, &network_order_payload, sizeof(network_order_payload), 0, (struct sockaddr *)&client_address, &client_address_length);

        if (bytes_received != sizeof(network_order_payload)) {
            perror("Error receiving packet");
            exit(EXIT_FAILURE);
        }

        uint64_t host_order_payload = network_order_payload;
        uint64_t current_timestamp_milliseconds = get_current_time_milliseconds();
        int delay = (int)(current_timestamp_milliseconds - host_order_payload);

        printf("Delay: %d ms\n", delay);
    }

    close(server_socket);
    return EXIT_SUCCESS;
}
