
#include "application.h"

int main(int argc, char** argv)
{ 
  
  if ( (argc < 3) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0) )) {
    printf("Usage: writer\tnserial SerialPort\tfilename \n\tex: nserial /dev/ttyS1 pinguim.gif\n");
    exit(-1);
  }
  
  printf("Sending file...\n");
  if(sendFile(argv[2], argv[1]) < 0){
      exit(-1);
  }
  printf("Done!\n");
}