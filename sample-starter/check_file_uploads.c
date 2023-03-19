#include <unistd.h>
#include <syslog.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>

void check_file_uploads(void) {
// void main(void) {

    // Fork a child process
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);

    } else if (pid == 0) { // Child process

        struct tm lastCheck = {0}; // initialize a tm struct to all zeroes
        time_t lastMove_time_t; // declare a time_t variable

        // set the fields of the tm struct to the desired values
        lastCheck.tm_year = 2023 - 1900; // year - 1900
        lastCheck.tm_mon = 2; // month (0-11, so March is 2)
        lastCheck.tm_mday = 18; // day of the month
        lastCheck.tm_hour = 21; // hour (24-hour clock)
        lastCheck.tm_min = 8; // minute
        lastCheck.tm_sec = 0; // second


        char* shared_dir = "../SharedDirectory/";
        DIR* directory;
        struct dirent* entry;
        int files_found = 0;
        char* file_extention;
        char file_names[4][20] = {"warehouse", "manufacturing", "distribution", "sales"};
        int fileCheck[4] = {0,0,0,0};
        int modified_files = 0;
        int rows = sizeof(file_names) / sizeof(file_names[0]);
    
        // Open directory
        directory = opendir(shared_dir);

        if (directory == NULL) {
            perror("Error opening directory");
            syslog(LOG_ERR, EXIT_FAILURE);
            exit(EXIT_FAILURE);
        }

        // Search for XML files with matching name
        while ((entry = readdir(directory)) != NULL) {

            for(int i=0; i<rows; i++){
                // Check if entry is a regular file
                if (entry->d_type == DT_REG) { 
                    
                    // Get file extension
                    file_extention = strrchr(entry->d_name, '.'); 
                    
                    
                    // Check if file is XML
                    if (file_extention != NULL && strcmp(file_extention, ".xml") == 0) { 

                        // Check if file name matches
                        if (strstr(entry->d_name, file_names[i]) != NULL) { 

                            //check file modified or creation time
                            struct stat filestat;
                            char* path = (char*)malloc((strlen(shared_dir) + strlen(entry->d_name) + 1) * sizeof(char));
                            strcpy(path, shared_dir);
                            strcat(path, entry->d_name);

                            if (stat(path, &filestat) == -1) {
                                // Error: could not stat file
                                perror("Error File Stat");
                                syslog(LOG_ERR, EXIT_FAILURE);
                                exit(EXIT_FAILURE);
                            }

                            // check new created files
                            if(difftime(filestat.st_mtime, lastMove_time_t) > 0) {
                                printf("\n New File Added -> %s", strstr(entry->d_name, file_names[i]));
                                files_found += 1;
                            

                                // check to see what departments uploaded thier files
                                if (i == 0){
                                    fileCheck[i] += 1;
                                } else if(i == 1) {
                                    fileCheck[i] += 1;
                                } else if(i == 2) {
                                    fileCheck[i] += 1;
                                } else {
                                    fileCheck[i] += 1;
                                } 

                                if(i==rows-1){
                                    break;
                                }
                            }

                            // to see if a file was modified
                            // if(difftime(filestat.st_mtime, lastMove_time_t ) > 0 && difftime(filestat.st_ctime, lastMove_time_t) < 0) {
                            //     printf("\n Modified Files -> %s", strstr(entry->d_name, file_names[i]));

                            //     modified_files++;

                            // }
                        }
                    }
                }
            }
        }

        for (int i = 0; i < rows; i++)
        {
            (fileCheck[i] == 0) ? syslog(LOG_INFO, "File NOT Uploaded By %s", file_names[i]) : syslog(LOG_INFO, "File NOT Uploaded By %s", file_names[i]);
        }

        closedir(directory);

        printf("found %d files", files_found);

        // return (match_count > 0); 
    } else { // Parent process
        
    }

    return 0;
}

      