
#include "application.h"

int main(int argc, char** argv)
{   

  if( (argc < 2) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0) && (strcmp("/dev/ttyS2", argv[1])!=0)
      && (strcmp("/dev/ttyS3", argv[1])!=0) && (strcmp("/dev/ttyS4", argv[1])!=0) && (strcmp("/dev/ttyS5", argv[1])!=0))) {
    printf("Usage: reader\tnserial SerialPort\n\tex: reader /dev/ttyS1\n");
    exit(-1);
  }

  printf("Receiving file...\n");

  FILE *log;
  log = fopen("writerLOG.txt", "w");
  clock_t start, end;
  double cpu_time_used;
     
  start = clock();

  if(receiveFile(argv[1]) < 0){
      exit(-1);
  }
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

  fwrite("Time: ", 1, 6, log);
  fprintf(log,"%f", cpu_time_used);
  printf("Done!\n");
  fclose(log);
}