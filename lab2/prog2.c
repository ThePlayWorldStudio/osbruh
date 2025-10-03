#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

#define BUF_SIZE 4096

int file_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

ssize_t copy_file(const char *src, const char *dst) {
    int in_fd = open(src, O_RDONLY);
    if (in_fd < 0) return -1;

    struct stat st;
    fstat(in_fd, &st);

    int out_fd = open(dst, O_WRONLY | O_CREAT | O_EXCL, st.st_mode);
    if (out_fd < 0) {
        close(in_fd);
        return -1;
    }

    char buf[BUF_SIZE];
    ssize_t total = 0, bytes;
    while ((bytes = read(in_fd, buf, BUF_SIZE)) > 0) {
        write(out_fd, buf, bytes);
        total += bytes;
    }

    close(in_fd);
    close(out_fd);
    return total;
}

void sync_file(const char *src_path, const char *dst_path) {
    ssize_t copied = copy_file(src_path, dst_path);
    printf("[PID %d] Копирован файл: %s\n", getpid(), src_path);
    printf("  Скопировано байт: %zd\n\n", copied >= 0 ? copied : 0);
}

int main() {
    char dir1[256], dir2[256];
    int max_procs;

    printf("Введите путь к Dir1: ");
    scanf("%255s", dir1);
    printf("Введите путь к Dir2: ");
    scanf("%255s", dir2);
    printf("Введите максимальное число процессов: ");
    scanf("%d", &max_procs);

    DIR *d1 = opendir(dir1);
    if (!d1) {
        perror("Ошибка открытия Dir1");
        return 1;
    }

    struct dirent *entry;
    int active_procs = 0;

    while ((entry = readdir(d1)) != NULL) {
        if (entry->d_type != DT_REG) continue;

        char src_path[512], dst_path[512];
        snprintf(src_path, sizeof(src_path), "%s/%s", dir1, entry->d_name);
        snprintf(dst_path, sizeof(dst_path), "%s/%s", dir2, entry->d_name);

        if (file_exists(dst_path)) continue;

        while (active_procs >= max_procs) {
            wait(NULL);
            active_procs--;
        }

        pid_t pid = fork();
        if (pid == 0) {
            sync_file(src_path, dst_path);
	    system("pstree | grep -i alacritty");
            exit(0);
        } else if (pid > 0) {
            active_procs++;
        } else {
            perror("Ошибка fork");
        }
    }

    closedir(d1);

    while (active_procs > 0) {
        wait(NULL);
        active_procs--;
    }

    return 0;
}
