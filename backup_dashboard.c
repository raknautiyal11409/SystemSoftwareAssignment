#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>

void backup_dashboard(time_t lastMove_time_t) {
// void main(){
    DIR* dir;
    struct dirent* ent;
    int count = 0;
    char* dash_dir = "/Dashboard_Directory/";
    char* backUp_dir = "/BackUp_Directory/";

    // struct tm lastMove_time = {0}; // initialize a tm struct to all zeroes
    // time_t lastMove_time_t; // declare a time_t variable

    // // set the fields of the tm struct to the desired values
    // lastMove_time.tm_year = 2023 - 1900; // year - 1900
    // lastMove_time.tm_mon = 2; // month (0-11, so March is 2)
    // lastMove_time.tm_mday = 18; // day of the month
    // lastMove_time.tm_hour = 12; // hour (24-hour clock)
    // lastMove_time.tm_min = 30; // minute
    // lastMove_time.tm_sec = 0; // second

    // // convert the tm struct to a time_t value
    // lastMove_time_t = mktime(&lastMove_time);

    // Open the directory
    dir = opendir(dash_dir);
    if (dir == NULL) {
        printf("Error opening directory: Shared Directory\n");
        return -1;
    }
    
    int status;

    // Fork child processes to copy each file to the dashboard folder
    while ((ent = readdir(dir)) != NULL)  {

        struct stat filestat;
        time_t* mtime;
        time_t* ctime;
        char* path = (char*)malloc((strlen(dash_dir) + strlen(ent->d_name) + 1) * sizeof(char));
        strcpy(path, dash_dir);
        strcat(path, ent->d_name);

        if (stat(path, &filestat) == -1) {
            // Error: could not stat file
            perror("Error");
            exit(EXIT_FAILURE);
        }

        
        // compare if the files has been modified since the last move
        if(difftime(filestat.st_mtime, lastMove_time_t ) < 0) {
            pid_t pid = fork();

            if (pid == -1) {
                printf("Error forking child process: %s\n", strerror(errno));
                syslog(LOG_ERR, "BackUP Dash: %s ",EXIT_FAILURE);
                exit(EXIT_FAILURE);

            } else if (pid == 0) {

                // Child process copies the file
                char dest[50];
                strcpy(dest, backUp_dir);
                strcat(dest, ent->d_name);
                
                if (execlp("cp", "cp", path, dest, NULL) == -1) {
                    printf("Error copying file: %s\n", strerror(errno));
                    syslog(LOG_ERR, "Error copying file: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }

                // record successful file transfer
                syslog(LOG_INFO, "Backup DIR: Successfully moved %s to Dashboard", ent->d_name);
            } 
        }

    }

    // Wait for all child processes to complete
    for (int i = 0; i < count; i++) {
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            printf("File copied successfully.\n");
        } else {
            printf("Error copying file\n");
        }
    }

    // Close the directory
    closedir(dir);
}
