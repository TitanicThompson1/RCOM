#ifndef MESSAGE_TYPES_H
#define MESSAGE_TYPES_H

typedef unsigned char byte;

#define FLAG 0x7e
#define A 0x03

#define C_SET 0x03
#define BCC1_SET A^C_SET

#define C_UA 0x07
#define BCC1_UA A^C_UA

#endif