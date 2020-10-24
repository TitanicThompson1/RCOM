
#include "dataLinker.h"

int main(int argc, char** argv)
{   
    int fd = llopen(argv[1], RECEIVER);
    printf("Connection established!\n");
    byte msg[255];

    int ret = llread(fd, msg);
    if(ret >= 0)
        print_message("Received message! ", msg, ret);  
    else{
        printf("Error in reading.\n");
        exit(-1);
    }
    
    ret = llread(fd, msg);
    if(ret >= 0)
        print_message("Received message! ", msg, ret);  
    else{
        printf("Error in reading.\n");
        exit(-1);
    }
    
    ret = llread(fd, msg);
    if(ret >= 0)
        print_message("Received message! ", msg, ret);  
    else{
        printf("Error in reading.\n");
        exit(-1);
    }
    
    llclose(fd);
    printf("Connection closed!\n");
    return 0;
}