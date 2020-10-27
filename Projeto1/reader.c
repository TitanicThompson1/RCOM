
#include "application.h"

int main(int argc, char** argv)
{   
  /*
  if ( (argc < 2) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0) )) {
    printf("Usage: reader\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(-1);
  }

  if(receiveFile(argv[1]) < 0){
      exit(-1);
  }
  */
  byte buf[512];

  int fd = llopen(argv[1], RECEIVER);

  llread(fd, buf);
  printf("First read\n");
  
  llread(fd, buf);
  printf("Second read\n");
  
  llread(fd, buf);
  printf("Third read\n");

  llread(fd, buf);
  printf("Fourth read\n");

  llread(fd, buf);
  printf("Fift read\n");

  llread(fd, buf);
  printf("Six read\n");

  llclose(fd);
}