// rotate-test
// Chandler Swift <chandler@chandlerswift.com>, June 2018

// These includes pull in interface definitions and utilities.
#include "mobilitycomponents_i.h"
#include "mobilitydata_i.h"
#include "mobilitygeometry_i.h"
#include "mobilityactuator_i.h"
#include "mobilityutil.h"
#include "userlib.h"
#include <iostream.h>

// socket includes
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>



#define PORT 10001
int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    int count = 0;
    int x,y;

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

    // SET UP DRIVE

    // This framework class simplifies setup and initialization for
    // client-only programs like this one.p
    //
    mbyClientHelper *pHelper;

    // This is a generic pointer that can point to any CORBA object
    // within Mobility.
    CORBA::Object_ptr ptempObj;

    // This is a smart pointer to an object descriptor. Automacially
    // manages memory.
    // The XXX_var classes automaticall release references for
    // hassle-free memory management.
    MobilityCore::ObjectDescriptor_var pDescriptor;

    // This is a buffer for object names.
    char pathName[255];

    // Holds -robot command line option.
    char *robotName = "MagellanPro";

    // All Mobility servers and clients use CORBA and this initialization
    // is required for the C++ language mapping of CORBA.
    pHelper = new mbyClientHelper(argc, argv);

    // Build a pathname to the component we want to use to get sensor data.
    sprintf(pathName, "%s/Sonar/Segment", robotName); // Use robot name arg.

    // Locate the component we want.
    ptempObj = pHelper->find_object(pathName);

    // Build pathname to the component we want to use to drive the robot.
    sprintf(pathName, "%s/Drive/Command", robotName); // Use robot name arg.

    // Locate object within robot.
    ptempObj = pHelper->find_object(pathName);

    // Find the drive command (we're going to drive the robot around).
    // The XX_var is a smart pointer for memory management.
    MobilityActuator::ActuatorState_var pDriveCommand;
    MobilityActuator::ActuatorData OurCommand;

    // We'll send two axes of command. Axis[0] == translate, Axis[1] == rotate.
    OurCommand.velocity.length(2);

    // Request the interface we need from the object we found.
    try
    {
        pDriveCommand = MobilityActuator::ActuatorState::_duplicate(
            MobilityActuator::ActuatorState::_narrow(ptempObj));
    } catch (...) {
        return -1;
    }

    return 0;
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
            if (strncmp(buffer, "DRV ", 4) == 0) {
                // TODO: Bounds Checking?
                OurCommand.velocity[0] = atof(buffer+4);
                OurCommand.velocity[1] = atof(buffer+9);

                pDriveCommand->new_sample(OurCommand, 0);
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
