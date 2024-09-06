#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s command1 [command2 ... commandN]\n", argv[0]);
        exit(EINVAL);
    }

    int num_cmnds = argc - 1; // because first argument is ./pipe
    int pipes[num_cmnds - 1][2];
    int status;
    pid_t pid;

    // Create all pipes before forking
    for (int i = 0; i < num_cmnds - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(errno);
        }
    }

    for (int i = 0; i < num_cmnds; i++) {
        pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(errno);
        }

        if (pid == 0) {
            // Child process
            if (i > 0) {
                // If not the first command, set stdin to read from the previous pipe
                if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(errno);
                }
            }
            if (i < num_cmnds - 1) {
                // If not the last command, set stdout to write to the current pipe
                if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(errno);
                }
            }

            // Close all pipe file descriptors in the child process
            for (int j = 0; j < num_cmnds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execlp(argv[i + 1], argv[i + 1], (char *)NULL);
            perror("execlp"); // If execlp fails, print error to stderr
            exit(errno); // Exit with the error code
        }
    }

    // Close all pipe file descriptors in the parent process
    for (int i = 0; i < num_cmnds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all child processes to finish and check their exit statuses
    for (int i = 0; i < num_cmnds; i++) {
        pid_t wpid = waitpid(-1, &status, 0);
        if (wpid == -1) {
            perror("waitpid");
            exit(errno);
        }
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            if (exit_status != 0) {
                fprintf(stderr, "Child process %d terminated with error status %d\n", wpid, exit_status);
                exit(exit_status);
            }
        } else {
            fprintf(stderr, "Child process %d did not terminate normally\n", wpid);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
