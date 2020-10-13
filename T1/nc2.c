/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "message_types.h"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */E
#define FALSE 0
#define TRUE 1



volatile int STOP=FALSE;

int ReceiveCommand(int fd, byte *received_command);
void ReadOneByte(int fd, byte command[]);
void UA_Reply(int fd, byte* ua_reply);

int main(int argc, char** argv)
{
    int fd;
    struct termios oldtio,newtio;
    byte received_command[8], ua_reply[8];
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
    
    //tcflush(fd, TCIOFLUSH);


    if(set_msg_received == 0){
      UA_Reply(fd, ua_reply);
      printf("Message sent!\n");
    }



    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}

int ReceiveCommand(int fd, byte *received_command){
    int current_state = STATE_START;
    byte buf[1];
    
        
    while(current_state != STATE_STOP){

        ReadOneByte(fd, buf);

        if(current_state == STATE_START){

          if(buf[0] != FLAG){
            printf("Error in reading FLAG: %x\n", buf[0]);
            
          }else{
            printf("Flag read: %x\n", buf[0]);

            current_state = STATE_FLAG;
            received_command[FLAG1_POS] = buf[0];
          }

        }else if(current_state == STATE_FLAG){

          if(buf[0] == FLAG){
            printf("Received another flag\n");
            
          }else if(buf[0] == A){

            printf("A read: %x\n", buf[0]);
            current_state = STATE_A;
            received_command[A_POS] = buf[0];
          }else{
            printf("Error in reading A. Received %x\n", buf[0]);
            current_state = STATE_START;
          }
        
        }else if(current_state == STATE_A){                                 //State A
          if(buf[0] == FLAG){
            printf("Received another flag\n");
            current_state = STATE_FLAG;
            
          }else if(buf[0] == C_SET){

            printf("C_SET read: %x\n", buf[0]);
            current_state = STATE_C;
            received_command[C_POS] = buf[0];
          }else{
            printf("Error in reading C. Received %x\n", buf[0]);
            current_state = STATE_START;
          }

        }else if(current_state == STATE_C){
          if(buf[0] == FLAG){
            printf("Received another flag\n");
            current_state = STATE_FLAG;
            
          }else if(buf[0] == BCC1_SET){

            printf("BCC1 read: %x\n", buf[0]);
            current_state = STATE_BCC1;
            received_command[BCC1_POS] = buf[0];
          }else{
            printf("Error in reading BCC1_SET. Received %x\n", buf[0]);
            current_state = STATE_START;
          }
        }else if(current_state == STATE_BCC1){

          if(buf[0] == FLAG){
            printf("Flag read: %x\n", buf[0]);
            received_command[FLAG2_POS] = buf[0];
            current_state = STATE_STOP;
            
          }else{
            printf("Error in reading FLAG. Received %x\n", buf[0]);
            current_state = STATE_START;
          }
        }
    }

  return 0;
    
}


/* This functions reads one byte of a file (fd) and
 * writes it in the command in position pos.
 * Returns the new value for pos
*/
void ReadOneByte(int fd, byte command[]){

    if(read(fd, command, 1) == -1){
      perror("ReadOneByte");
      exit(-1);
    }
    
}

void UA_Reply(int fd, byte* ua_reply){
    int res;

    ua_reply[0] = FLAG;
    ua_reply[1] = A;
    ua_reply[2] = C_UA;
    ua_reply[3] = BCC1_UA;
    ua_reply[4] = FLAG;

    res = write(fd, ua_reply, 5);   
    printf("%d bytes written\n", res);
}
