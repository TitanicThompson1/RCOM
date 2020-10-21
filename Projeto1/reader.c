
#include "dataLinker.h"

int main(int argc, char** argv)
{
    int fd = llopen(argv[1], RECEIVER);
    printf("Connection established!\n");
    
    sleep(2);
    
    llclose(fd, RECEIVER);
    printf("Connection closed!\n");
    return 0;
}