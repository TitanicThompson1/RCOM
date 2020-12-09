#include "parseURL.h"


void parse_url(char* url,  ftp_args* parsed_args){
    
    // getting the first part of url to discard it
    strtok(url, "//");

    // getting the first part of url
    char* user_pass_host = strtok(NULL, "/");
    
    // getting the second part of url (path)
    char* path = strtok(NULL, " ");

    // parsing the user, password and host
    parse_uph(user_pass_host, parsed_args);

    //the path is only the remaing
    strcpy(parsed_args->path, path);
} 


void parse_uph(char* user_pass_host, ftp_args* parsed_args){
    
    // storing the user_pass_host because strtok is going to modify it and it's needed to verify if there is an user or not
    char all_phrase[255*3];
    strcpy(all_phrase, user_pass_host);

    // getting user. If user is not specified, its going to return all 
    char *user = strtok(user_pass_host, ":");
    

    if(strcmp(user, all_phrase) == 0){          // no user (the ':' doesn't exists in the initial phrase)
        strcpy(parsed_args->user, "anonymus");
        strcpy(parsed_args->pass, "");
        strcpy(parsed_args->host, user);
        

    } else {                                   // user is specified
        strcpy(parsed_args->user, user);
        char *pass = strtok(NULL, "@");
        strcpy(parsed_args->pass, pass);
        char *host = strtok(NULL, "");
        strcpy(parsed_args->host, host);
    }    
}



void print_args(ftp_args arg){
    printf("User: %s \t Pass: %s \t Host: %s \t Path: %s \n", arg.user, arg.pass, arg.host, arg.path);
}


