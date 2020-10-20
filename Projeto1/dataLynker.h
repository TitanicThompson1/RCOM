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

typedef unsigned char byte;

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

//I messafe
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

// The various types of messages
enum MessageType{
    SET,
    UA,
    I,
    DISC,
    RR,
    REJ
};

/**
 * Receives the DISC command, from file fd, and puts it in received_command array. The message is put in message.
*/
int ReceiveI(int fd, byte *received_command);

/**
 * Receives a response of type DISC, UA, REJ or RR and return the type of the message received
 */
enum MessageType ReceiveResponse(int fd, byte *received_command);

/**
 * Receives the data part of I message, from file fd. It puts the message in the correct position in received_command array.
*/
int ReceiveMessage(int fd, byte* received_command);

/**
 * Reads one byte from file fd and puts it on command.
 */
int ReadOneByte(int fd, byte *command);

/**
 * Sends SET command in array command to file fd.
 */
void send_set_command(int fd);

/**
 * Sends I command with message msg to file fd.
 */
void send_i_command(int fd, byte* msg, int n);

/**
 * Sends DISC command to file fd.
 */
void send_disc_command(int fd);

/**
 * Sends UA command to file fd.
 */
void send_ua_command(int fd);

/**
 * Sends RR command to file fd.
 */
void send_rr_command(int fd);

/**
 * Sends REJ command to file fd.
 */
void send_rej_command(int fd);

/**
 * Prints array message that has the typical format of a message (with flag in the beginning and in the end).
 * Optionally, before parameter can be used to write something before the message. If you don't desire, initialize before = NULL
 */
void print_message(char *before, byte *message);

/**
 * Updates Ns and returns the current one
 */
int updateLastNs();

/**
 * Updates Nr and returns the current one
 */
int updateLastNr();


void updateReceiverNs();


void updateEmitterNr();
#endif