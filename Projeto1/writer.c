
#include "application.h"

int main(int argc, char** argv)
{ 
  activate_debug_a();
  activate_debug_d();

  if( (argc < 2) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0) && (strcmp("/dev/ttyS2", argv[1])!=0)
      && (strcmp("/dev/ttyS3", argv[1])!=0) && (strcmp("/dev/ttyS4", argv[1])!=0) && (strcmp("/dev/ttyS5", argv[1])!=0))) {
    printf("Usage: writer\tnserial SerialPort\tfilename \n\tex: writer /dev/ttyS1 pinguim.gif\n");
    exit(-1);
  }
  
  printf("Sending file...\n");
  if(sendFile(argv[2], argv[1]) < 0){
      exit(-1);
  }
  printf("Done!\n");
}