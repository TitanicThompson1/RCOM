#ifndef FTP_DATA_H
#define FTP_DATA_H

/**
 * @brief struct that holds the data needed to do ftp
 * 
 */
#define USER_MAX_LENGTH 63
#define PASS_MAX_LENGTH 63
#define HOST_MAX_LENGTH 255
#define PATH_MAX_LENGTH 255


typedef struct ftp_args{
    char user[USER_MAX_LENGTH];
    char pass[PASS_MAX_LENGTH];
    char host[HOST_MAX_LENGTH];
    char path[PATH_MAX_LENGTH];

} ftp_args;


#endif  // FTP_DATA_H