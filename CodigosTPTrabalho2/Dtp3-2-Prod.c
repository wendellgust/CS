#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "Dtp3.h"

struct flock lock;

void *numbersProducer(void *vargp)
{
    
    unsigned int number;
    char buffer[BUFFER_SIZE];
    int file_descriptor = *(int *)vargp;

    while (1)
    {
        number = rand() % 1000;
        printf("Generated number %u\n", number);
        // Prepare data to be written to file (integer to fixed-size string format)
        int size = snprintf(buffer, BUFFER_SIZE, "%09d\n", number);
        // TODO - Write to file

        lock.l_type = F_WRLCK;
        if( fcntl( file_descriptor, F_SETLKW, &lock ) < 0 )
        exit(-1); // error in acquiring the lock

        write(file_descriptor, buffer, size);
        
        
        lock.l_type = F_UNLCK;
        if (fcntl(file_descriptor, F_SETLK, &lock) < 0) // no need to wait, command F_SETLK
        exit(-1); // error in releasing the lock

        sleep(1);
    }

    return NULL;
}

int main()
{
    int file_descriptor = -1;
    pthread_t tid_1;
    /* F_WRLCK write lock, F_RDLCK read lock, F_UNLCK unlock */
    lock.l_whence = SEEK_SET; /* base for offset of block to be locked */
    lock.l_start = 0; /* start of block to be locked */
    lock.l_len = 0; /* its length (0 means until end of the file) */
    lock.l_pid = getpid();

    // TODO - Open file so that:
    // - only write-only operations are allowed
    // - file is created if not existing
    // - if file exists, append data (don't overwrite file)
    // - set permissions to 0644
    file_descriptor = open(PRODUCER_FILE_NAME, O_WRONLY | O_CREAT| O_APPEND, 0644);

    if (file_descriptor == -1)
    {
        fprintf(stderr, "Error: %s: %s\n", PRODUCER_FILE_NAME, strerror(errno));
        return -1;
    }

    // Modify the following statement if necessary (hint: it is!)
    pthread_create(&tid_1, NULL, numbersProducer, (void *)&file_descriptor);

    pthread_join(tid_1, NULL);

    exit(0);
}
