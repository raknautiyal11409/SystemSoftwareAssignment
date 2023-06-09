
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <syslog.h>
#include <stdlib.h>

//rm #include <stdio.h>
#include <stdio.h>

void unlock_directories() {
// void main() {

    int pid, status;
    char* dash_dir = "Dashboard_Directory";
    char* shared_dir = "SharedDirectory";
    
    pid = fork(); // create a child process
    
    if (pid == -1) { // fork failed
        perror("fork");
        syslog(LOG_ERR, "Unlocking DIR: Failed FORK ");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) { // child process
        if (chmod(dash_dir , S_IRWXU | S_IRGRP | S_IROTH) == -1) { //change permission to read, write and execute for dashboard directory
            perror("Unlocking DIR Error");
            syslog(LOG_ERR, "Unlocking DIR: Failed to change permission %s ", dash_dir);
            exit(EXIT_FAILURE); 
        }

        printf("Permission of %s changed.\n", dash_dir);
        syslog(LOG_INFO, "Permission of %s changed.\n", dash_dir);
        exit(EXIT_SUCCESS);
    }
    else { // parent process
        if (chmod(shared_dir , S_IRWXU | S_IRWXG | S_IROTH) == -1) { //change permission to read, write and execute for shared directory
            perror("Unlocking DIR Error");
            syslog(LOG_ERR, "Unlocking DIR: Failed to change permission %s ", shared_dir);
            exit(EXIT_FAILURE);
        }
        printf("Permission of %s changed.\n", shared_dir);
        syslog(LOG_INFO, "Permission of %s changed.\n", shared_dir);
        
        wait(&status); // wait for child process to finish
    }

    return 0;
}
        