
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <syslog.h>
#include <stdlib.h>

#include <stdio.h>

void lock_directories() {
// void main() {

    int pid, status;
    char* dash_dir = "../Dashboard_Directory";
    char* shared_dir = "../SharedDirectory";
    
    pid = fork(); // create a child process
    
    if (pid == -1) { // fork failed
        perror("fork");
        syslog(LOG_ERR, "Locking DIR: %s ", EXIT_FAILURE);
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) { // child process (change permission for Dashboard Directory)
        if (chmod(dash_dir , S_IRWXU|S_IRGRP|S_IROTH) == -1) { //change permission 
            perror("chmod failed");
            syslog(LOG_ERR, "Locking DIR: %s ", EXIT_FAILURE);
            exit(EXIT_FAILURE); 
        }

        printf("Permission of %s changed.\n", dash_dir);
        syslog(LOG_INFO, "Permission of Shared Directory changed.\n", dash_dir);
        exit(EXIT_SUCCESS);
    }
    else { // parent process (change permission for Shared Directory)
        if (chmod(shared_dir , S_IRWXU|S_IRGRP|S_IROTH) == -1) { //change permission 
            perror("chmod failed");
            syslog(LOG_ERR, "Locking DIR: %s ", EXIT_FAILURE);
            exit(EXIT_FAILURE);
        }
        printf("Permission of Shared Directory changed.\n", shared_dir);
        syslog(LOG_INFO, "Permission of %s changed.\n", shared_dir);
        wait(&status); // wait for child process to finish
    }

    return 0;
}