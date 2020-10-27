#ifndef APPLICATION_H
#define APPLICATION_H

#include "dataLinker.h"

#define DATA_PACK 0x01
#define CONTROL_START 0x02
#define CONTROL_END 0x03

#define FILE_NAME 0x01
#define FILE_SIZE 0x00

#define FIRST_BYTE(x) (byte)((x << 24) >> 24)
#define SECOND_BYTE(x) (byte)((x << 16) >> 24)
#define THIRD_BYTE(x)  (byte)((x << 8) >> 24)
#define FOURTH_BYTE(x) (byte)(x >> 24)

/*
* Responsible for calling the necessary functions to create the control packets and data packets, and sending them.
* Return 0 for success.
*/
int sendFile(char* filename, char *port);

/*
* Creates the corresponding control packet and sends it.
* Return 0 for success.
*/
int sendControlPacket(int controlType, unsigned int fileLength, char * filename, int fd);

/*
* Sends all data packets until there's no packet left.
* Return 0 for success.
*/
int sendDataPackets(FILE *file, int fd);

/*
* Responsible for calling all the necessary function to read the control packets and data packets.
* Return 0 for success.
*/
int receiveFile(char *port);

/*
* Reads and interprets the received control packets.
* Return 0 for success.
*/
int readControlPacket(int fd, char* filename);

/*
* Reads and interprets the received data packets.
* Return 0 for success.
*/
int readDataPackets(int fd, FILE *file);


#endif