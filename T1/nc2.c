/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7E
#define A 0x01
#define C_UA 0x07
#define BCC1 A^C

volatile int STOP=FALSE;

void ReceiveCommand(int fd, char received_command);
int ReadOneByte(int fd, char command[], int pos);

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char received_command[255], ua_command[255];
    /*
    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }
    */


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
  
    
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */



  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) proximo(s) caracter(es)
  */

    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");

    


    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}

void ReceiveCommand(int fd, char *received_command){
    int pos = 0;
    char A_temp;
    char C_temp;
    int new_pos = ReadOneByte(fd, received_command, pos);
    
    int not_done = 0;       
    while(not_done){
        
        if(strcmp(received_command[pos], FLAG) != 0)
            continue;
        
        pos = new_pos;
        new_pos = ReadOneByte(fd, received_command, pos);

        if(strcmp(received_command[new_pos], A) != 0)
            continue;
        

        pos = new_pos;
        new_pos = ReadOneByte(fd, received_command, pos);

        if(strcmp(received_command[new_pos], C) != 0){

        }
    }
    
}


/* This functions reads one byte of a file (fd) and
 * writes it in the command in position pos.
 * Returns the new value for pos
*/
int ReadOneByte(int fd, char command[], int pos){
    int res, j;
    char buf[1];
           
    res = read(fd, buf, 1);
    //TODO: ERROR TRATEMENT               
    command[pos++] = buf[0];
        
    return pos;
}
