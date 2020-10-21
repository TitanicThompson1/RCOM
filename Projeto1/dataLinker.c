#include "dataLinker.h"


int receiverNs = 0, emitterNr = 1;     //expected Ns by the receiver and expected Nr by the emitter
int lastNs = 1, lastNr = 0;             //las Ns sent by emitter and last Nr sent by receiver

int timeout_flag = 0, n_alarm = 0;
struct termios oldtio;

void count(){
    timeout_flag = 1;
    n_alarm++;
}


enum MessageType ReceiveMessage(int fd, byte *received_command){
    int current_state = STATE_START;
    enum MessageType ret;
    byte buf[1];

    while(current_state != STATE_STOP){

        if(ReadOneByte(fd, buf) == -1){
            return TIME_OUT;
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

            }else if(buf[0] == C_SET){                       //receiving UA message

                printf("C_SET read: %x\n", buf[0]);
                current_state = STATE_C;
                received_command[C_POS] = buf[0];
                ret = SET;

            }else if(buf[0] == C_UA){                       //receiving UA message

                printf("C_UA read: %x\n", buf[0]);
                current_state = STATE_C;
                received_command[C_POS] = buf[0];
                ret = UA;

            }else if (buf[0] == C_RR(emitterNr)){          //receiving RR message

                printf("C_RR read: %x\n", buf[0]);
                current_state = STATE_C;
                received_command[C_POS] = buf[0];
                ret = RR;

            }else if (buf[0] == C_REJ(emitterNr)){         //receiving REJ message

                printf("C_REJ read: %x\n", buf[0]);
                current_state = STATE_C;
                received_command[C_POS] = buf[0];
                ret = REJ;

            }else if (buf[0] == C_DISC){                    //receiving DISC message

                printf("C_DISC read: %x\n", buf[0]);
                current_state = STATE_C;
                received_command[C_POS] = buf[0];
                ret = DISC;

            }else{
                printf("Error in reading C. Received %x\n", buf[0]);
                current_state = STATE_START;
            }

        }else if(current_state == STATE_C){
            if(buf[0] == FLAG){
                printf("Received another flag\n");
                current_state = STATE_FLAG;

            }else if(ret == SET && buf[0] == BCC1(received_command[C_POS])){         // UA message

                printf("BCC1_SET read: %x\n", buf[0]);
                current_state = STATE_BCC1;
                received_command[BCC1_POS] = buf[0];

            }else if(ret == UA && buf[0] == BCC1(received_command[C_POS])){         // UA message

                printf("BCC1_UA read: %x\n", buf[0]);
                current_state = STATE_BCC1;
                received_command[BCC1_POS] = buf[0];

            }else if (ret == RR && buf[0] == BCC1(received_command[C_POS])){        // RR message

                printf("BCC1_RR read: %x\n", buf[0]);
                current_state = STATE_BCC1;
                received_command[BCC1_POS] = buf[0];
                
            }else if(ret == REJ && buf[0] == BCC1(received_command[C_POS])){        // REJ message

                printf("BCC1_REJ read: %x\n", buf[0]);
                current_state = STATE_BCC1;
                received_command[BCC1_POS] = buf[0];

            }else if(ret == DISC &&  buf[0] == BCC1(received_command[C_POS])){      // DISC message

                printf("BCC1_DISC read: %x\n", buf[0]);
                current_state = STATE_BCC1;
                received_command[BCC1_POS] = buf[0];

            }else {

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
    if(ret == REJ || ret == RR)
        updateEmitterNr();
    return ret;

}

int ReadOneByte(int fd, byte *command){

    if(read(fd, command, 1) == -1){
        
        if(timeout_flag == 1){
            timeout_flag = 0;
            return -1;
        }
        perror("ReadOneByte");
        return -1;
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

            }else if(buf[0] == C_I(receiverNs)){

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

            }else if(buf[0] == BCC1(C_I(receiverNs))){

                printf("BCC1_I read: %x\n", buf[0]);
                current_state = STATE_STOP;
                received_command[BCC1_POS] = buf[0];                

                int ret = ReceiveMessage(fd, received_command);
                if(ret == -1)
                    return -1;

                updateReceiverNs();                
                return 0;

            }else{
                printf("Error in reading BCC1_I. Received %x\n", buf[0]);
                current_state = STATE_START;
            }
        }
    }
    return 0;
}

int ReceiveIData(int fd, byte* received_command){
    byte buf[1];
    int currentPos = BCC1_POS + 1;
    
    byte currentXOR = received_command[BCC1_POS], previousByte = 0x00;

    ReadOneByte(fd,buf);
    while(buf[0] != FLAG){
        
        //Byte destuffing
        if(buf[0] == ESC){

            ReadOneByte(fd,buf);

            if(buf[0] == FLAG_ESC){
                received_command[currentPos++] = FLAG;

            }else if(buf[0] == ESC_ESC){
                received_command[currentPos++] = ESC;
            }

        }else{
            received_command[currentPos++] = buf[0];
        }        
        
        if(previousByte != 0x00)
            currentXOR = currentXOR ^ previousByte;
        
        previousByte = received_command[currentPos - 1];
        ReadOneByte(fd,buf);
    }
    //buf[0] stores the FLAG
    received_command[currentPos] = buf[0];

    //After the cycle ends, the BCC2 is stored in previousByte
    if(currentXOR != previousByte)
        return -1;
    printf("BCC2 received: %x\n", previousByte);
    printf("Flag read: %x\n", buf[0]);

    return 0;
}

void send_set_message(int fd){
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

void send_i_message(int fd, byte *msg, int n){    
    int res;
    byte command[255];

    command[0] = FLAG;
    command[1] = A;
    
    int currNs = updateLastNs();
    command[2] = C_I(currNs);
    command[3] = BCC1(command[2]);
    
    byte currentXOR = command[3];

    int i, j;
 
    printf("Size: %d\n", n);

    for(i = 4, j=0; j < n; i++, j++){      //i iterates over command array. j iterates over msg array
        
        //Byte Stuffing
        if(msg[j] == FLAG){                     
            
            command[i] = ESC;
            command[++i] = FLAG_ESC;
        }else if(msg[j] == ESC){
            
            command[i] = ESC;
            command[++i] = ESC_ESC;
        }else {

            command[i] = msg[j];            
        }
        currentXOR = currentXOR ^ msg[j];

    }
    
    command[i] = currentXOR;                    //BCC2
    command[++i] = FLAG;

    res = write(fd, command, i + 1);
    printf("%d bytes written\n", res);
    print_message("I command",command);
}

void send_disc_message(int fd){
    int res;
    byte disc_reply[5];

    disc_reply[0] = FLAG;
    disc_reply[1] = A;
    disc_reply[2] = C_DISC;
    disc_reply[3] = BCC1(C_DISC);
    disc_reply[4] = FLAG;

    res = write(fd, disc_reply, 5);
    printf("%d bytes written\n", res);

}

void send_ua_message(int fd){
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

void send_rr_message(int fd){
    int res;
    byte rr_reply[5];

    rr_reply[0] = FLAG;
    rr_reply[1] = A;
    int updated_nr = updateLastNr();
    rr_reply[2] = C_RR(updated_nr);
    rr_reply[3] = BCC1(rr_reply[2]);
    rr_reply[4] = FLAG;

    res = write(fd, rr_reply, 5);
    printf("%d bytes written\n", res);
}

void send_rej_message(int fd){
    int res;
    byte rej_reply[5];

    rej_reply[0] = FLAG;
    rej_reply[1] = A;
    int updated_nr = updateLastNr();
    rej_reply[2] = C_REJ(updated_nr);
    rej_reply[3] = BCC1(rej_reply[2]);
    rej_reply[4] = FLAG;

    res = write(fd, rej_reply, 5);
    printf("%d bytes written\n", res);
}

void print_message(char *before, byte *message){
    if( before != NULL)
        printf("%s ", before);
    
    printf("%x ",message[0]);
    int i = 1;
    while(message[i] != FLAG){
        printf("%x ",message[i]);
        i++;
    }
    printf("%x ",message[i]);
    printf("\n");
}

int updateLastNs(){
    int currentNs;
    if(lastNs == 0)
        currentNs = 1;
    else
        currentNs = 0;

    lastNs = currentNs;
    return currentNs;
}

int updateLastNr(){
    int currentNr;
    if(lastNr == 0)
        currentNr = 1;
    else
        currentNr = 0;
    lastNr = currentNr;
    return currentNr;
}

void updateReceiverNs(){
    
    if(receiverNs == 0)
        receiverNs = 1;
    else
        receiverNs = 0;
}

void updateEmitterNr(){

    if(emitterNr == 0)
        emitterNr = 1;
    else
        emitterNr = 0;
}

int llopen(char *port, int role){
    
    int fd = open_serialPort(port);
    
    set_costume_conf(fd);

    int res;

    if(role == TRANSMITTER)
        res = open_emitter(fd);
    else if(role == RECEIVER){
        res = open_receiver(fd);
    }else{
        printf("Unkwoned role.\n");
        res = -1;
    }

    if(res == -1 )
        return -1;
    
    return fd;
}

int open_serialPort(char *port){
    int fd;

    fd = open(port, O_RDWR | O_NOCTTY );
    if (fd <0 ) {perror("/dev/ttyS0"); return -1; }

    return fd;
}

void set_costume_conf(int fd){

    struct termios newtio;
     if (tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
        perror("tcgetattr");
        exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */


    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }
}

int reset_serialPort_conf(int fd, struct termios oldtio){
    if (tcsetattr(fd,TCSANOW,&oldtio) == -1) {
        perror("tcsetattr");
        return -1;
    } 
    return 0;
}

int open_emitter(int fd){
    byte received_message[255];

    signal(SIGALRM, count);
    siginterrupt(SIGALRM, 1);
    
    
    while(n_alarm < 3){

        send_set_message(fd);
        alarm(3);
        enum MessageType response = ReceiveMessage(fd, received_message);
        
        if(response == TIME_OUT){
            printf("Time_out occcured. Resending message.\n");

        }else if(response != UA){
            printf("Received message from wrong type.\n");
            continue;

        }else {         //Received UA message
            break;
        }
    }

    if(n_alarm ==3){
        printf("Couldn't establish connection.\n");
        return -1;
    }
    return 0;
}

int open_receiver(int fd){
    byte received_message[255];
    
    while(1){

        enum MessageType response = ReceiveMessage(fd, received_message);
        if(response != SET){
            printf("Received message from wrong type.\n");
            continue;
        }else{
            send_ua_message(fd);
            break;
        }
    }
    return 0;
}

int llclose(int fd, int role){
    int res;

    if(role == TRANSMITTER)
        res = close_emitter(fd);
    else if(role == RECEIVER){
        res = close_receiver(fd);
    }else{
        printf("Unkwoned role.\n");
        res = -1;
    }
    if(res == -1)
        return res;

    if(reset_serialPort_conf(fd, oldtio) == -1)
        return -1;

    close(fd);
    return 0;
    
}

int close_emitter(int fd){
    byte received_message[255];

    signal(SIGALRM, count);
    siginterrupt(SIGALRM, 1);
    
    
    while(n_alarm < 3){

        send_disc_message(fd);
        alarm(3);
        enum MessageType response = ReceiveMessage(fd, received_message);
        
        if(response == TIME_OUT){
            printf("Time_out occcured. Resending message.\n");

        }else if(response != DISC){
            printf("Received message from wrong type.\n");
            continue;
        }else{
            send_ua_message(fd);
            break;
        }
    }

    if(n_alarm ==3){
        printf("Couldn't establish connection.\n");
        return -1;
    }
    return 0;
}

int close_receiver(int fd){
    byte received_message[255];
    
    while(1){

        enum MessageType response = ReceiveMessage(fd, received_message);
        if(response != DISC){
            printf("Received message from wrong type.\n");
            continue;
        }

        send_disc_message(fd);

        response = ReceiveMessage(fd, received_message);
        if(response != UA){
            printf("Received message from wrong type.\n");
            continue;
        }else {
            break;
        }
    }
    return 0;
}