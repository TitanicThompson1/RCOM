#include "parser.h"


int parse_url(char* url,  ftp_args* parsed_args){
    
    // getting the first part of url to discard it
    char* protocol = strtok(url, "//");
    if(protocol == NULL || strcmp(protocol, "ftp:")){           
        printf("Wrong format of URL\n");
        return -1;
    }

    // getting the first part of url
    char* user_pass_host = strtok(NULL, "/");
    if(user_pass_host == NULL){            //Error control
        printf("Wrong format of URL\n");
        return -1;
    }

    
    // getting the second part of url (path)
    char* path = strtok(NULL, " ");
    if(path == NULL){            // Error control
        printf("Wrong format of URL\n");
        return -1;
    }

    // parsing the user, password and host
    if(parse_uph(user_pass_host, parsed_args) != 0) return -1;

    // storing the path
    strcpy(parsed_args->path, path);

    return 0;

} 


int parse_uph(char* user_pass_host, ftp_args* parsed_args){
    
    // storing the user_pass_host because strtok is going to modify it and it's needed to verify if there is an user or not
    char all_phrase[255*3];
    strcpy(all_phrase, user_pass_host);

    // getting user. If user is not specified, its going to return all 
    char *user = strtok(user_pass_host, ":");
    if(user == NULL){            // Error control
        printf("Wrong format of URL\n");
        return -1;
    }

    if(strcmp(user, all_phrase) == 0){          // no user (the ':' doesn't exists in the initial phrase)
        strcpy(parsed_args->user, "anonymous");
        strcpy(parsed_args->pass, "");
        strcpy(parsed_args->host, user);
        

    } else {                                   // user is specified
        strcpy(parsed_args->user, user);

        char *pass = strtok(NULL, "@");
        if(pass == NULL){            // Error control
            printf("Wrong format of URL\n");
            return -1;
        }   

        strcpy(parsed_args->pass, pass);
        
        char *host = strtok(NULL, "");
        if(user == NULL){            // Error control
            printf("Wrong format of URL\n");
            return -1;
        }
        
        strcpy(parsed_args->host, host);
    }
    return 0;
}



void print_args(ftp_args arg){
    printf("User: %s \t Pass: %s \t Host: %s \t Path: %s \n", arg.user, arg.pass, arg.host, arg.path);
}

int get_filename(char* path, char* filename){
    
    char *part_of_path = malloc(255);
    part_of_path = strtok(path, "/");
    if(part_of_path == NULL){
        printf("Wrong format of URL\n");
        free(part_of_path);
        return -1;
    }

    do{
        //copies the previous parte of path to filename
        strcpy(filename, part_of_path);

        //gets the next path
        part_of_path = strtok(NULL, "/");
    }while(part_of_path != NULL);

    free(part_of_path);

    return 0;
}


int parse_server_response(char* response, ftp_server_res* formatted_response){
    
    // Getting code
    char* code = strtok(response, " ");
    if(code == NULL){           
        printf("Wrong format of server response\n");
        return -1;
    }

    // Getting description
    char* description = strtok(NULL, "");
    if(description == NULL){           
        printf("Wrong format of server response\n");
        return -1;
    }

    // Puting the code and description in struct
    formatted_response->code = atoi(code);
    strcpy(formatted_response->description, description);

    return 0;
}

