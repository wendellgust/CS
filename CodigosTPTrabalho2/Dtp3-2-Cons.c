#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "Dtp3.h"

struct flock lock;

void *oddNumbersConsumer(void *vargp)
{
    int prod_file_descriptor = *(int *)vargp;
    int file_descriptor_odd;
    unsigned int number;
    char buffer[BUFFER_SIZE];
    int read_size;

    // TODO - Open the odd numbers file so that:
    // - only write-only operations are allowed
    // - file is created if not existing
    // - if file exists, overwrite existing data
    // - set permissions to 0644

    file_descriptor_odd = open(ODD_NUMBERS_FILE_NAME, O_WRONLY|O_CREAT|O_TRUNC, 0644);

    if (file_descriptor_odd == -1)
    {
        fprintf(stderr, "Error: %s: %s\n", ODD_NUMBERS_FILE_NAME, strerror(errno));
        return NULL;
    }

    while (1)
    {
        // TODO - Read one number from the producers file. Recall
        // that numbers are in text format with 9 characters.
        lock.l_type = F_RDLCK;
        if(fcntl(prod_file_descriptor, F_SETLKW, &lock) < 0) exit(-1);

        read_size = read(prod_file_descriptor, buffer, 9);

        lock.l_type=F_UNLCK;
        if(fcntl(prod_file_descriptor, F_SETLK, &lock)<0) exit(-1); 

        if (read_size > 0)
        {   // Convert text format (string) to integer
            number = atoi(buffer);
            if (number % 2 != 0)
            {
                printf("Storing odd number %u\n", number);
                int size = snprintf(buffer, BUFFER_SIZE, "%09d\n", number);

                // TODO - Store formated odd number into the odd numbers file
                lock.l_type = F_WRLCK;
                if(fcntl(file_descriptor_odd, F_SETLKW, &lock)<0) exit(-1);

                write(file_descriptor_odd, buffer, size);

                lock.l_type = F_UNLCK;
                if(fcntl(file_descriptor_odd, F_SETLK, &lock)<0) exit(-1);
            }
        }
        usleep(250000);
    }

    return NULL;
}

void *evenNumbersConsumer(void *vargp)
{
    int file_descriptor = *(int *)vargp;
    int file_descriptor_even;
    unsigned int number;
    char buffer[BUFFER_SIZE];
    int read_size;

    // TODO - Open the even numbers file so that:
    // - only write-only operations are allowed
    // - file is created if not existing
    // - if file exists, overwrite existing data
    // - set permissions to 0644

    file_descriptor_even = open(EVEN_NUMBERS_FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (file_descriptor_even == -1)
    {
        fprintf(stderr, "Error: %s: %s\n", EVEN_NUMBERS_FILE_NAME, strerror(errno));
        return NULL;
    }

    while (1)
    {
        // TODO - Read one number from the producers file. Recall
        // that numbers are in text format with 9 characters.
        lock.l_type = F_RDLCK;
        if(fcntl(file_descriptor, F_SETLKW, &lock) < 0) exit(-1);

        read_size = read(file_descriptor, buffer, 9);

        lock.l_type=F_UNLCK;
        if(fcntl(file_descriptor, F_SETLK, &lock)<0) exit(-1); 

        if (read_size > 0)
        {   // Convert text format (string) to integer
            number = atoi(buffer);
            if (number % 2 == 0)
            {
                printf("Storing even number %u\n", number);
                int size = snprintf(buffer, BUFFER_SIZE, "%09d\n", number);
                
                // TODO - Store formated even number into the even numbers file
                lock.l_type = F_WRLCK;
                if(fcntl(file_descriptor_even, F_SETLKW, &lock)<0) exit(-1);

                write(file_descriptor_even, buffer, size);

                lock.l_type = F_UNLCK;
                if(fcntl(file_descriptor_even, F_SETLK, &lock)<0) exit(-1);
            }
        }
        usleep(250000);
    }

    return NULL;
}

int main()
{
    pthread_t tid_1, tid_2;

    lock.l_whence = SEEK_SET; /* base for offset of block to be locked */
    lock.l_start = 0; /* start of block to be locked */
    lock.l_len = 0; /* its length (0 means until end of the file) */
    lock.l_pid = getpid();
    
    // TODO - Open the producer file so that:
    // - only read operations are allowed
    // - file is created if not existing
    // - set permissions to 0644
    // Hint: remember that both threads will read from the same
    // file and how file descriptors track the progress within
    // the file....
    int file_descriptor = open(PRODUCER_FILE_NAME,O_RDONLY|O_CREAT ,0644);

    if (file_descriptor == -1)
    {
        fprintf(stderr, "Error: %s: %s\n", PRODUCER_FILE_NAME, strerror(errno));
        return -1;
    }

    pthread_create(&tid_1, NULL, oddNumbersConsumer, (void *)&file_descriptor);
    pthread_create(&tid_2, NULL, evenNumbersConsumer, (void *)&file_descriptor);

    pthread_join(tid_1, NULL);
    pthread_join(tid_2, NULL);

    exit(0);
}
