#include "application.h"

int sendFile(int fd, char * filename, int port){
    
    int fileLength = 0;
    
    llopen(fd, TRANSMITTER);

    //getting file length
    struct stat buffer; 
    if(fstat(fd, buffer) != 0){         //int fstat(int fd, struct stat *buf);
        printf("Error on reading file information");
        return -1;
    }
    fileLength = buffer.st_size;        //st_size;    /* total size, in bytes */

    //Send START control field
    sendControlPacket(CONTROL_START, fileLength, filename, port);

    createDataPackets();

    sendDataPackets();

    //Send END control field
    sendControlPacket();

}

int sendControlPacket(int controlType, int fileLength, char * filename, int port){
    //index 0: CONTROL_START ou CONTROL_END
    //index 1: Type (0-> length of file, 1-> filename, ...)
    //index 2: Length (tamanho do campo a passar e.g. tamanho do nome do fichero etc)
    //index 3: Value (valor do campo a passar e.g. actual filename)

    byte controlPacket[255];
    int controlPacketLength = 0;

    if(controlType == CONTROL_START){
        controlPacket[0] = CONTROL_START;
        
    }
    else if(controlType == CONTROL_END){
        controlPacket[0] = CONTROL_END;
    }
    else{
        printf("Invalid control type");
        return -1;
    }


    llwrite(port, controlPacket, controlPacketLength);
}

int createDataPacket(){
    //index 0 : CONTROL_DATA
    //index 1: o que é o número de sequência??
    //index 2: length
    //index 3: actual data

    byte dataPacket[255];
    int dataPacketLength = 0;
    int sequenceNumber = 0; //??? não faço ideia o que é isto :)

    //é preciso calcular o tamanho dum data packet, no enunciado diz que depende do tamanho máximo do campo de informação da trama I ??

    dataPacket[0] = CONTROL_DATA;
    dataPacket[1] = sequenceNumber;
    dataPacket[2] = dataPacketLength;

}

int sendDataPackets(){
    //going to call llwrite
}

int receiveFile(int fd){

    readControlPacket();
    readDataPackets();
}

int readControlPacket(){
    //going to call llread
}

int readDataPackets(){
    //going to call llread
}
