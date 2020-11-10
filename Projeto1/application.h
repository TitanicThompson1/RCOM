#ifndef APPLICATION_H
#define APPLICATION_H

#include "dataLinker.h"

//Packet types
#define DATA_PACK 0x01
#define CONTROL_START 0x02
#define CONTROL_END 0x03

//File info
#define FILE_NAME 0x01
#define FILE_SIZE 0x00

//Order bytes inside packets
#define FIRST_BYTE(x) (x & 0xFF)
#define SECOND_BYTE(x) (x >> 8 & 0xFF)
#define THIRD_BYTE(x)  (x >> 16 & 0xFF)
#define FOURTH_BYTE(x) (x >> 24 & 0xFF)

/**
 * @brief Activates the debug mode (= printf throughout the code)
 * 
 */
void activate_debug_a(void);


/**
 * @brief Deactivates the debug mode
 * 
 */
void deactivate_debug_a(void);


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