#ifndef MESSAGE_TYPES_H
#define MESSAGE_TYPES_H

typedef unsigned char byte;

#define FLAG 0x7e
#define A 0x03

#define C_SET 0x03
#define BCC1_SET (byte)(A^C_SET)

#define C_UA 0x07
#define BCC1_UA (byte)(A^C_UA)

//States
#define STATE_START 1
#define STATE_FLAG  2
#define STATE_A     3
#define STATE_C     4
#define STATE_BCC1  5
#define STATE_STOP  0

//Positions in Array
#define FLAG1_POS  0
#define A_POS      1
#define C_POS      2
#define BCC1_POS   3
#define FLAG2_POS  4

#endif