#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// Error exit
void report_and_exit(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

// Producer: writes data to the device
void producer(const char* path, const char* str) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) report_and_exit("producer open failed");

    ssize_t written = write(fd, str, strlen(str));
    if (written < 0) report_and_exit("Escrevir failed");

    printf("Escrevir %zd bytes\n", written);
    close(fd);
}

// Consumer: reads data from the device
void consumer(const char* path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) report_and_exit("consumer open failed");

    char buffer[1024];
    ssize_t n;

    printf("Consumer is lendo...\n");
    while ((n = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        printf("Consumer read: %s\n", buffer);
    }

    if (n < 0) report_and_exit("read falha");

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 3)
        report_and_exit("Usage: ./test_pipe /dev/pipe \"message\"");

    const char* path = argv[1];
    const char* message = argv[2];

    pid_t pid = fork();
    if (pid < 0) {
        report_and_exit("fork  falha");
    } else if (pid == 0) {
        // Child: consumer
        sleep(1);  // Let producer go first
        consumer(path);
        _exit(0);
    } else {
        // Parent: producer
        producer(path, message);
        wait(NULL);  // Wait for consumer to finish
    }

    return 0;
}
