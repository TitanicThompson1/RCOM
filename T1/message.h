#ifndef MESSAGE_TYPES_H
#define MESSAGE_TYPES_H

typedef unsigned char byte;

//Common message parts
#define FLAG 0x7e
#define A 0x03
#define BCC1(cmd) (byte)(A^cmd)

//SET command
#define C_SET 0x03

//UA command
#define C_UA 0x07

//RR command
#define C_RR(x) (byte)(x << 7 | 0x05)

//DISC command
#define C_DISC 0x0B

//I command
#define C_I(x) (byte)(x << 6) 


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

//
enum MessageType{
    SET,
    UA,
    I,
    DISC,
    RR,
    REJ
};

/**
 * Receives a command of type MessageType, from file fd, and puts it in received_command array
 */
int ReceiveCommand(int fd, byte *received_command, enum MessageType message);

/**
 * Receives the UA command, from file fd, and puts it in received_command array
 */
int ReceiveUA(int fd, byte *received_command);

/**
 * Receives the RR command, from file fd, and puts it in received_command array
 */
int ReceiveRR(int fd, byte *received_command);

/**
 * Reads one byte from file fd and puts it on command
 */
int ReadOneByte(int fd, byte *command);

/**
 * Updates Ns or Nr
 */
void updateN(int *n);

/**
 * Receives the DISC command, from file fd, and puts it in received_command array
 */
int ReceiveDISC(int fd, byte *received_command);

/**
 * Sends SET command in array command to file fd
 */
void send_set_command(int fd, byte* command);

/**
 * Prints array message
 */
void print_message(byte *message);

/**
 * Sends I command with message msg to file fd
 */
void send_I_command(int fd, char * msg);

/**
 * Sends SET command to file fd
 */
void send_UA(int fd, byte* ua_reply);

/**
 * Receives the DISC command, from file fd, and puts it in received_command array. The message is put in message
*/
void ReceiveI(int fd, byte *received_command, char* message);

void ReceiveData(int fd, char *message);

#endif