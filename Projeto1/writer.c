
#include "application.h"

int main(int argc, char** argv)
{ 
  /*
  if ( (argc < 3) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0) )) {
    printf("Usage: writer\tnserial SerialPort\tfilename \n\tex: nserial /dev/ttyS1 pinguim.gif\n");
    exit(-1);
  }
  
  if(sendFile(argv[2], argv[1]) < 0){
      exit(-1);
  }
  */

  byte buf[512] = {0x01, 0x02, 0x03};

  int fd = llopen(argv[1], RECEIVER);

  llwrite(fd, buf, 3);
  printf("First wrote\n");
  
  llwrite(fd, buf, 3);
  printf("Second wrote\n");
  
  llwrite(fd, buf, 3);
  printf("Third wrote\n");

  llwrite(fd, buf, 3);
  printf("Fourth wrote\n");

  llwrite(fd, buf, 3);
  printf("Fift wrote\n");

  llwrite(fd, buf, 3);
  printf("Six wrote\n");

  llclose(fd);

}