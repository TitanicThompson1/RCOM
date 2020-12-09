#ifndef FTP_H
#define FTP_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "ftp_data.h"


#define FTP_PORT 21
#define SERVER_RES_LEN 255

int ftp_transfer_file(ftp_args info);

int establish_TCP(char* ip);

int authenticate_user(int socketfd ,char* user, char* pass);

int send_command(int sockfd, char* command);

int receive_response(int sockfd, char* response);

int host_to_ip(char* host, char* ip);


#endif  // FTP_H