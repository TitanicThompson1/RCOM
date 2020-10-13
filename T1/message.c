#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "message.h"

int Ns = 1, Nr = 0;


int ReceiveCommand(int fd, byte *received_command, enum MessageType message){

    switch (message)
    {
    case UA:
        if(ReceiveUA(fd, received_command) == -1){
            return -1;
        }
        break;
    
    case I:
        char* message[255];
        ReceiveI(fd, received_command, message);
        
        
        break;

    case DISC:
        if(ReceiveDISC(fd, received_command) == -1){
            return -1;
        }
        break;

    default:
        break;
    }

    return 0;

}

int ReceiveUA(int fd, byte *received_command){

    int current_state = STATE_START;
    byte buf[1];

    while(current_state != STATE_STOP){

        if(ReadOneByte(fd, buf) == -1){
            return -1;
        }

        if(current_state == STATE_START){

            if(buf[0] != FLAG){
                printf("Error in reading FLAG: %x\n", buf[0]);

            }else{
                printf("Flag read: %x\n", buf[0]);

                current_state = STATE_FLAG;
                received_command[FLAG1_POS] = buf[0];
            }

        }else if(current_state == STATE_FLAG){

            if(buf[0] == FLAG){
                printf("Received another flag\n");

            }else if(buf[0] == A){

                printf("A read: %x\n", buf[0]);
                current_state = STATE_A;
                received_command[A_POS] = buf[0];
            }else{
                printf("Error in reading A. Received %x\n", buf[0]);
                current_state = STATE_START;
            }

        }else if(current_state == STATE_A){
            if(buf[0] == FLAG){
                printf("Received another flag\n");
                current_state = STATE_FLAG;

            }else if(buf[0] == C_UA){

                printf("C_UA read: %x\n", buf[0]);
                current_state = STATE_C;
                received_command[C_POS] = buf[0];
            }else{
                printf("Error in reading C. Received %x\n", buf[0]);
                current_state = STATE_START;
            }

        }else if(current_state == STATE_C){
            if(buf[0] == FLAG){
                printf("Received another flag\n");
                current_state = STATE_FLAG;

            }else if(buf[0] == BCC1(UA)){

                printf("BCC1_UA read: %x\n", buf[0]);
                current_state = STATE_BCC1;
                received_command[BCC1_POS] = buf[0];
            }else{
                printf("Error in reading BCC1_UA. Received %x\n", buf[0]);
                current_state = STATE_START;
            }
        }else if(current_state == STATE_BCC1){

            if(buf[0] == FLAG){
                printf("Flag read: %x\n", buf[0]);
                received_command[FLAG2_POS] = buf[0];
                current_state = STATE_STOP;

            }else{
                printf("Error in reading FLAG. Received %x\n", buf[0]);
                current_state = STATE_START;
            }
        }
    }
}

int ReceiveRR(int fd, byte *received_command){

    int current_state = STATE_START;
    byte buf[1];

    while(current_state != STATE_STOP){

        if(ReadOneByte(fd, buf) == -1){
            return -1;
        }

        if(current_state == STATE_START){

            if(buf[0] != FLAG){
                printf("Error in reading FLAG: %x\n", buf[0]);

            }else{
                printf("Flag read: %x\n", buf[0]);

                current_state = STATE_FLAG;
                received_command[FLAG1_POS] = buf[0];
            }

        }else if(current_state == STATE_FLAG){

            if(buf[0] == FLAG){
                printf("Received another flag\n");

            }else if(buf[0] == A){

                printf("A read: %x\n", buf[0]);
                current_state = STATE_A;
                received_command[A_POS] = buf[0];
            }else{
                printf("Error in reading A. Received %x\n", buf[0]);
                current_state = STATE_START;
            }

        }else if(current_state == STATE_A){
            if(buf[0] == FLAG){
                printf("Received another flag\n");
                current_state = STATE_FLAG;

            }else if(buf[0] == C_UA){

                printf("C_UA read: %x\n", buf[0]);
                current_state = STATE_C;
                received_command[C_POS] = buf[0];
            }else{
                printf("Error in reading C. Received %x\n", buf[0]);
                current_state = STATE_START;
            }

        }else if(current_state == STATE_C){
            if(buf[0] == FLAG){
                printf("Received another flag\n");
                current_state = STATE_FLAG;

            }else if(buf[0] == BCC1(C_RR(Ns))){

                printf("BCC1_UA read: %x\n", buf[0]);
                current_state = STATE_BCC1;
                received_command[BCC1_POS] = buf[0];
                updateN(&Ns);

            }else{
                printf("Error in reading BCC1_UA. Received %x\n", buf[0]);
                current_state = STATE_START;
            }
        }else if(current_state == STATE_BCC1){

            if(buf[0] == FLAG){
                printf("Flag read: %x\n", buf[0]);
                received_command[FLAG2_POS] = buf[0];
                current_state = STATE_STOP;

            }else{
                printf("Error in reading FLAG. Received %x\n", buf[0]);
                current_state = STATE_START;
            }
        }
    }
}

int ReadOneByte(int fd, byte *command){

    if(read(fd, command, 1) == -1){
        perror("ReadOneByte");
        exit(-1);
    }/*
    if(read == 0 || timeout_flag == 1){
        timeout_flag = 0;
        return -1;
    }*/
    return 0;
}

void updateN(int *n){
    if(n == 0)
        n = 1;
    else
        n = 0;
}

int ReceiveDISC(int fd, byte *received_command){

    int current_state = STATE_START;
    byte buf[1];

    while(current_state != STATE_STOP){

        if(ReadOneByte(fd, buf) == -1){
            return -1;
        }

        if(current_state == STATE_START){

            if(buf[0] != FLAG){
                printf("Error in reading FLAG: %x\n", buf[0]);

            }else{
                printf("Flag read: %x\n", buf[0]);

                current_state = STATE_FLAG;
                received_command[FLAG1_POS] = buf[0];
            }

        }else if(current_state == STATE_FLAG){

            if(buf[0] == FLAG){
                printf("Received another flag\n");

            }else if(buf[0] == A){

                printf("A read: %x\n", buf[0]);
                current_state = STATE_A;
                received_command[A_POS] = buf[0];
            }else{
                printf("Error in reading A. Received %x\n", buf[0]);
                current_state = STATE_START;
            }

        }else if(current_state == STATE_A){
            if(buf[0] == FLAG){
                printf("Received another flag\n");
                current_state = STATE_FLAG;

            }else if(buf[0] == C_UA){

                printf("C_UA read: %x\n", buf[0]);
                current_state = STATE_C;
                received_command[C_POS] = buf[0];
            }else{
                printf("Error in reading C. Received %x\n", buf[0]);
                current_state = STATE_START;
            }

        }else if(current_state == STATE_C){
            if(buf[0] == FLAG){
                printf("Received another flag\n");
                current_state = STATE_FLAG;

            }else if(buf[0] == BCC1(C_RR(0))){

                printf("BCC1_UA read: %x\n", buf[0]);
                current_state = STATE_BCC1;
                received_command[BCC1_POS] = buf[0];
            }else{
                printf("Error in reading BCC1_UA. Received %x\n", buf[0]);
                current_state = STATE_START;
            }
        }else if(current_state == STATE_BCC1){

            if(buf[0] == FLAG){
                printf("Flag read: %x\n", buf[0]);
                received_command[FLAG2_POS] = buf[0];
                current_state = STATE_STOP;

            }else{
                printf("Error in reading FLAG. Received %x\n", buf[0]);
                current_state = STATE_START;
            }
        }
    }
}

void send_set_command(int fd, byte* command){
    int res;

    command[0] = FLAG;
    command[1] = A;
    command[2] = C_SET;
    command[3] = BCC1(SET);
    command[4] = FLAG;

    res = write(fd, command, 5);
    printf("%d bytes written\n", res);
}

void print_message(byte *message){
    printf("%x %x %x %x %x\n", message[0], message[1], message[2], message[3], message[4]);
}

void send_I_command(int fd, char * msg){    
    int res;
    byte command[255];

    command[0] = FLAG;
    command[1] = A;
    command[2] = C_I(Ns);
    updateN(Ns);
    command[3] = BCC1(SET);
    
    int i = 0;
    for(i = 0; i < strlen(msg); i++){
        command[i+4] = msg[i];
    }

    command[i+5] = FLAG;
    command[i+6] = '\0';

    res = write(fd, command, strlen(command));
    printf("%d bytes written\n", res);
}   

void send_UA(int fd, byte* ua_reply){
    int res;

    ua_reply[0] = FLAG;
    ua_reply[1] = A;
    ua_reply[2] = C_UA;
    ua_reply[3] = BCC1(C_UA);
    ua_reply[4] = FLAG;

    res = write(fd, ua_reply, 5);
    printf("%d bytes written\n", res);
}

void ReceiveI(int fd, byte *received_command, char* message){
    
    int current_state = STATE_START;
    byte buf[1];

    while(current_state != STATE_STOP){

        if(ReadOneByte(fd, buf) == -1){
            return -1;
        }

        if(current_state == STATE_START){

            if(buf[0] != FLAG){
                printf("Error in reading FLAG: %x\n", buf[0]);

            }else{
                printf("Flag read: %x\n", buf[0]);

                current_state = STATE_FLAG;
                received_command[FLAG1_POS] = buf[0];
            }

        }else if(current_state == STATE_FLAG){

            if(buf[0] == FLAG){
                printf("Received another flag\n");

            }else if(buf[0] == A){

                printf("A read: %x\n", buf[0]);
                current_state = STATE_A;
                received_command[A_POS] = buf[0];
            }else{
                printf("Error in reading A. Received %x\n", buf[0]);
                current_state = STATE_START;
            }

        }else if(current_state == STATE_A){
            if(buf[0] == FLAG){
                printf("Received another flag\n");
                current_state = STATE_FLAG;

            }else if(buf[0] == C_UA){

                printf("C_UA read: %x\n", buf[0]);
                current_state = STATE_C;
                received_command[C_POS] = buf[0];
            }else{
                printf("Error in reading C. Received %x\n", buf[0]);
                current_state = STATE_START;
            }

        }else if(current_state == STATE_C){
            if(buf[0] == FLAG){
                printf("Received another flag\n");
                current_state = STATE_FLAG;

            }else if(buf[0] == BCC1(C_RR(Ns))){

                printf("BCC1_UA read: %x\n", buf[0]);
                current_state = STATE_BCC1;
                received_command[BCC1_POS] = buf[0];

                char message[255];
                ReceiveData(fd, message);

                updateN(&Ns);

            }else{
                printf("Error in reading BCC1_UA. Received %x\n", buf[0]);
                current_state = STATE_START;
            }
        }else if(current_state == STATE_BCC1){
            
        }
    }
}

void ReceiveData(int fd, char *message){
    char buf[1];
    int i = 0;
    //TODO
}
