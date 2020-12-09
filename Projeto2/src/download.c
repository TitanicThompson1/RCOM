
#include <stdio.h>
#include "parseURL.h"

int main(int argc, char** argv){

    // Wrong input from user
    if (argc != 2){
        printf("Usage: download ftp://[<user>:<password>@]<host>/<url-path>\n");
        return -1;
    }

    ftp_args arguments;
    
    parse_url(argv[1], &arguments);
    
    print_args(arguments);

    return 0;

}