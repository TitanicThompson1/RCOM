#include "ftp.h"



int ftp_transfer_file(ftp_args info){

    char ip[30];
    host_to_ip(info.host, ip);

    int socketfd;
    if((socketfd = establish_TCP(ip, FTP_PORT)) < 0){
        printf("Error in establishing TCP connection\n");
        return -1;
    }
    printf("TCP connection established!\n");

    ftp_server_res welcome_res;
    
    // Welcome response
    if(receive_response_struct(socketfd, &welcome_res) != 0 || welcome_res.code != 220) {
        close(socketfd);
        return -1;
    }
    

    // Authentication
    if(authenticate_user(socketfd, info.user, info.pass)) {
        printf("Error in authentication\n");
        close(socketfd);
        return -1;
    }
    printf("Authentication succeded!\n");

    // Enter passive mode
    int file_socket = enter_pasv_mode(socketfd);
    if(file_socket < 0){
        printf("Error entering passive mode\n");
        close(socketfd);
        return -1;
    }
    printf("Now in passive mode!\n");

    // Downloading file
    if(retrieve_file(socketfd, file_socket, info.path)){
        printf("Error in retrieving file\n");
        close(socketfd);
        close(file_socket);
        return -1;    
    }
    printf("File retrieved!\n");

    if(closes_TCP(socketfd)){
        printf("Error in closing conection\n");
        close(file_socket);
        return -1;
    }
    printf("Connection closed!\n");

    close(file_socket);

    return 0;

}


int establish_TCP(char* ip, int port){

	int	sockfd;
	struct sockaddr_in server_addr;

	//server address handling
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);	//32 bit Internet address network byte ordered
	server_addr.sin_port = htons(port);		    //server TCP port must be network byte ordered 
    
	//open an TCP socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    //connect to the server
    if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("connect()");
        return -1;
    }

    printf("OI\n");
    return sockfd;
}


int authenticate_user(int socketfd ,char* user, char* pass){
    char user_command[USER_LEN + 10]; char pass_command[PASS_LEN + 10];

    // Formating the user and password commands
    sprintf(user_command,"user %s\n", user);
    sprintf(pass_command,"pass %s\n", pass);

    ftp_server_res user_resp; ftp_server_res pass_resp;

    if(send_command(socketfd, user_command)) return -1;

    if(receive_response_struct(socketfd, &user_resp)) return -1;
    
    int n_tries = 0;
    if(strcmp(user, "anonymous") == 0){
        
        while(user_resp.code != 230 && n_tries < MAX_TRIES){            // Tries to send the command more times (MAX_TRIES)
            printf("Unable to contact server. Trying again in 1 sec\n"); n_tries++;
            sleep(1);

            if(send_command(socketfd, user_command)) return -1;

            if(receive_response_struct(socketfd, &user_resp)) return -1;
        }

        if(n_tries == MAX_TRIES){                                       // Exceed maximum number of tries
            printf("Maximum tries exceded. Exiting\n");
            return -1;
        }

        // Anonymous doesn't need the password
        return 0;                                                       
    }else {

        while(user_resp.code != 331 && n_tries < MAX_TRIES){
            printf("Unable to contact server. Trying again in 1 sec\n"); n_tries++;
            sleep(1);

            if(send_command(socketfd, user_command)) return -1;

            if(receive_response_struct(socketfd, &user_resp)) return -1;
        }

        if(n_tries == MAX_TRIES){                                       // Exceed maximum number of tries
            printf("Maximum tries exceded. Exiting\n");
            return -1;
        }
    }


    if(send_command(socketfd, pass_command)) return -1;

    if(receive_response_struct(socketfd, &pass_resp)) return -1;

    n_tries = 0;
    while(pass_resp.code != 230 && n_tries < MAX_TRIES){
        printf("Unable to contact server. Trying again in 1 sec\n"); n_tries++;
        sleep(1);

        if(send_command(socketfd, pass_command)) return -1;

        if(receive_response_struct(socketfd, &pass_resp)) return -1;

    }

    if(n_tries == MAX_TRIES){                                           // Exceed maximum number of tries
        printf("Maximum tries exceded. Exiting\n");
        return -1;
    }

    return 0;
}


int enter_pasv_mode(int socketfd){

    // Sending command to enter passive mode
    if(send_command(socketfd, "pasv \n")) return -1;


    ftp_server_res pasv_resp;
    if(receive_response_struct(socketfd, &pasv_resp)) return -1;

    
    int n_tries = 0;
    while(pasv_resp.code != 227 && n_tries < MAX_TRIES){
        printf("Unable to contact server. Trying again in 1 sec\n"); n_tries++;
        sleep(1);

        if(send_command(socketfd, "pasv \n")) return -1;

        if(receive_response_struct(socketfd, &pasv_resp)) return -1;
    }

    if(n_tries == MAX_TRIES){
        printf("Maximum tries exceded. Exiting\n");
        return -1;
    }

    // Getting rid of the garbage
    strtok(pasv_resp.description, "(");

    // This gets only the part needed of the description
    char *pre_formatted_ip = strtok(NULL, ")");

    IP download_ip; int pre_port1, pre_port2;


    // Parsing the string into the IP and ports
    int ret = sscanf(pre_formatted_ip, "%d,%d,%d,%d,%d,%d", &download_ip.part1, &download_ip.part2, &download_ip.part3, &download_ip.part4, &pre_port1, &pre_port2);
    if(ret == EOF){                             // Error control
        printf("Error in sscanf\n");
        return -1;
    }

    int port = pre_port1 * 256 + pre_port2;     // Needed to get the actual port

    char ip[30];
    ret = sprintf(ip, "%d.%d.%d.%d", download_ip.part1, download_ip.part2, download_ip.part3, download_ip.part4);
    if(ret < 0){                                // Error control
        printf("Error in sprintf\n");
        return -1;
    }

    return establish_TCP(ip, port);
}


int retrieve_file(int socketfd, int file_socket, char* path){

    char retr_cmmd[PATH_LEN + 10]; ftp_server_res retr_resp;
    
    
    sprintf(retr_cmmd, "retr %s\n", path);

    // Sending retrieve command
    if(send_command(socketfd, retr_cmmd)) return -1;

    if(receive_response_struct(socketfd, &retr_resp)) return -1;


    int n_tries = 0;
    while(retr_resp.code != 150 && n_tries < MAX_TRIES){
        printf("Unable to contact server. Trying again in 1 sec\n"); n_tries++;
        sleep(1);

        if(send_command(socketfd, retr_cmmd)) return -1;

        if(receive_response_struct(socketfd, &retr_resp)) return -1;
    }

    if(n_tries == MAX_TRIES){
        printf("Maximum tries exceded. Exiting\n");
        return -1;
    }

    char filename[FILENAME_LEN];
    if(get_filename(path, filename)) return -1;

    int filefd = open(filename, O_WRONLY | O_CREAT, 0644);
    if(filefd == -1){
        perror("open");
        return -1;
    }
    
    char buf[1]; int nread, nwrite;

    do{
        nread = read(file_socket, buf, 1);
        if(nread == -1){
            perror("read");
            return -1;
        }

        nwrite = write(filefd, buf, 1);
        if(nwrite == -1){
            perror("write");
            return -1;
        }
    }while(nread != 0);

    close(filefd);

    ftp_server_res completed_res;

    if(receive_response_struct(socketfd, &completed_res) != 0 || completed_res.code != 226) return -1;
    
    return 0;
}


int closes_TCP(int socketfd){
    send_command(socketfd, "quit \n");

    ftp_server_res quit_resp;
    receive_response_struct(socketfd, &quit_resp);

    int n_tries = 0;
    while(quit_resp.code != 221 && n_tries < MAX_TRIES){            // Tries to send the command more times (MAX_TRIES)
        printf("Unable to contact server. Trying again in 1 sec\n"); n_tries++;
        sleep(1);

        if(send_command(socketfd, "quit \n")) return -1;

        if(receive_response_struct(socketfd, &quit_resp)) return -1;
    }

    if(n_tries == MAX_TRIES){                                       // Exceed maximum number of tries
        printf("Maximum tries exceded. Exiting\n");
        return -1;
    }

    close(socketfd);
    return 0;
}


int send_command(int sockfd, char* command){
    
    //send a string to the server
    int ret = write(sockfd, command, strlen(command));
    if(ret < 0){
        perror("write");
        return -1;
    }
    return 0;
}


int receive_response(int sockfd, char* response){
    char buf[1]; int i=0;
    
    int ret = read(sockfd, buf, 1);
    if(ret < 0){                        // Error control
        perror("read");
        return -1;
    }
    response[i++] = buf[0];

    while(buf[0] != '\n'){              // it finishes when it reads \n
        ret = read(sockfd, buf, 1);
        if(ret < 0){
            perror("read");
            return -1;
        }else if(ret == 0) break;

        response[i++] = buf[0];
    }
    response[i] = '\0';

    return 0;
}


int receive_response_struct(int sockfd, ftp_server_res* response){
    char resp[SERVER_RES_LEN];
    if(receive_response(sockfd, resp)) return -1;
    
    printf("%s\n", resp);
    
    if(parse_server_response(resp, response)) return -1;

    return 0;
}


int host_to_ip(char* host, char* ip){
    struct hostent *h;

    if ((h=gethostbyname(host)) == NULL) {  
        herror("gethostbyname");
        return -1;
    }
    strcpy(ip, inet_ntoa(*((struct in_addr *)h->h_addr)));
    return 0;

}

