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
#define C_SET 0x03
#define C_UA 0x07
#define BCC1_SET A^C_SET
#define BCC1_UA A^C_UA

volatile int STOP=FALSE;

int ReceiveCommand(int fd, char *received_command);
int ReadOneByte(int fd, char command[], int pos);
void UAReply(int fd, char *ua_reply);

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char received_command[8], ua_reply[8];
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

    
    int set_msg_received = ReceiveCommand(fd, received_command);
    printf("Message received!\n");
    
    if(set_msg_received == 0){
      UA_Reply(fd, ua_reply);
      printf("Message sent!\n");
    }



    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}

int ReceiveCommand(int fd, char *received_command){
    int pos = 0;
    int new_pos = ReadOneByte(fd, received_command, pos);
    
    int not_done = 1;       
    while(not_done){
        pos = 0;

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

        if(received_command[pos] != C_SET){
          continue;
        }

        printf("C read\n");

        pos = new_pos;
        new_pos = ReadOneByte(fd, received_command, pos);

        if(received_command[pos] != BCC1_SET){
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


/* This functions reads one byte of a file (fd) and
 * writes it in the command in position pos.
 * Returns the new value for pos
*/
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

void UA_Reply(int fd, char* ua_reply){
    int res;

    ua_reply[0] = FLAG;
    ua_reply[1] = A;
    ua_reply[2] = C_UA;
    ua_reply[3] = BCC1_UA;
    ua_reply[4] = FLAG;

    res = write(fd,ua_reply, 5);   
    printf("%d bytes written\n", res);
}
