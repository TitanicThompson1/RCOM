/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7E
#define A 0x03
#define C_SET 0x03
#define C_UA 0x07
#define BCC1_SET A^C_SET
#define BCC1_UA A^C_UA
volatile int STOP=FALSE;

int ReceiveCommand(int fd, char *received_command);
int ReadOneByte(int fd, char command[], int pos);

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char set_command[255], received_command[8];
    int i, sum = 0, speed = 0;
    
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
    leitura do(s) pr�ximo(s) caracter(es)
  */

    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");


    set_command[0] = FLAG;
    set_command[1] = A;
    set_command[2] = C_SET;
    set_command[3] = BCC1_SET;
    set_command[4] = FLAG;

    res = write(fd,set_command, 5);   
    printf("%d bytes written\n", res);

   
    if (tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    int set_msg_received = ReceiveCommand(fd, received_command);
    printf("Message received!\n");

    if(set_msg_received == 0){
      printf("%s\n", received_command);
    }

    close(fd);
    return 0;
}

int ReceiveCommand(int fd, char *received_command){
    int pos = 0;
    int new_pos = ReadOneByte(fd, received_command, pos);
    
    int not_done = 1;       
    while(not_done){
        
        if(received_command[pos] != FLAG){
          continue;
        }
        printf("Flag read\n");
        
        pos = new_pos;
        new_pos = ReadOneByte(fd, received_command, pos);

        if(received_command[pos] != A){
          continue;
        }
        printf("A read\n");
        
        pos = new_pos;
        new_pos = ReadOneByte(fd, received_command, pos);

        if(received_command[pos] != C_UA){
          continue;
        }

        printf("C read\n");

        pos = new_pos;
        new_pos = ReadOneByte(fd, received_command, pos);

        if(received_command[pos] != BCC1_UA){
          continue;
        }

        printf("BCC1 read\n");

        pos = new_pos;
        new_pos = ReadOneByte(fd, received_command, pos);

        if(received_command[pos] != FLAG){
          continue;
        }

        printf("Flag read\n");

        not_done = 0;
    }

  return 0;
    
}


int ReadOneByte(int fd, char command[], int pos){
    int res, j;
    char buf[1];
           
    if(read(fd, buf, 1) == -1){
      perror("ReadOneByte");
      exit(-1);
    }
                 
    command[pos++] = buf[0];
        
    return pos;
}