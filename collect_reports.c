#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pwd.h>


void collect_reports(time_t lastMove_time_t){
// void main() {
    DIR *dir;
    struct dirent *ent;
    int count = 0;
    char* shared_dir = "SharedDirectory/";
    char* dash_dir = "Dashboard_Directory/";

    // struct tm lastMove_time = {0}; // initialize a tm struct to all zeroes
    // time_t lastMove_time_t;        // declare a time_t variable

    // // set the fields of the tm struct to the desired values
    // lastMove_time.tm_year = 2023 - 1900; // year - 1900
    // lastMove_time.tm_mon = 2;            // month (0-11)
    // lastMove_time.tm_mday = 10;          // day of the month
    // lastMove_time.tm_hour = 21;           // hour (24-hour clock)
    // lastMove_time.tm_min = 20;            // minute
    // lastMove_time.tm_sec = 0;            // second

    // // convert the tm struct to a time_t value
    // lastMove_time_t = mktime(&lastMove_time);

    FILE *logFile;
    FILE *filesInDash;
    char tempFileName[150];
    int col = 100;
    int row = 0;
    int numberOfLines = 0;
    char file_names[4][20] = {"warehouse", "manufacturing", "distribution", "sales"};
    int rows = sizeof(file_names) / sizeof(file_names[0]);

    logFile = fopen("FilesMovedToDashboardReport.txt", "a");
    filesInDash = fopen("FilesMovedToDash.txt", "a+");

    if (logFile == NULL || filesInDash == NULL)
    {
        printf("Error opening file!\n");
        exit(EXIT_FAILURE);
    }

    // Count the number of lines in the file
    while (fgets(tempFileName, col, filesInDash) != NULL)
    {
        numberOfLines++;
    }

    // Dynamically allocate a 2D array of chars
    char **copiedFileNames = malloc(numberOfLines * sizeof(char *));
    for (int i = 0; i < numberOfLines; i++)
    {
        copiedFileNames[i] = malloc(col * sizeof(char));
    }

    rewind(filesInDash);
    
    int x = 0; 
    while (fgets(tempFileName, col, filesInDash) != NULL)
    {
        // Copy the string from str to the i-th row of the text array
        sscanf(tempFileName, "%[^\n]", copiedFileNames[x]);
        x++;
    }

    // Open the directory
    dir = opendir(shared_dir);
    if (dir == NULL)
    {
        printf("Error opening directory: Shared Directory\n");
        perror("Collect Report");
        exit(EXIT_FAILURE);
    }


    // get current time
    time_t current_time = time(NULL);
    if (current_time == (time_t)(-1)) {
        perror("time");
        exit(EXIT_FAILURE);
    }

    struct tm *local_time = localtime(&current_time);
    if (local_time == NULL) {
        perror("localtime");
        exit(EXIT_FAILURE);
    }

    // convert into readable format
    char time_string[80];
    if (strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", local_time) == 0) {
        perror("convert time");
        exit(EXIT_FAILURE);
    }

    // Print to log file
    if (fprintf(logFile, "\n -- ATTEMPT to copy files on %s --\n", time_string) < 0) {
        perror("fprintf");
        exit(EXIT_FAILURE);
    }

    int status;

    // Fork child processes to copy each file to the dashboard folder
    while ((ent = readdir(dir)) != NULL)
    {

        struct stat filestat;
        time_t *mtime;
        time_t *ctime;
        char *path = (char *)malloc((strlen(shared_dir) + strlen(ent->d_name) + 1) * sizeof(char));
        strcpy(path, shared_dir);
        strcat(path, ent->d_name);
        int alreadyCopied = 0;

        printf("\n %d",difftime(filestat.st_mtime, lastMove_time_t));

        if (stat(path, &filestat) == -1)
        {
            // Error: could not stat file
            perror("Error");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < rows; i++)
        {

            // Check if file name matches
            if (strstr(ent->d_name, file_names[i]) != NULL)
            {

                // check if a file has already been copied
                for (int i = 0; i < numberOfLines; i++)
                {
                    if (strcmp(copiedFileNames[i], ent->d_name) != 0)
                    {
                        alreadyCopied = 1;
                    }
                }

                // check if a previously modified file has been copied
                if (difftime(filestat.st_mtime, lastMove_time_t) < 0 && alreadyCopied)
                {
                    break;
                }

                char new_or_modified[20] = "MODIFIED FILE";

                if (!alreadyCopied)
                {
                    strcpy(new_or_modified, "NEW FILE");
                    fprintf(filesInDash, "%s\n", ent->d_name);
                }

                // Get modified time/date
                char modifiedTime[20];
                strftime(modifiedTime, sizeof(modifiedTime), "%Y-%m-%d %H:%M:%S", localtime(&filestat.st_mtime));

                //get username
                struct passwd *user = getpwuid(filestat.st_uid);
                if (user == NULL) {
                    perror("Collect reports getpwuid");
                    exit(EXIT_FAILURE);
                }

                // Log file transfers
                fprintf(logFile, "%s  %s  %s  %s\n", ent->d_name, modifiedTime, user->pw_name, new_or_modified);

                pid_t pid = fork();

                if (pid == -1)
                {
                    printf("Error forking child process: %s\n", strerror(errno));
                    syslog(LOG_ERR, "BackUP Dash:Error forking child process: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
                else if (pid == 0)
                {

                    // Child process copies the file
                    char dest[50];
                    strcpy(dest, dash_dir);
                    strcat(dest, ent->d_name);

                    if (execlp("cp", "cp", path, dest, NULL) == -1)
                    {
                        printf("Error copying file: %s\n", strerror(errno));
                        syslog(LOG_ERR, "Error copying file: %s\n", strerror(errno));
                        exit(EXIT_FAILURE);
                    }

                    // record successful file transfer
                    syslog(LOG_INFO, "Backup DIR: Successfully moved %s to Dashboard", ent->d_name);
                    exit(EXIT_SUCCESS);
                }
            }
        }
    }

    // Wait for all child processes to complete
    for (int i = 0; i < count; i++)
    {
        int status;
        wait(&status);
        if (WIFEXITED(status))
        {
            printf("File copied successfully.\n");
        }
        else
        {
            printf("Error copying file\n");
        }
    }

    free(copiedFileNames);
    fclose(logFile);
    fclose(filesInDash);
    // Close the directory
    closedir(dir);
}
