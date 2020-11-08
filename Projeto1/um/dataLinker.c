#include "dataLinker.h"
#define DELAY 1

int DEBUG_MODE = 0;                     //0 indicates inactive. 1 is active. Debug mode does some extra printf
int ROLE = -1;                          //0 for TRANSMITER, 1 for RECEIVER

int emitterNr = 1, receiverNs = 0;      //expected Ns by the receiver and expected Nr by the emitter
int currentNs = 0, currentNr = 1;       //Ns to be sent by emitter and Nr to be sent by receiver

int timeout_flag = 0, n_alarm = 0, first = 1, size_previous = 0;
struct termios oldtio;
byte previous_msg[MAX_DATA_D];

void count(){
    timeout_flag = 1;
    n_alarm++;
}

void activate_debug_d(void){
    DEBUG_MODE = 1;
}
void deactivate_debug_d(void){
    DEBUG_MODE = 0;
}


enum MessageType ReceiveMessage(int fd, byte *received_message){
    int current_state = STATE_START;
    enum MessageType ret;
    byte buf[1];
    
    while(current_state != STATE_STOP){
        

        int ROBres = ReadOneByte(fd, buf);
        if(ROBres == -2){
            return TIME_OUT;
        }else if(ROBres == -1){
            return ERROR;
        }
      

        if(current_state == STATE_START){

            if(buf[0] != FLAG){
                if(DEBUG_MODE) printf("Error in reading FLAG: %x\n", buf[0]);

            }else{
                if(DEBUG_MODE) printf("Flag read: %x\n", buf[0]);

                current_state = STATE_FLAG;
                received_message[FLAG1_POS] = buf[0];
            }

        }else if(current_state == STATE_FLAG){

            if(buf[0] == FLAG){
                if(DEBUG_MODE) printf("Received another flag\n");

            }else if(buf[0] == A){

                if(DEBUG_MODE) printf("A read: %x\n", buf[0]);
                current_state = STATE_A;
                received_message[A_POS] = buf[0];
            }else{
                if(DEBUG_MODE) printf("Error in reading A. Received %x\n", buf[0]);
                current_state = STATE_START;
            }

        }else if(current_state == STATE_A){
            if(buf[0] == FLAG){
                if(DEBUG_MODE) printf("Received another flag\n");
                current_state = STATE_FLAG;

            }else if(buf[0] == C_SET){                       //receiving UA message

                if(DEBUG_MODE) printf("C_SET read: %x\n", buf[0]);
                current_state = STATE_C;
                received_message[C_POS] = buf[0];
                ret = SET;

            }else if(buf[0] == C_UA){                       //receiving UA message

                if(DEBUG_MODE) printf("C_UA read: %x\n", buf[0]);
                current_state = STATE_C;
                received_message[C_POS] = buf[0];
                ret = UA;

            }else if (buf[0] == C_RR(emitterNr)){          //receiving RR message

                if(DEBUG_MODE) printf("C_RR read: %x\n", buf[0]);
                current_state = STATE_C;
                received_message[C_POS] = buf[0];
                ret = RR;

            }else if (buf[0] == C_REJ(emitterNr)){         //receiving REJ message

                if(DEBUG_MODE) printf("C_REJ read: %x\n", buf[0]);
                current_state = STATE_C;
                received_message[C_POS] = buf[0];
                ret = REJ;

            }else if(buf[0] == C_RR(emitterNr == 1 ? 0 : 1)){
                
                if(DEBUG_MODE) printf("C_RR read: %x\n", buf[0]);
                current_state = STATE_C;
                received_message[C_POS] = buf[0];
                ret = RR_REPEATED;

            }else if (buf[0] == C_DISC){                    //receiving DISC message

                if(DEBUG_MODE) printf("C_DISC read: %x\n", buf[0]);
                current_state = STATE_C;
                received_message[C_POS] = buf[0];
                ret = DISC;

            }else{
                if(DEBUG_MODE) printf("Error in reading C. Received %x\n", buf[0]);
                current_state = STATE_START;
            }

        }else if(current_state == STATE_C){
            if(buf[0] == FLAG){
                if(DEBUG_MODE) printf("Received another flag\n");
                current_state = STATE_FLAG;

            }else if(ret == SET && buf[0] == BCC1(received_message[C_POS])){         // UA message

                if(DEBUG_MODE) printf("BCC1_SET read: %x\n", buf[0]);
                current_state = STATE_BCC1;
                received_message[BCC1_POS] = buf[0];

            }else if(ret == UA && buf[0] == BCC1(received_message[C_POS])){         // UA message

                if(DEBUG_MODE) printf("BCC1_UA read: %x\n", buf[0]);
                current_state = STATE_BCC1;
                received_message[BCC1_POS] = buf[0];

            }else if (ret == RR && buf[0] == BCC1(received_message[C_POS])){        // RR message

                if(DEBUG_MODE) printf("BCC1_RR read: %x\n", buf[0]);
                current_state = STATE_BCC1;
                received_message[BCC1_POS] = buf[0];
                
            }else if(ret == REJ && buf[0] == BCC1(received_message[C_POS])){        // REJ message

                if(DEBUG_MODE) printf("BCC1_REJ read: %x\n", buf[0]);
                current_state = STATE_BCC1;
                received_message[BCC1_POS] = buf[0];

            }else if(ret == DISC &&  buf[0] == BCC1(received_message[C_POS])){      // DISC message

                if(DEBUG_MODE) printf("BCC1_DISC read: %x\n", buf[0]);
                current_state = STATE_BCC1;
                received_message[BCC1_POS] = buf[0];

            }else {

                if(DEBUG_MODE) printf("Error in reading BCC1_UA. Received %x\n", buf[0]);
                current_state = STATE_START;
            }
        }else if(current_state == STATE_BCC1){

            if(buf[0] == FLAG){
                if(DEBUG_MODE) printf("Flag read: %x\n", buf[0]);
                received_message[FLAG2_POS] = buf[0];
                current_state = STATE_STOP;

            }else{
                if(DEBUG_MODE) printf("Error in reading FLAG. Received %x\n", buf[0]);
                current_state = STATE_START;
            }
        }
    }
    
    return ret;

}

int ReadOneByte(int fd, byte *command){

    if(read(fd, command, 1) == -1){
        
        if(timeout_flag == 1){
            timeout_flag = 0;
            return -2;
        }
        perror("ReadOneByte");
        return -1;
    }
    return 0;
}

int ReceiveI(int fd, byte *result){

    byte received_message[FRAME_SIZE];
    int current_state = STATE_START;
    byte buf[1];
       
    while(current_state != STATE_STOP){

        if(ReadOneByte(fd, buf)){
            return -1;
        }
       
        if(current_state == STATE_START){

            if(buf[0] != FLAG){
                if(DEBUG_MODE) printf("Error in reading FLAG: %x\n", buf[0]);

            }else{
                if(DEBUG_MODE) printf("Flag read: %x\n", buf[0]);
                current_state = STATE_FLAG;
                received_message[FLAG1_POS] = buf[0];
            }

        }else if(current_state == STATE_FLAG){

            if(buf[0] == FLAG){
                if(DEBUG_MODE) printf("Received another flag\n");

            }else if(buf[0] == A){

                if(DEBUG_MODE) printf("A read: %x\n", buf[0]);
                current_state = STATE_A;
                received_message[A_POS] = buf[0];
            }else{
                if(DEBUG_MODE) printf("Error in reading A. Received %x\n", buf[0]);
                current_state = STATE_START;
            }

        }else if(current_state == STATE_A){
            if(buf[0] == FLAG){
                if(DEBUG_MODE) printf("Received another flag\n");
                current_state = STATE_FLAG;

            }else if(buf[0] == C_I(receiverNs)){

                if(DEBUG_MODE) printf("C_I read: %x\n", buf[0]);
                current_state = STATE_C;
                received_message[C_POS] = buf[0];

            }else if (buf[0] == C_I(receiverNs == 1 ? 0 : 1)){

                if(DEBUG_MODE) printf("Received same message\n");
                return -2;


            }else{
                if(DEBUG_MODE) printf("Error in reading C_I. Received %x\n", buf[0]);
                current_state = STATE_START;
            }

        }else if(current_state == STATE_C){

            if(buf[0] == FLAG){
                if(DEBUG_MODE) printf("Received another flag\n");
                current_state = STATE_FLAG;

            }else if(buf[0] == BCC1(C_I(receiverNs))){

                if(DEBUG_MODE) printf("BCC1_I read: %x\n", buf[0]);
                current_state = STATE_STOP;
                received_message[BCC1_POS] = buf[0];                

                int ret = ReceiveIData(fd, received_message, result);
                if(ret == -1)
                    return -1;

                return ret;

            }else{
                if(DEBUG_MODE) printf("Error in reading BCC1_I. Received %x\n", buf[0]);
                current_state = STATE_START;
            }
        }
    }
    return 0;
}

int ReceiveIData(int fd, byte* received_message, byte *result){
    byte buf[1];
    int currentPos = BCC1_POS + 1, size = 0;
    
    byte currentXOR = received_message[BCC1_POS], previousByte = 0x00;

    if(DEBUG_MODE) printf("Starting to receive data\n");

    ReadOneByte(fd, buf);
    while(buf[0] != FLAG){
        
        //Byte destuffing
        if(buf[0] == ESC){

            ReadOneByte(fd,buf);
            if(buf[0] == FLAG_ESC){
                received_message[currentPos++] = FLAG;
                result[size++] = FLAG;

            }else if(buf[0] == ESC_ESC){
                received_message[currentPos++] = ESC;
                result[size++] = ESC;

            }

        }else{
            received_message[currentPos++] = buf[0];
            result[size++] = buf[0];
        }        
        
        if(previousByte != 0x00)
            currentXOR = currentXOR ^ previousByte;
        
        previousByte = received_message[currentPos - 1];
        ReadOneByte(fd,buf);
    }
    //buf[0] stores the FLAG
    received_message[currentPos] = buf[0];
    result[size++] = buf[0];


    //After the cycle ends, the BCC2 is stored in previousByte
    if(currentXOR != previousByte){
        printf("BCC2 doesn't match\n");
        return -1;
    }
    if(DEBUG_MODE) printf("BCC2 received: %x\n", previousByte);
    if(DEBUG_MODE) printf("Flag read: %x\n", buf[0]);

    return size;
}

int send_set_message(int fd){

    byte message[5];
    message[0] = FLAG;
    message[1] = A;
    message[2] = C_SET;
    message[3] = BCC1(C_SET);
    message[4] = FLAG;

    int res = write(fd, message, 5);
    if(res <= 0){
        printf("Error in writing to serialPort\n");
        return -1;
    }
    if(DEBUG_MODE) print_message("SET: ", message, 5);
    return 0;
}

int send_i_message(int fd, byte *msg, int n){    
    int res;
    if(n > MAX_DATA_D){
        printf("Error: Data size to big to send\n");
        return -1;
    }
    
    byte command[FRAME_SIZE];
    
    command[0] = FLAG;
    command[1] = A;
    command[2] = C_I(currentNs);
    command[3] = BCC1(command[2]);
    
    byte currentXOR = command[3];

    int i, j;
 
    for(i = 4, j = 0; j < n; i++, j++){      //i iterates over command array. j iterates over msg array
        
        //Byte Stuffing
        if(msg[j] == FLAG){                     
            
            command[i++] = ESC;
            command[i] = FLAG_ESC;
        }else if(msg[j] == ESC){
            
            command[i++] = ESC;
            command[i] = ESC_ESC;
        }else {

            command[i] = msg[j];            
        }
        currentXOR = currentXOR ^ msg[j];

    }

    //Byte Stuffing     BCC2
    if(currentXOR == FLAG){                     
        
        command[i++] = ESC;
        command[i++] = FLAG_ESC;
    }else if(currentXOR == ESC){
        
        command[i++] = ESC;
        command[i++] = ESC_ESC;
    }else {

        command[i++] = currentXOR;            
    }                    
    command[i++] = FLAG;

    res = write(fd, command, i);    
    if(DEBUG_MODE) print_message("I command: ", command, i);

    return res;
}

int send_disc_message(int fd){
    byte disc_reply[5];

    disc_reply[0] = FLAG;
    disc_reply[1] = A;
    disc_reply[2] = C_DISC;
    disc_reply[3] = BCC1(C_DISC);
    disc_reply[4] = FLAG;

    int res = write(fd, disc_reply, 5);
    if(res <= 0){
        printf("Error in writing to serialPort\n");
        return -1;
    }
    if(DEBUG_MODE) print_message("DISC: ", disc_reply, 5);
    return 0;
}

int  send_ua_message(int fd){
    byte ua_reply[5];

    ua_reply[0] = FLAG;
    ua_reply[1] = A;
    ua_reply[2] = C_UA;
    ua_reply[3] = BCC1(C_UA);
    ua_reply[4] = FLAG;
    
    int res = write(fd, ua_reply, 5);
    if(res <= 0){
        printf("Error in writing to serialPort\n");
        return -1;
    }
    if(DEBUG_MODE) print_message("UA: ", ua_reply, 5);
    return 0;
}

int send_rr_message(int fd){
    byte rr_reply[5];

    rr_reply[0] = FLAG;
    rr_reply[1] = A;
    rr_reply[2] = C_RR(currentNr);
    rr_reply[3] = BCC1(rr_reply[2]);
    rr_reply[4] = FLAG;

    int res = write(fd, rr_reply, 5);
    if(res <= 0){
        printf("Error in writing to serialPort\n");
        return -1;
    }
    if(DEBUG_MODE) print_message("RR: ", rr_reply, 5);
    return 0;
}

int send_rej_message(int fd){
    byte rej_reply[5];

    rej_reply[0] = FLAG;
    rej_reply[1] = A;
    rej_reply[2] = C_REJ(currentNr);
    rej_reply[3] = BCC1(rej_reply[2]);
    rej_reply[4] = FLAG;

    int res = write(fd, rej_reply, 5);
    if(res <= 0){
        printf("Error in writing to serialPort\n");
        return -1;
    }
    if(DEBUG_MODE) print_message("REJ: ", rej_reply, 5);
    return 0;
}

void print_message(char *before, byte *message, int size){

    if(before != NULL)
        printf("%s", before);

    for (int i = 0; i < size; i++){
        printf("%x ",message[i]);
    }
    printf("\n");
}

void updateCurrentNs(){
   
    if(currentNs == 0)
        currentNs = 1;
    else
        currentNs = 0;
}

void updateCurrentNr(){
    if(currentNr == 0)
        currentNr = 1;
    else
        currentNr = 0;
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
    //\if(DEBUG_MODE) printf("Serial Port opened\n");

    if (set_costume_conf(fd) == -1)
        return -1;

    int res;

    if(role == TRANSMITTER)
        res = open_emitter(fd);

    else if(role == RECEIVER){
        res = open_receiver(fd);

    }else{
        printf("Unkwoned role.\n");
        res = -1;
    }
    ROLE = role;

    if(res == -1 )
        return -1;
    
    return fd;
}

int open_serialPort(char *port){
    int fd;

    fd = open(port, O_RDWR | O_NOCTTY );
    if (fd <0 ) {perror("port"); return -1; }

    return fd;
}

int set_costume_conf(int fd){

    struct termios newtio;
     if (tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
        perror("tcgetattr");
        return -1;
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
        return -1;
    }
    return 0;
}

int reset_serialPort_conf(int fd, struct termios oldtio){
    if (tcsetattr(fd,TCSANOW,&oldtio) == -1) {
        perror("tcsetattr");
        return -1;
    } 
    return 0;
}

int open_emitter(int fd){
    byte received_message[5];

    signal(SIGALRM, count);
    siginterrupt(SIGALRM, 1);
    
     
    while(n_alarm < MAX_TIMEOUT){

        send_set_message(fd);   
        alarm(TIMEOUT_SECS);
        
        enum MessageType response = ReceiveMessage(fd, received_message);
        
        if(response == TIME_OUT){
            printf("Time_out occcured. Resending message.\n");

        }else if(response != UA){
            printf("Received message from wrong type.\n");
            continue;

        }else {
            break;
        }
    }
    alarm(0);

    if(n_alarm == MAX_TIMEOUT){
        return -1;
    }
    return 0;
}

int open_receiver(int fd){
    byte received_message[5];
    
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

int llclose(int fd){
    int res;

    if(ROLE == TRANSMITTER)
        res = close_emitter(fd);
    else if(ROLE == RECEIVER){
        res = close_receiver(fd);
    }else{
        printf("Unkwoned role.\n");
        res = -1;
    }
    if(res == -1)
        return res;

    sleep(1);

    if(reset_serialPort_conf(fd, oldtio) == -1)
        return -1;

    
    close(fd);
    return 0;
    
}

int close_emitter(int fd){
    byte received_message[10];

    while(n_alarm < MAX_TIMEOUT){

        send_disc_message(fd);
        alarm(TIMEOUT_SECS);
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
    alarm(0);

    if(n_alarm == MAX_TIMEOUT){
        printf("Couldn't close connection.\n");
        return -1;
    }
    return 0;
}

int close_receiver(int fd){
    byte received_message[10];
    
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

int llwrite(int fd, byte* buffer, int length){
    int numWrittenCharacters;
    
    byte received_message[10];
    enum MessageType ret;

    while(n_alarm < MAX_TIMEOUT){
        //send frame
        numWrittenCharacters = send_i_message(fd, buffer, length);
        if(numWrittenCharacters < 0) {
            printf("Error in sendinf I message\n");
            return -1;
        }
        alarm(TIMEOUT_SECS);

        usleep(DELAY * 1000);  // 1ms
        ret = ReceiveMessage(fd,received_message);
        if(ret == REJ){
            alarm(0);
            if(DEBUG_MODE) printf("Received REJ message.\n");
            n_alarm = 0;

        }else if(ret == TIME_OUT){
            printf("Time_out occurred.\n");

        }else if(ret == RR){ 
            
            alarm(0);
            if(DEBUG_MODE) printf("Received RR message. \n");
            break;
        
        }else if(ret == RR_REPEATED){
            
            alarm(0);
            if(DEBUG_MODE) printf("Received RR_REPEATED message. \n");
            break;
        }
    }
   
    if(n_alarm == MAX_TIMEOUT){
        printf("Couldn't establish connection.\n");
        return -1;
    }
    n_alarm = 0;
    
    updateCurrentNs();
    updateEmitterNr();
    return numWrittenCharacters;  //returns the number of written characters
}

int llread(int fd, byte* buffer){

    usleep(DELAY * 1000);  // 1ms
    int ret = ReceiveI(fd, buffer);
    
    while(1){
        
        if(ret == -1){
            printf("Error in receiving message\n");
            send_rej_message(fd); 
            usleep(DELAY * 1000);  // 1ms  
            ret = ReceiveI(fd, buffer);
          
        }else if(ret == -2){
            send_rr_message(fd);
            usleep(DELAY * 1000);  // 1ms
            ret = ReceiveI(fd, buffer);
            
        }else{
            if(DEBUG_MODE) printf("Received Message!\n");
            send_rr_message(fd);
   

            updateCurrentNr();
            updateReceiverNs();
            return ret;
        }
        
    }
   
}


