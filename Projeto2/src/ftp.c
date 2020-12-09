#include "ftp.h"



int ftp_transfer_file(ftp_args info){

    char ip[63];
    host_to_ip(info.host, ip);
    printf("%s\n", ip);
    int socketfd;
    if((socketfd = establish_TCP(ip)) < 0) return -1;

    char response[SERVER_RES_LEN];

    // Welcome response
    if(receive_response(socketfd, response)) return -1;
    printf("Response: %s", response);

    // Authentication
    if(authenticate_user(socketfd, info.user, info.pass)) return -1;

    // Enter passive mode

    

    return 0;

}


int establish_TCP(char* ip){

	int	sockfd;
	struct	sockaddr_in server_addr;

	//server address handling
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);	//32 bit Internet address network byte ordered
	server_addr.sin_port = htons(FTP_PORT);		    //server TCP port must be network byte ordered 
    
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
    
    return sockfd;
}

int authenticate_user(int socketfd ,char* user, char* pass){
    char user_command[USER_LEN + 10]; char pass_command[PASS_LEN + 10];

    // Formating the user and password commands
    sprintf(user_command,"user %s\n", user);
    sprintf(pass_command,"pass %s\n", pass);

    char user_response[SERVER_RES_LEN]; char pass_response[SERVER_RES_LEN];
    ftp_server_res user_resp; ftp_server_res pass_resp;

    send_command(socketfd, user_command);

    receive_response(socketfd, user_response);

    if(parse_server_response(user_response, &user_resp)) return -1;
    
    int n_tries = 0;
    if(strcmp(user, "anonymous") == 0){
        
        while(user_resp.code != 230 && n_tries < MAX_TRIES){            // Tries to send the command more times (MAX_TRIES)
            n_tries++;

            send_command(socketfd, user_command);

            receive_response(socketfd, user_response);
            if(parse_server_response(user_response, &user_resp)) return -1;
        }

        if(n_tries == MAX_TRIES){                                       // Exceed maximum number of tries
            printf("Maximum tries exceded. Exiting\n");
            return -1;
        }

        // Anonymous doesn't need the password
        return 0;                                                       
    }else {

        while(user_resp.code != 331 && n_tries < MAX_TRIES){
            n_tries++;

            send_command(socketfd, user_command);

            receive_response(socketfd, user_response);
            if(parse_server_response(user_response, &user_resp)) return -1;
        }

        if(n_tries == MAX_TRIES){
            printf("Maximum tries exceded. Exiting\n");
            return -1;
        }
    }


    send_command(socketfd, pass_command);

    receive_response(socketfd, pass_response);
    printf("Response Pass: %s", pass_response);

    if(parse_server_response(pass_response, &pass_resp)) return -1;

    n_tries = 0;
    while(pass_resp.code != 230 && n_tries < MAX_TRIES){
        n_tries++;

        send_command(socketfd, pass_command);

        receive_response(socketfd, pass_response);

        if(parse_server_response(pass_response, &pass_resp)) return -1;

    }

    if(n_tries == MAX_TRIES){
        printf("Maximum tries exceded. Exiting\n");
        return -1;
    }

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


int host_to_ip(char* host, char* ip){
    struct hostent *h;

    if ((h=gethostbyname(host)) == NULL) {  
        herror("gethostbyname");
        return -1;
    }
    strcpy(ip, inet_ntoa(*((struct in_addr *)h->h_addr)));
    return 0;

}

