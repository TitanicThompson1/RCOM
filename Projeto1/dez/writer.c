
#include "application.h"
#include <time.h>

int main(int argc, char** argv)
{ 

  if( (argc < 2) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0) && (strcmp("/dev/ttyS2", argv[1])!=0)
      && (strcmp("/dev/ttyS3", argv[1])!=0) && (strcmp("/dev/ttyS4", argv[1])!=0) && (strcmp("/dev/ttyS5", argv[1])!=0))) {
    printf("Usage: writer\tnserial SerialPort\tfilename \n\tex: writer /dev/ttyS1 pinguim.gif\n");
    exit(-1);
  }
  
  printf("Sending file...\n");
  
  struct timespec startTime;
  clock_gettime(CLOCK_REALTIME, &startTime);

  if(sendFile(argv[2], argv[1]) < 0){
      exit(-1);
  }
  
  struct timespec endTime;
  clock_gettime(CLOCK_REALTIME, &endTime);
  double sTime = startTime.tv_sec + startTime.tv_nsec * 1e-9;
  double eTime = endTime.tv_sec + endTime.tv_nsec * 1e-9;
  printf("-Time Passed: %.6lf-\n", eTime - sTime);
  printf("Done!\n");

}