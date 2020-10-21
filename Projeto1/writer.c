
#include "dataLinker.h"

int main(int argc, char** argv)
{
    int fd = llopen(argv[1], TRANSMITTER);

    printf("Connection established!\n");

    sleep(2);

    llclose(fd,TRANSMITTER);

    printf("Connection closed!\n");
  
    return 0;
}