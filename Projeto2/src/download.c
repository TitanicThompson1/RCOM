
#include <stdio.h>

#include "parseURL.h"
#include "ftp.h"

int main(int argc, char** argv){

    // Wrong input from user
    if (argc != 2 && strcmp(argv[0], "download") != 0){
        printf("Usage: download ftp://[<user>:<password>@]<host>/<url-path>\n");
        return -1;
    }

    ftp_args arguments;
    
    if(parse_url(argv[1], &arguments) != 0){
        printf("Usage: download ftp://[<user>:<password>@]<host>/<url-path>\n");
        return -1;
    }

    print_args(arguments);

    if(ftp_transfer_file(arguments) != 0){
        printf("Error in transfering file\n");
        return -1;
    }

    printf("File transfered!\n");

    return 0;

}