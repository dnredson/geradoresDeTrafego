#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <inttypes.h>
#include <endian.h>
#include <string.h>
#include <math.h>

#define PORT 12345

uint64_t get_current_time_milliseconds() {
    struct timespec current_time;
    clock_gettime(CLOCK_REALTIME, &current_time);
    return (uint64_t)(current_time.tv_sec * 1000) + (current_time.tv_nsec / 1000000);
}

double generate_uniform() {
    return (double)rand() / RAND_MAX;
}

int poisson_distribution(double lambda) {
    if (lambda < 30) {
        double L = exp(-lambda);
        double p = 1.0;
        int k = 0;

        while (p > L) {
            k++;
            p *= generate_uniform();
        }

        return k - 1;
    } else {
        double b = 0.931 + 2.53 * sqrt(lambda);
        double a = -0.059 + 0.02483 * b;
        double inv_alpha = 1.1239 + 1.1328 / (b - 3.4);
        double v_r = 0.9277 - 3.6224 / (b - 2);

        while (1) {
            double U, V, us;
            U = generate_uniform() - 0.5;
            V = generate_uniform();
            us = 0.5 - fabs(U);
            if (us < 0.013 && V > us) {
                continue;
            }

            double k = floor((2 * a / us + b) * U + lambda + 0.43);
            if (k < 0) {
                continue;
            }

            if (us >= 0.07 && V <= v_r) {
                return (int)k;
            }

            if (us < 0.013 && V <= exp(-0.5 * k * k / lambda)) {
                return (int)k;
            }

            if (log(V) + log(inv_alpha) - log(a / (us * us) + b) <= -lambda + k * log(lambda) - lgamma(k + 1)) {
                return (int)k;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <duration in seconds> <messages per second> <destination IP>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int duration = atoi(argv[1]);
    double messages_per_second = atof(argv[2]);
    const char *dest_ip = argv[3];

    int client_socket;
    struct sockaddr_in server_address;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(dest_ip);

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error connecting to the server");
        exit(EXIT_FAILURE);
    }

    time_t start_time = time(NULL);

    while (time(NULL) - start_time < duration) {
        int num_messages = poisson_distribution(messages_per_second);
        uint64_t start_of_loop = get_current_time_milliseconds();

        for (int i = 0; i < num_messages; ++i) {
            uint64_t current_timestamp = get_current_time_milliseconds();
            char xml_payload[1024]; 
            snprintf(xml_payload, sizeof(xml_payload), "<xml><timestamp>%" PRIu64 "</timestamp></xml>", current_timestamp);
            
            printf("Sending: %s\n", xml_payload);
            ssize_t bytes_sent = send(client_socket, xml_payload, strlen(xml_payload), 0);
            if (bytes_sent != strlen(xml_payload)) {
                perror("Error sending XML payload");
                exit(EXIT_FAILURE);
            }
        }

        uint64_t end_of_loop = get_current_time_milliseconds();
        uint64_t elapsed_time = end_of_loop - start_of_loop;
        uint64_t time_to_sleep = (1000 > elapsed_time) ? (1000 - elapsed_time) : 0;
        usleep(time_to_sleep * 1000);
    }

    close(client_socket);
    return 0;
}
