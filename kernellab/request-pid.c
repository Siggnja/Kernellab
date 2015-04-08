#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "pidinfo.h"

/* run_current()
 * We allocate memory for the PID and write it the kcurrent file.
 * Kcurrent file is opened with write privaledges and closed when done.
 * We make sure to free the memory when everything has finished operation.
 */
void run_current()
{
	int *pid = malloc(sizeof(int));

	FILE* kcurrent = fopen("/sys/kernel/kernellab/kcurrent", "w");
	fprintf(kcurrent, "%p", pid);
	fclose(kcurrent); 

	printf("Current PID: %d\n", *pid);

	free(pid);
}

/* run_pid()
 * We allocate memory for the sysfs struct.
 * We assign pid with getpid call and address to info via refernece.
 * Pid file is opened with write privaledges and closed when done
 * We make sure to free the memory when everything has finished operation.
 */
void run_pid()
{
	struct pid_info info;

	struct sysfs_message *message = malloc(sizeof(struct sysfs_message));
	message->pid = getpid();
	message->address = &info;

	FILE* pidF = fopen("/sys/kernel/kernellab/pid", "w");
	fprintf(pidF, "%p", message);
	fclose(pidF); 

	printf("PID: %d\n", info.pid);
	printf("COMM: %s\n", info.comm);
	printf("State: %ld\n", info.state);

	free(message);
} 

int main()
{
        run_current();    
        run_pid();
        return EXIT_SUCCESS;
}
