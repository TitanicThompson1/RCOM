#ifndef FTP_DATA_H
#define FTP_DATA_H


#define USER_LEN 63
#define PASS_LEN 63
#define HOST_LEN 255
#define PATH_LEN 255

#define DESCR_LEN 127

#define FILENAME_LEN 63

#define SERVER_RES_LEN 255
#define ARGS_LEN 255

/**
 * @brief Struct with the arguments necessary to FTP protocol (from the URL)
 * 
 */
typedef struct ftp_args{
    char user[USER_LEN];
    char pass[PASS_LEN];
    char host[HOST_LEN];
    char path[PATH_LEN];

} ftp_args;

/**
 * @brief Struct to the server response
 * 
 */
typedef struct ftp_server_res{
    int code;
    char description[DESCR_LEN];

} ftp_server_res;

/**
 * @brief A struct that contains the parts of the IP
 * 
 */
typedef struct IP{
    int part1;
    int part2;
    int part3;
    int part4;
} IP;



#endif  // FTP_DATA_H