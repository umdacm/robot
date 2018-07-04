#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 10000
int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    int count = 0;
    float x,y;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    for (;;)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("connection opened\n");
        count = 0;
        while (read(new_socket, buffer, 1024)) {
            // CMD 0.00 1.00\0
            // 01234567890123
            if (strncmp(buffer, "CAM ", 4) == 0) {
                x = atof(buffer+4);
                y = atof(buffer+9);
                printf("Moving camera to (%f, %f).\n", x, y);
            }

            printf("%i: %s\n", count++, buffer);
            memset(buffer, 0, sizeof(buffer)); // reset buffer (could also null terminate after read, potentially? TODO)
            send(new_socket, "OK", 2, 0); // Don't send null termination, messes with receiving script
        }
        printf("connection closed\n");
    }
    return 0;
}
