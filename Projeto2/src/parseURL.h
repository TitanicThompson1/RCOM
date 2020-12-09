#ifndef PARSEURL_H
#define PARSEURL_H

#include <string.h>
#include <stdio.h>

typedef struct ftp_args{
    char user[255];
    char pass[255];
    char host[255];
    char path[255];

} ftp_args;

/**
 * @brief Parses the url passed by user and puts it in a struct ftp_args
 * 
 * @param url the url to parse
 * @param parsed_args the resulting struct
 */
void parse_url(char* url, ftp_args* parsed_args);

/**
 * @brief Parses the user path and host part of the url and puts it in the struct
 * 
 * @param user_pass_host the user, path and host
 * @param parsed_args the struct with the above parts 
 */
void parse_uph(char* user_pass_host, ftp_args* parsed_args);


void print_args(ftp_args arg);

#endif  // PARSEURL_H