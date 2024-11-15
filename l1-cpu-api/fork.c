#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int orig();
int hiBye();
int ls();
int userPipe();

int main() {
    userPipe();
}

int ls() {
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        return 1;
    } else if (pid == 0) {
        printf("This is the child process: %d\n", getpid());
        char *args[] = {"ls", "-l", NULL};
        execvp("/bin/ls", args);
    } else {
        int status;
        wait(NULL);
        printf("this is the parent process %d\n", getpid());
    }
}

int orig() {
    pid_t pid;
    pid = fork();
    int fd = open("text.txt", O_RDONLY);
    char buffer[100];
    ssize_t bytes_read;
    bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read == -1) {
        printf("Error reading file");
        return 1;
    }

    buffer[bytes_read] = '\0';
    printf("File contents: \n%s, pid: \n%d\n", buffer, pid);
    close(fd);
    return 0;
}

int hiBye() {
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        return 1;
    } else if (pid == 0) {
        printf("This is the child process: %d\n", getpid());
        printf("Hello");
    } else {
        int status;
        waitpid(pid, &status, 0);
        printf("this is the parent process %d\n", getpid());
        printf("Goodbye");
    }
    return 0;
}

int userPipe() {
    pid_t pid1, pid2;
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    // Fork the first child
    pid1 = fork();
    if (pid1 == 0) {
        close(pipefd[0]);
        const char *message = "Hello OOOOOOO";
        write(pipefd[1], message, strlen(message) + 1);
        printf("Child 1 (PID: %d) is running.\n", getpid());
        sleep(2);  // Simulate work
        printf("Child 1 (PID: %d) is exiting.\n", getpid());
        close(pipefd[1]);
        exit(1);   // Exit with status 1
    }
    // Parent process waits for both children individually
    int status;
    pid_t finished_pid;

    // Wait for the first child to finish
    finished_pid = waitpid(pid1, &status, 0);
    if (finished_pid == pid1) {
        if (WIFEXITED(status)) {
            printf("Child 1 exited with status %d.\n", WEXITSTATUS(status));
        }
    }

    // Fork the second child
    pid2 = fork();
    if (pid2 == 0) {
        printf("Child 2 (PID: %d) is running.\n", getpid());
        close(pipefd[1]);
        char buffer[100];
        // receive message
        read(pipefd[0], buffer, sizeof(buffer));
        printf("CHILD 2 Received message: %s\n", buffer);
        // Second child process
        sleep(1);  // Simulate work
        printf("Child 2 (PID: %d) is exiting.\n", getpid());
        close(pipefd[0]);
        exit(2);   // Exit with status 2
    }



    // Wait for the second child to finish
    finished_pid = waitpid(pid2, &status, 0);
    if (finished_pid == pid2) {
        if (WIFEXITED(status)) {
            printf("Child 2 exited with status %d.\n", WEXITSTATUS(status));
        }
    }

    return 0;
}
