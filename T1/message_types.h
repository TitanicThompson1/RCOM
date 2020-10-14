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


#endif