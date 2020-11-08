#ifndef MESSAGE_H
#define MESSAGE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

typedef unsigned char byte;

#define BAUDRATE B38400


//Common message parts
#define FLAG 0x7e
#define A 0x03
#define BCC1(cmd) (byte)(A^cmd)

//SET message
#define C_SET 0x03

//UA message
#define C_UA 0x07

//RR message
#define C_RR(nr) (byte)((nr << 7) | 0x05) 

//REJ message
#define C_REJ(nr) (byte)((nr << 7) | 0x01)

//DISC message
#define C_DISC 0x0B

//I message
#define C_I(x) (byte)(x << 6)

//Byte Stuffing
#define ESC 0x7d
#define FLAG_ESC 0x5e
#define ESC_ESC 0x5d


//States
#define STATE_START 1
#define STATE_FLAG  2
#define STATE_A     3
#define STATE_C     4
#define STATE_BCC1  5
#define STATE_STOP  0
#define STATE_I     6
#define STATE_BCC2  6


//Positions in Array
#define FLAG1_POS  0
#define A_POS      1
#define C_POS      2
#define BCC1_POS   3
#define FLAG2_POS  4

//Maximum size of frame of Data Lynker
#define FRAME_SIZE 256

//Maximum size of data that can be send
#define MAX_DATA_D (FRAME_SIZE - 6) / 2

// The various types of messages
enum MessageType{
    SET,
    UA,
    I,
    DISC,
    RR,
    RR_REPEATED,
    REJ,
    TIME_OUT,
    ERROR
};

// Role of computer
#define TRANSMITTER 0
#define RECEIVER 1

// Timeout
#define TIMEOUT_SECS 3
#define MAX_TIMEOUT 3


/**
 * @brief Activates the debug mode(= printf throughout the code)
 * 
 */
void activate_debug_d(void);


/**
 * @brief Deactivates the debug mode
 * 
 */
void deactivate_debug_d(void);


/**
 * @brief Receives the I message, from file fd, and puts it in received_message array
 * 
 * @param fd file descriptor of serial port
 * @param received_message the array where the message is going to be stored
 * @return int -1 in case of error; the size of the message otherwise
 */
int ReceiveI(int fd, byte *received_message);

/**
 * @brief Receives a response of type SET, DISC, UA, REJ or RR and return the type of the message received
 * 
 * @param fd file descriptor of serial port
 * @param received_message the array where the message is going to be stored
 * @return enum MessageType the type of the message received
 */
enum MessageType ReceiveMessage(int fd, byte *received_message);

/**
 * Receives the data part of I message, from file fd. It puts the message in the correct position in received_command array.
*/
int ReceiveIData(int fd, byte* received_command, byte* result);

/**
 * Reads one byte from file fd and puts it on command.
 */
int ReadOneByte(int fd, byte *command);

/**
 * Sends SET command in array command to file fd.
 */
int send_set_message(int fd);

/**
 * Sends I command with message msg to file fd.
 */
int send_i_message(int fd, byte* msg, int n);

/**
 * Sends DISC command to file fd.
 */
int send_disc_message(int fd);

/**
 * Sends UA command to file fd.
 */
int send_ua_message(int fd);

/**
 * Sends RR command to file fd.
 */
int  send_rr_message(int fd);

/**
 * Sends REJ command to file fd.
 */
int send_rej_message(int fd);

/**
 * Prints array message that has the typical format of a message (with flag in the beginning and in the end).
 * Optionally, before parameter can be used to write something before the message. If you don't desire, initialize before = NULL
 */
void print_message(char *before, byte *message, int size);

/**
 * Updates Ns and returns the current one
 */
void updateCurrentNs();

/**
 * Updates Nr and returns the current one
 */
void updateCurrentNr();


void updateReceiverNs();


void updateEmitterNr();


/**
 * Establishes the connection between the computers. 
 * Port contains the port where the communication takes place.
 * Role indicates if we want to establish the connection as the receiver or emitter
 * Returns the file descriptor of the port, or a negative integer in case of error
 */
int llopen(char *port, int role);

/**
 * Closes the connection previous established
 * fd is the file descriptor of the port
 * Return a positive integer in case of success, and a negative one, otherwise.
 */
int llclose(int fd);

/**
* Creates frame to send to receiver. 
* Does the byte stuffing by calling send_i_message.
* Returns the length of characters written, or -1 in case of error .
*/
int llwrite(int fd, byte* buffer, int length);

/**
* Reads received frame by calling ReceiveI.
* Does the destuffing by calling ReceiveIData.
* Returns the length of all characters read. 
*/
int llread(int fd, byte* buffer);

/**
 * Opens the serial port with name port for communication
 */
int open_serialPort(char *port);

/**
 * Sets the configuration of the serial port to the right one
 */
int set_costume_conf(int fd);

/**
 * The receiver receives a SET message and sends a UA message
 * This opens the communication.
 */
int open_receiver(int fd);

/**
 * The emitter sends a SET message and receives an UA message.
 * This opens the communication.
 */
int open_emitter(int fd);

/**
 * Resets the serial port to the default configuration
 */
int reset_serialPort_conf(int fd, struct termios oldtio);

/**
 * The emitter sends a DISC message, receives another DISC message, and finally sends an UA message
 * This closes the communication.
 */
int close_emitter(int fd);

/**
 * The receiver receives a DISC message, sends another DISC message and receives a UA message.
 * This opens the communication.
 */
int close_receiver(int fd);

//int compare(byte *arr1, byte *arr2);

#endif