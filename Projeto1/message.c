#include "message.h"


int expectedNs = 0, expectedNr = 1;     //expected Ns by the receiver and expected Nr by the emitter
int lastNs = 1, lastNr = 0;             //las Ns sent by emitter and last Nr sent by receiver

int ReceiveCommand(int fd, byte* received_command, enum MessageType message){

    switch (message)
    {
    case UA:
        if(ReceiveUA(fd, received_command) == -1){
            return -1;
        }
        break;
    
    case I:
        
        ReceiveI(fd, received_command);
        
        
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
    return 0;
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

            }else if(buf[0] == C_RR(expectedNr)){

                printf("C_RR read: %x\n", buf[0]);
                current_state = STATE_C;
                received_command[C_POS] = buf[0];
            }else{
                printf("Error in reading C_RR. Received %x\n", buf[0]);
                current_state = STATE_START;
            }

        }else if(current_state == STATE_C){
            if(buf[0] == FLAG){
                printf("Received another flag\n");
                current_state = STATE_FLAG;

            }else if(buf[0] == BCC1(C_RR(expectedNr))){

                printf("BCC1_RR read: %x\n", buf[0]);
                current_state = STATE_BCC1;
                received_command[BCC1_POS] = buf[0];
                

            }else{
                printf("Error in reading BCC1_RR. Received %x\n", buf[0]);
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
    return 0;
}

int ReadOneByte(int fd, byte *command){

    if(read(fd, command, 1) == -1){
        //TODO
        /*if( timeout_flag == 1){
            timeout_flag = 0;
            return -1;
        }*/
        perror("ReadOneByte");
        exit(-1);
    }/*
    if(read == 0 || timeout_flag == 1){
        timeout_flag = 0;
        return -1;
    }*/
    return 0;
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

            }else if(buf[0] == C_DISC){

                printf("C_DISC read: %x\n", buf[0]);
                current_state = STATE_C;
                received_command[C_POS] = buf[0];
            }else{
                printf("Error in reading C_DISC. Received %x\n", buf[0]);
                current_state = STATE_START;
            }

        }else if(current_state == STATE_C){
            if(buf[0] == FLAG){
                printf("Received another flag\n");
                current_state = STATE_FLAG;

            }else if(buf[0] == BCC1(C_DISC)){

                printf("BCC1_DISC read: %x\n", buf[0]);
                current_state = STATE_BCC1;
                received_command[BCC1_POS] = buf[0];
            }else{
                printf("Error in reading BCC1_DISC. Received %x\n", buf[0]);
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
    return 0;
}

int ReceiveI(int fd, byte *received_command){
    
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

            }else if(buf[0] == C_I(expectedNs)){

                printf("C_I read: %x\n", buf[0]);
                current_state = STATE_C;
                received_command[C_POS] = buf[0];
            }else{
                printf("Error in reading C_. Received %x\n", buf[0]);
                current_state = STATE_START;
            }

        }else if(current_state == STATE_C){

            if(buf[0] == FLAG){
                printf("Received another flag\n");
                current_state = STATE_FLAG;

            }else if(buf[0] == BCC1(C_I(expectedNs))){

                printf("BCC1_I read: %x\n", buf[0]);
                current_state = STATE_STOP;
                received_command[BCC1_POS] = buf[0];

                

                int ret = ReceiveMessage(fd, received_command);
                if(ret == -1)
                    return -1;
                return 0;

            }else{
                printf("Error in reading BCC1_I. Received %x\n", buf[0]);
                current_state = STATE_START;
            }
        }
    }
    return 0;
}

int ReceiveMessage(int fd, byte* received_command){
    byte buf[1];
    int currentPos = BCC1_POS + 1;
    
    byte currentXOR = received_command[BCC1_POS], previousByte = 0x00;

    ReadOneByte(fd,buf);
    while(buf[0] != FLAG){
        
        
        received_command[currentPos++] = buf[0];
        if(previousByte != 0x00)
            currentXOR = currentXOR ^ previousByte;

        previousByte = buf[0];
        ReadOneByte(fd,buf);
        

    }
    received_command[currentPos] = buf[0];

    //After the cycle ends, the BCC2 is stored in previousByte
    if(currentXOR != previousByte)
        return -1;
    printf("BCC2 received: %x\n", previousByte);
    printf("Flag read: %x\n", buf[0]);

    return 0;
}

void send_set_command(int fd){
    int res;

    byte command[5];
    command[0] = FLAG;
    command[1] = A;
    command[2] = C_SET;
    command[3] = BCC1(C_SET);
    command[4] = FLAG;

    res = write(fd, command, 5);
    printf("%d bytes written\n", res);
}

void send_i_command(int fd, byte *msg, int n){    
    int res;
    byte command[255];

    command[0] = FLAG;
    command[1] = A;
    
    command[2] = C_I(expectedNs);
    int currNs = updateNs();
    command[3] = BCC1(command[2]);
    
    byte currentXOR = command[3];

    int i = 0;
 
    printf("Size: %d\n", n);

    for(i = 0; i < n; i++){
        command[i+4] = msg[i];
        currentXOR = currentXOR ^ msg[i];
    }
    command[i+4] = currentXOR;
    command[i+5] = FLAG;

    res = write(fd, command, i+6);
    printf("%d bytes written\n", res);
    print_message(command);
}   

void send_ua_command(int fd){
    int res;
    byte ua_reply[5];

    ua_reply[0] = FLAG;
    ua_reply[1] = A;
    ua_reply[2] = C_UA;
    ua_reply[3] = BCC1(C_UA);
    ua_reply[4] = FLAG;

    res = write(fd, ua_reply, 5);
    printf("%d bytes written\n", res);
}

void print_message(byte *message){
    printf("%x ",message[0]);
    int i = 1;
    while(message[i] != FLAG){
        printf("%x ",message[i]);
        i++;
    }
    printf("%x ",message[i]);
    printf("\n");
}

int updateNs(){
    int currentNs;
    if(lastNs == 0)
        currentNs = 1;
    else
        currentNs = 0;
    lastNs = currentNs;
    expectedNs = currentNs;
    return currentNs;
}

int updateNr(){
    int currentNr;
    if(lastNs == 0)
        currentNr = 1;
    else
        currentNr = 0;
    lastNr = currentNr;
    expectedNr = currentNr;

    return currentNr;
}


