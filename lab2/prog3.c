#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 4096

int compare_files(const char *file1, const char *file2, size_t *bytes_read) {
    int fd1 = open(file1, O_RDONLY);
    int fd2 = open(file2, O_RDONLY);
    if (fd1 < 0 || fd2 < 0) return -1;

    char buf1[BUF_SIZE], buf2[BUF_SIZE];
    ssize_t r1, r2;
    *bytes_read = 0;

    do {
        r1 = read(fd1, buf1, BUF_SIZE);
        r2 = read(fd2, buf2, BUF_SIZE);
        *bytes_read += r1 + r2;
        if (r1 != r2 || memcmp(buf1, buf2, r1) != 0) {
            close(fd1); close(fd2);
            return 0; // разные
        }
    } while (r1 > 0 && r2 > 0);

    close(fd1); close(fd2);
    return 1; // одинаковые
}

void process_pair(const char *path1, const char *path2) {
    size_t bytes = 0;
    int result = compare_files(path1, path2, &bytes);
    printf("[PID %d] %s <-> %s\n", getpid(), path1, path2);
    printf("  Просмотрено байт: %zu\n", bytes);
    printf("  Результат: %s\n\n", result == 1 ? "Одинаковые" : "Разные");
}

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

	struct dirent *f1, *f2;
    char path1[512], path2[512];
    int active_procs = 0;

    while ((f1 = readdir(d1)) != NULL) {
        if (f1->d_type != DT_REG) continue;
        snprintf(path1, sizeof(path1), "%s/%s", dir1, f1->d_name);

        rewinddir(d2);
        while ((f2 = readdir(d2)) != NULL) {
            if (f2->d_type != DT_REG) continue;
            snprintf(path2, sizeof(path2), "%s/%s", dir2, f2->d_name);

            while (active_procs >= procNum) {
                wait(NULL);
                active_procs--;
            }

            pid_t pid = fork();
            if (pid == 0) {
                process_pair(path1, path2);
                exit(0);
            } else if (pid > 0) {
                active_procs++;
            } else {
                perror("Ошибка fork");
            }
        }
    }

    while (active_procs > 0) {
        wait(NULL);
        active_procs--;
    }


	closedir(d1);
	closedir(d2);
	return 0;
}
