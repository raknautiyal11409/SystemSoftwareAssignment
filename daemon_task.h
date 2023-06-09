#ifndef DAEMON_TASKS 
#define DAEMON_TASKS

#include <time.h>

extern time_t lastMove_time_t;        // declare a time_t variable

void collect_reports(time_t);

void backup_dashboard(time_t);

void lock_directories(void);

void unlock_directories(void );

void generate_reports(void);

void check_file_uploads(time_t);

void sig_handler(int);

void update_timer(struct tm*);

#endif
