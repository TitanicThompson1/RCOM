
#include "dataLinker.h"

int main(int argc, char** argv)
{   
    int fd = llopen(argv[1], TRANSMITTER);
    if (fd < 0){
        printf("Couldn't establish connection.\n");
        exit(-1);
    }
    printf("Connection established!\n");
    byte msg[5] = {0x01, 0x7e, 0x7d, 0x02, 0x03};
    

    llwrite(fd, msg, 5);
    printf("Sent message!\n");

    llwrite(fd, msg, 5);
    printf("Sent message!\n");

    llwrite(fd, msg, 5);
    printf("Sent message!\n");



    int ret = llclose(fd);
    if(ret == -1){
        printf("Couldn't close connection.\n");
        exit(-1);
    }
    printf("Connection closed!\n");
    return 0;
}