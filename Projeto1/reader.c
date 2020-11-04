
#include "application.h"

int main(int argc, char** argv)
{   
  
  if( (argc < 2) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0) && (strcmp("/dev/ttyS2", argv[1])!=0)
      && (strcmp("/dev/ttyS3", argv[1])!=0) && (strcmp("/dev/ttyS4", argv[1])!=0) && (strcmp("/dev/ttyS5", argv[1])!=0))) {
    printf("Usage: reader\tnserial SerialPort\n\tex: reader /dev/ttyS1\n");
    exit(-1);
  }

  printf("Receiving file...\n");
  if(receiveFile(argv[1]) < 0){
      exit(-1);
  }

  printf("Done!\n");
}