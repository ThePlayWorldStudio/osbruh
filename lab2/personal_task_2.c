#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void print_id(const char* msg)
{
    pid_t cur_pid = getpid();
    pid_t parent_pid = getppid();
    printf("%s:\nPID = %d\nPPID = %d\n\n", msg, cur_pid, parent_pid);
}

pid_t create_process(int child_num)
{
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // child
        printf("Process %d was created\n", child_num);
        print_id("");
        return pid;
    }
    else {
        // parent
        printf("Process %d forked to process %d.\n\n", getpid(), pid);
        return pid;
    }
}

int main() {
    print_id("Program start");

    // process  0 1 2 3 4 5 6 7    
    // parents  # 0 1 2 2 3 4 6
    pid_t pids[8];

    pids[1] = create_process(1);
    if (pids[1] == 0) 
    {
        // process 1
        pids[2] = create_process(2);

        if (pids[2] == 0) 
        {
            // process 2
            pids[3] = create_process(3); 
            
            if (pids[3] == 0) 
            {
                // process 3
                pids[5] = create_process(5);

                if (pids[5] == 0) 
                {
                    // process 5
                    print_id("Process 5 before ending");
                    printf("Process with PID = %d and PPID = %d is ending.\n\n", getpid(), getppid());
                    exit(EXIT_SUCCESS);
                } else {
                    // process 3
                    waitpid(pids[5], NULL, 0);
                    print_id("Process 3 before ending");
                    printf("Process with PID = %d and PPID = %d is ending.\n\n", getpid(), getppid());
                    exit(EXIT_SUCCESS);
                }
            } else {
                // process 2
                pids[4] = create_process(4);

                if (pids[4] == 0) 
                {
                    // process 4
                    pids[6] = create_process(6);

                    if (pids[6] == 0) 
                    {
                        // process 6
                        pids[7] = create_process(7);

                        if (pids[7] == 0)
                        {
                            // process 7
                            print_id("Process 7 before ending");
                            printf("Process with PID = %d and PPID = %d is ending.\n\n", getpid(), getppid());
                            exit(EXIT_SUCCESS);
                        } else {
                            // process 6
                            waitpid(pids[7], NULL, 0);
                            print_id("Process 6 before ending");
                            printf("Process with PID = %d and PPID = %d is ending.\n\n", getpid(), getppid());
                            exit(EXIT_SUCCESS);
                        }
                    } else {
                        // process 4
                        waitpid(pids[6], NULL, 0);
                        print_id("Process 4 before ending");
                        printf("Process with PID = %d and PPID = %d is ending.\n\n", getpid(), getppid());
                        exit(EXIT_SUCCESS);
                    }
                } else {
                    // process 2
                    waitpid(pids[3], NULL, 0);
                    waitpid(pids[4], NULL, 0);
                    
                    printf("Process 2 complete exec for command 'ps aux'\n\n");
                    
                    // process 2 complete a command "ps"
                    execlp("ps", "ps", "aux", NULL);
                    
                    perror("exec failure");
                    exit(EXIT_FAILURE);
                }
            } 
        } else {
            // process 1
            waitpid(pids[2], NULL, 0);
            print_id("Process 1 before ending");
            printf("Process with PID = %d and PPID = %d is ending.\n\n", getpid(), getppid());
            exit(EXIT_SUCCESS);
        }
    }
    else 
    {
        // process 0
        waitpid(pids[1], NULL, 0);
        print_id("Core process before ending");
        printf("Process with PID = %d and PPID = %d is ending.\n\n", getpid(), getppid());
        exit(EXIT_SUCCESS);
    }

    return 0;
}