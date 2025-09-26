#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>

#define BUFF_SIZE 4096;

int main() {
	char dir1[256], dir2[256];
	int procNum;

	printf("Введите 1 путь к папке: ");
	scanf("%256s", dir1);
	printf("Введите 2 путь к папке: ");
	scanf("%256s", dir2);
	printf("Введите количество процессов: ");
	scanf("%d", &procNum);

	DIR *d1 = opendir(dir1);
	DIR *d2 = opendir(dir2);

	if(!d1 || !d2){
		perror("Can't open dirs");
		return 1;
	}

	closedir(d1);
	closedir(d2);
	return 0;
}
