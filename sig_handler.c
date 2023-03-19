#include <unistd.h>
#include <syslog.h>
#include "daemon_task.h"
#include <signal.h>
#include <stdlib.h>

void sig_handler(int sigNum)
{
	if (sigNum == SIGINT) {
		syslog(LOG_INFO, "RECEIVED SIGNAL INTERRUPT, INITIATING BACKUP AND TRANSFER");
		lock_directories();
		collect_reports(lastMove_time_t);
		backup_dashboard(lastMove_time_t);
		sleep(30);
		unlock_directories();	
		lastMove_time_t == time(NULL);
	}

}