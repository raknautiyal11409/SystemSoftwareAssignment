CC=gcc
objects = daemon.o check_file_uploads.o lock_directories.o collect_reports.o backup_dashboard.o unlock_directories.o generate_reports.o update_timer.o sig_handler.o

daemon: $(objects)
	$(CC) -o daemon $(objects)

daemon.o: daemon.c daemon_task.h
	$(CC) -c daemon.c

check_file_uploads.o: check_file_uploads.c
	$(CC) -c check_file_uploads.c

lock_directories.o: lock_directories.c
	$(CC) -c lock_directories.c

collect_reports.o: collect_reports.c
	$(CC) -c collect_reports.c

backup_dashboard.o: backup_dashboard.c
	$(CC) -c backup_dashboard.c

unlock_directories.o: unlock_directories.c
	$(CC) -c unlock_directories.c

generate_reports.o: generate_reports.c
	$(CC) -c generate_reports.c

update_timer.o: update_timer.c
	$(CC) -c update_timer.c

sig_handler.o: sig_handler.c
	$(CC) -c sig_handler.c

clean:
	rm daemon $(objects)

