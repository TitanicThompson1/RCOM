#ifndef FTP_H
#define FTP_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "ftp_data.h"
#include "parser.h"


#define FTP_PORT 21
#define MAX_TRIES 3

int ftp_transfer_file(ftp_args info);

/**
 * @brief Establishes the TCP conection with the server.
 * 
 * @param ip ip of the server to connect.
 * @param port the port of the connection.
 * @return int negative if an error occured. The file descriptor of the socket otherwise.
 */
int establish_TCP(char* ip, int port);

/**
 * @brief Authenticates the user into the server.
 * 
 * @param socketfd the socket file descriptor.
 * @param user name of the user.
 * @param pass password of the user.
 * @return int negative if an error occured. 0 otherwise.
 */
int authenticate_user(int socketfd ,char* user, char* pass);

/**
 * @brief enters to passive mode and returns the new socket file descriptor.
 * 
 * @param socketfd the socket file descriptor.
 * @return int the new socket file descriptor.
 */
int enter_pasv_mode(int socketfd);

/**
 * @brief Retrieves file from server.
 * 
 * @param socketfd the connection socket file descriptor.
 * @param file_socket the socket file descriptor to retrieve the file .
 * @param path path of file within the server.
 * @return int negative if an error occured. 0 otherwise.
 */
int retrieve_file(int socketfd, int file_socket, char* path);

/**
 * @brief Closes the TCP connection with quit command
 * 
 * @param socketfd the connection socket file descriptor.
 * @return int negative if an error occured. 0 otherwise.
 */
int closes_TCP(int socketfd);

/**
 * @brief Sends a command to the server.
 * 
 * @param sockfd the socket file descriptor.
 * @param command the command to be sent.
 * @return int negative if an error occured. 0 otherwise.
 */
int send_command(int sockfd, char* command);

/**
 * @brief Receives a response from the server.
 * 
 * @param sockfd the socket file descriptor.
 * @param response the command to be sent.
 * @return int negative if an error occured. 0 otherwise.
 */
int receive_response(int sockfd, char* response);


/**
 * @brief Receives the server response and puts it into the struct ftp_server_response.
 * 
 * @param sockfd the socket file descriptor.
 * @param response the response of the server
 * @return int 
 */
int receive_response_struct(int sockfd, ftp_server_res* response);

/**
 * @brief Translates the host name into the IP.
 * 
 * @param host hostname.
 * @param ip the ip associated with the hostname.
 * @return int negative if an error occured. 0 otherwise.
 */
int host_to_ip(char* host, char* ip);


#endif  // FTP_H