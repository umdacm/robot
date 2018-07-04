// socket includes
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
// camera includes
#include "../visca/libvisca.h"
#include <fcntl.h> /* File control definitions */
#include <errno.h> /* Error number definitions */


#define PORT 10000
int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    int count = 0;
    int x,y;

    VISCAInterface_t iface;
    VISCACamera_t camera;
    int camera_num;
    uint8_t value;
    uint16_t zoom;
    int pan_pos, tilt_pos;

    // SET UP SOCKET
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        exit(1);
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                &opt, sizeof(opt)))
        exit(2);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                sizeof(address)) < 0)
        exit(3);
    if (listen(server_fd, 3) < 0)
        exit(4);

    // SET UP CAMERA

    if (VISCA_open_serial(&iface, "/dev/ttyS2")!=VISCA_SUCCESS)
    {
        fprintf(stderr,"%s: unable to open serial device %s\n",argv[0],argv[1]);
        exit(5);
    }

    iface.broadcast=0;
    VISCA_set_address(&iface, &camera_num);
    camera.address=1;
    VISCA_clear(&iface, &camera);

    VISCA_get_camera_info(&iface, &camera);
    fprintf(stderr,"Some camera info:\n------------------\n");
    fprintf(stderr,"vendor: 0x%04x\n model: 0x%04x\n ROM version: 0x%04x\n socket number: 0x%02x\n",
            camera.vendor, camera.model, camera.rom_version, camera.socket_num);

    // Handling loop
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
                x = (int)(atof(buffer+4) * 24.0); // TODO: bounds checking?
                y = (int)(atof(buffer+9) * 20.0);
                printf("Moving camera to (%i, %i).\n", x, y);
                if (x > 0) {
                    if (y > 0) {
                        VISCA_set_pantilt_upright(&iface, &camera, x, y);
                    } else {
                        VISCA_set_pantilt_downright(&iface, &camera, x, -y);
                    }
                } else {
                    if (y > 0) {
                        VISCA_set_pantilt_upleft(&iface, &camera, -x, y);
                    } else {
                        VISCA_set_pantilt_downleft(&iface, &camera, -x, -y);
                    }
                }
            } else {
                printf("Unknown command: %s", buffer);
            }

            printf("%i: %s\n", count++, buffer);
            memset(buffer, 0, sizeof(buffer)); // reset buffer (could also null terminate after read, potentially? TODO)
            send(new_socket, "OK", 2, 0); // Don't send null termination, messes with receiving script
        }

        printf("connection closed\n");
    }

    return 0;
}
