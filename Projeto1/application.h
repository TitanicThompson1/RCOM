#ifndef APPLICATION_H
#define APPLICATION_H

#include "dataLinker.h"

#define CONTROL_DATA 0x01
#define CONTROL_START 0x02
#define CONTROL_END 0x03

/*
* Responsible for calling the necessary functions to create the control packets and data packets, and sending them.
* Return 0 for success.
*/
int sendFile(int fd, char* filename, int port);

/*
* Creates the corresponding control packet and sends it.
* Return 0 for success.
*/
int sendControlPacket(int controlType, int fileLength, char * filename, int port);

/*
* Creates the data packets to be sent.
* Return 0 for success.
*/
int createDataPackets();

/*
* Sends all data packets until there's no packet left.
* Return 0 for success.
*/
int sendDataPackets();

/*
* Responsible for calling all the necessary function to read the control packets and data packets.
* Return 0 for success.
*/
int receiveFile(int fd);

/*
* Reads and interprets the received control packets.
* Return 0 for success.
*/
int readControlPacket();

/*
* Reads and interprets the received data packets.
* Return 0 for success.
*/
int readDataPackets();


#endif