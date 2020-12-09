#ifndef PARSER_H
#define PARSER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ftp_data.h"

/**
 * @brief Parses the url passed by user and puts it in a struct ftp_args
 * 
 * @param url the url to parse
 * @param parsed_args the resulting struct
 * @return int negative if an error occured. 0 otherwise.
 */
int parse_url(char* url, ftp_args* parsed_args);

/**
 * @brief Parses the user path and host part of the url and puts it in the struct
 * 
 * @param user_pass_host the user, path and host
 * @param parsed_args the struct with the above parts 
 * @return int negative if an error occured. 0 otherwise.
 */
int parse_uph(char* user_pass_host, ftp_args* parsed_args);

/**
 * @brief Prints the content of arg in a nice format
 * 
 * @param arg the argumets of ftp
 */
void print_args(ftp_args arg);

/**
 * @brief Parses the response from the server into the struct
 * 
 * @param response response from the server to be parsed
 * @param formatted_response the response in the struct
 * @return int negative if an error occured. 0 otherwise.
 */
int parse_server_response(char* response, ftp_server_res* formatted_response);

#endif  // PARSER_H