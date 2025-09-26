#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

void print_info(const char *label){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	struct tm *tm_info = localtime(&tv.tv_sec);

	printf("[%s] PID: %d, PPID: %d, Время: %02d:%02d:%02d:%03ld\n", label, getpid(), getppid(), tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, tv.tv_usec / 1000);
}

int main(){
	pid_t pid1, pid2;

	print_info("Родитель");

	pid1 = fork(); 
	if(pid1 == 0){
		print_info("Дочерний 1");
		exit(0);
	}
	
	pid2 = fork(); 
	if(pid2 == 0){
		print_info("Дочерний 2");
		exit(0);
	}

	sleep(1);
	printf("\n Список процессов: \n");
	system("ps -x | grep out");

	return 0;
}
