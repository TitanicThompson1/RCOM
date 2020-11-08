#include "application.h"
int DEBUG_MODE_A = 0;                     //0 indicates inactive. 1 is active. Debug mode does some extra printf


void activate_debug_a(void){
    DEBUG_MODE_A = 1;
}
void deactivate_debug_a(void){
    DEBUG_MODE_A = 0;
}

int sendFile(char * filename, char *port){
    
    unsigned int fileLength = 0;
    
    
    int fd = llopen(port, TRANSMITTER);
    
    if(fd < 0) return -1;

    //getting file length
    struct stat buffer; 
    if(fstat(fd, &buffer) != 0){         //int fstat(int fd, struct stat *buf);
        printf("Error on reading file information");
        return -1;
    }
    fileLength = buffer.st_size;        //st_size;    /* total size, in bytes */

    //Send START control field
    if(sendControlPacket(CONTROL_START, fileLength, filename, fd) < 0){
        return -1;
    }
    if(DEBUG_MODE_A) printf("Sent control start packet\n");

    FILE * file_to_send = fopen(filename, "rb");
    if (file_to_send == NULL){
        perror("fopen");
        return -1;
    }
    
    if(sendDataPackets(file_to_send, fd) < 0) return -1;
    if(DEBUG_MODE_A) printf("Sent all data\n");


    //Send END control field
    if(sendControlPacket(CONTROL_END, fileLength, filename, fd) < 0) return -1;
    if(DEBUG_MODE_A) printf("Sent control end packet\n");

    
    if(llclose(fd) < 0) return -1;
    if(DEBUG_MODE_A) printf("Closed communication\n");


    fclose(file_to_send);
    
    return 0;
}

int sendControlPacket(int controlType, unsigned int fileLength, char * filename, int fd){
    //index 0: CONTROL_START ou CONTROL_END
    //index 1: Type (0-> length of file, 1-> filename, ...)
    //index 2: Length (tamanho do campo a passar e.g. tamanho do nome do fichero etc)
    //index 3: Value (valor do campo a passar e.g. actual filename)

    byte controlPacket[MAX_DATA_D];

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
    int i=1;
    controlPacket[i++] = FILE_SIZE;
    controlPacket[i++] = 4;
    controlPacket[i++] = FOURTH_BYTE(fileLength);
    controlPacket[i++] = THIRD_BYTE(fileLength);
    controlPacket[i++] = SECOND_BYTE(fileLength);
    controlPacket[i++] = FIRST_BYTE(fileLength);

    controlPacket[i++] = FILE_NAME;               //Sending filename
    int size_filename = strlen(filename);
    
    if(size_filename + 9 > MAX_DATA_D){
        printf("Control packet is to big!\n");
        return -1;
    }

    controlPacket[i++] = size_filename; 
    int j=0;
    for (; j < size_filename; i++, j++){
        controlPacket[i] = (byte)filename[j];
    }


    if(llwrite(fd, controlPacket, i) < 0){
        return -1;  
    }
    return 0;
}

int sendDataPackets(FILE *file, int fd){
    byte dataPacket[MAX_DATA_D];
    int sequenceNumber = 0; 

    dataPacket[0] = DATA_PACK;
    dataPacket[1] = sequenceNumber++;
    dataPacket[2] = SECOND_BYTE((MAX_DATA_D - 4));
    dataPacket[3] = FIRST_BYTE((MAX_DATA_D - 4));                   

    byte buf[1];
    int ret = fread(buf, 1, 1, file), i = 4, size = 1;
    if(ret != 1){
        perror("fread");
        return -1;
    }
    dataPacket[i++] = buf[0]; 
    
    while (1){
        //Reading one byte per read
        
        ret = fread(buf, 1, 1, file);
        if(ret != 1) break;    
        dataPacket[i++] = buf[0];
        size++;

        if(i == MAX_DATA_D){
            
            if(llwrite(fd, dataPacket, i) == -1) return -1;
            if(DEBUG_MODE_A) printf("Sent %dº packet\n", sequenceNumber);    
            size = 0;
            memset(dataPacket, 0, MAX_DATA_D);
            
            sequenceNumber = sequenceNumber % 255;
            dataPacket[0] = DATA_PACK;
            dataPacket[1] = sequenceNumber++;
            dataPacket[2] = SECOND_BYTE((MAX_DATA_D - 4));
            dataPacket[3] = FIRST_BYTE((MAX_DATA_D - 4));                   
            i = 4;
        }
    }
    
    if(size != 0){
        dataPacket[2] = SECOND_BYTE(size);
        dataPacket[3] = FIRST_BYTE(size);

        if(DEBUG_MODE_A) printf("Final packet size: %d\n", size);
        if(llwrite(fd, dataPacket, i) < 0) return -1;
        if(DEBUG_MODE_A) printf("Sent %dº packet\n", sequenceNumber);    

    }

    return 0;   
}

int receiveFile(char *port){
    int fd = llopen(port, RECEIVER);
    char filename[255];

    if(readControlPacket(fd, filename) < 0) return -1;
    if(DEBUG_MODE_A) printf("Read contorl start packet\n");

    FILE *file;
    file = fopen(filename, "wb");
    if(file == NULL){
        perror("fopen");
        return -1;
    }

    if(readDataPackets(fd,  file) < 0) return -1;
    if(DEBUG_MODE_A) printf("Read all data packets\n");


    llclose(fd);
    if(DEBUG_MODE_A) printf("Closed communication\n");


    fclose(file);

    return 0;
}

int readControlPacket(int fd, char* filename){
    //going to call llread

    byte controlPacket[MAX_DATA_D];
    unsigned int size_filename;
    byte size_file[4];
    unsigned int packetSize = 0, has_filename = 0, has_filesize = 0;    

    if(llread(fd, controlPacket) < 0){
        printf("Error on reading Start Control Packet\n");
        return -1;
    }

    //checks for start control packet
    if(controlPacket[packetSize++] != CONTROL_START){
        printf("Error on reading Start Control Packet\n");
        return -1;    
    }
  

    while(has_filename == 0 || has_filesize == 0){
        //checks for type of info   
        if(controlPacket[packetSize] == FILE_NAME){
            packetSize++;
            //size of filename
            size_filename = controlPacket[packetSize++];
                
            //reading filename
            int i = 0; 
            for(; i < size_filename; i++){
                filename[i] = controlPacket[packetSize++];
            }
            filename[i] = '\0';
            has_filename = 1;
        }
        else if(controlPacket[packetSize] == FILE_SIZE){
            packetSize++;
            int j = 0, n = controlPacket[packetSize++];     //n is the size of the V field
            for(; j < n; j++){
                size_file[j] = controlPacket[packetSize++];
            }
            unsigned int file_size = size_file[0] << 24 | size_file[1] << 16 | size_file[2] << 8 | size_file[3];  
            if(file_size){
                //...
            }
            has_filesize = 1;
        }
        else {
            printf("Invalid type of info: %x\n", controlPacket[packetSize]);
            return -1;
        }
    }  
    return 0;   
}

int readDataPackets(int fd, FILE *file){

    byte dataPacket[MAX_DATA_D], buf[1];
    unsigned int size = 0;
    if(llread(fd, dataPacket) < 0) return -1;
    int numP = 1;
    
    while(dataPacket[0] != CONTROL_END && dataPacket[0] != CONTROL_START){
        size = dataPacket[2] << 8 | dataPacket[3];
        if(DEBUG_MODE_A) printf("Data packet size: %d\n", size);  

        for (int i = 0; i < size; i++){
            buf[0] = (char) dataPacket[i + 4];
            if(fwrite(buf, 1, 1, file) != 1){
                perror("fwrite");
                return -1;
            }
        }
        if(DEBUG_MODE_A) printf("Received %dº packet\n", numP++); 
        memset(dataPacket, 0, MAX_DATA_D);
        if(llread(fd, dataPacket) < 0) return -1;        
    }
    
    return 0;
}
