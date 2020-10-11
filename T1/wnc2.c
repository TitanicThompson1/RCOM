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
#include <signal.h>

#include "message_types.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;


int timeout_flag = 0;
int n_alarm = 0;


int ReceiveCommand(int fd, byte *received_command);
int ReadOneByte(int fd, byte command[], int pos);
void send_command(int fd, byte* command);

void count(){
	timeout_flag=1;
	n_alarm++;
}

int main(int argc, char** argv)
{
    int fd;
    struct termios oldtio,newtio;
    byte set_command[8], received_command[8];
    //int i, sum = 0, speed = 0;
    
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

    newtio.c_cc[VTIME]    = 30;   /* inter-character timer unused */
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

    while( n_alarm != 3){

      send_command(fd, set_command);

      sleep(1);

      (void) signal(SIGALRM, count);
    
      int set_msg_received = ReceiveCommand(fd, received_command);

      if(set_msg_received == 0){

        printf("Message received!\n");
        printf("%s\n", received_command);
        break;
      }else{
        printf("Error in reading. Repeating reading.\n");
      }
    }

    if(n_alarm == 3){
      printf("Repeated reading 3 times. Now ending\n");
    }

    if (tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);
    return 0;
}

int ReceiveCommand(int fd, byte *received_command){
    int pos = 0;
    int new_pos = ReadOneByte(fd, received_command, pos);
    
    if(new_pos == -1)
      return -1;
    

    int not_done = 1;  
    while(not_done || n_alarm == 3){
        pos = 0;
        
        if(received_command[pos] != FLAG){
          printf("Error in reading FLAG: %x\n", received_command[pos]);
          n_alarm++;
          return -1;
        }
        printf("Flag read: %x\n", received_command[pos]);
        
        pos = new_pos;
        new_pos = ReadOneByte(fd, received_command, pos);

        if(received_command[pos] != A){
          printf("Error in reading A: %x\n", received_command[pos]);
          n_alarm++;
          return -1;
        }
        printf("A read: %x\n", received_command[pos]);
        
        pos = new_pos;
        new_pos = ReadOneByte(fd, received_command, pos);

        if(received_command[pos] != C_UA){
          printf("Error in reading C_UA: %x\n", received_command[pos]);
          n_alarm++;
          return -1;
        }

        printf("C read: %x\n", received_command[pos]);

        pos = new_pos;
        new_pos = ReadOneByte(fd, received_command, pos);

        //TODO: Perguntar ao prof
        byte bcc1 = BCC1_UA;
        if(received_command[pos] != bcc1){
          printf("Error in reading BCC1_UA: %x", received_command[pos]);
          n_alarm++;
          return -1;
        }

        printf("BCC1 read: %x\n", received_command[pos]);

        pos = new_pos;
        new_pos = ReadOneByte(fd, received_command, pos);

        if(received_command[pos] != FLAG){
          printf("Error in reading FLAG: %x\n", received_command[pos]);
          n_alarm++;
          return -1;
        }

        printf("Flag read: %x\n", received_command[pos]);

        not_done = 0;
    }

  return 0;
    
}


int ReadOneByte(int fd, byte command[], int pos){
    byte buf[1];
    int n;

    if((n = read(fd, buf, 1)) == -1){
      printf("ReadOneByte\n");
      exit(-1);
    }

    if(n==0){
      n_alarm++;
      return -1;
    }

    command[pos++] = buf[0];

    return pos;
}


void send_command(int fd, byte* command){
    int res;

    command[0] = FLAG;
    command[1] = A;
    command[2] = C_SET;
    command[3] = BCC1_SET;
    command[4] = FLAG;

    res = write(fd, command, 5);   
    printf("%d bytes written\n", res);
}