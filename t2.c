#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

int main()
{
    // Shared Memory Variables
    int id_sem1=shmget(1390, 1024, 0666 | IPC_CREAT | IPC_EXCL);
    int id_sem2=shmget(1490, 1024, 0666 | IPC_CREAT | IPC_EXCL);
    int id_sem3=shmget(1590, 1024, 0666 | IPC_CREAT | IPC_EXCL);
    int id_sem4=shmget(1690, 1024, 0666 | IPC_CREAT | IPC_EXCL);
    int id_buffer1=shmget(1790, 10, 0666 | IPC_CREAT | IPC_EXCL);
    int id_buffer2=shmget(1890, 10, 0666 | IPC_CREAT | IPC_EXCL);
    sem_t *sem1 = (sem_t*) shmat(id_sem1, NULL, 0);
    sem_t *sem2 = (sem_t*) shmat(id_sem2, NULL, 0);
    sem_t *sem3 = (sem_t*) shmat(id_sem3, NULL, 0);
    sem_t *sem4 = (sem_t*) shmat(id_sem4, NULL, 0);
    sem_init(sem1, 1, 1);
    sem_init(sem2, 1, 0);
    sem_init(sem3, 1, 0);
    sem_init(sem4, 1, 0);

    pid_t pid = fork();
    pid_t pid2 = fork();
    if(pid > 0 && pid2 > 0) // Process A.
    {
        id_sem1=shmget(1390, 1024, 0);
	    id_sem2=shmget(1490, 1024, 0);
        id_buffer1=shmget(1790, 10, 0);
        sem_t *sem1 = (sem_t*) shmat(id_sem1, NULL, 0);
        sem_t *sem2 = (sem_t*) shmat(id_sem2, NULL, 0);
        char* buffer1 = (char*) shmat(id_buffer1, NULL, 0);
        sem_wait(sem1);
        int fd = open("1.txt", O_RDONLY);
        read(fd, buffer1, 10);
        sem_post(sem3);

        shmdt(sem1);
        shmdt(sem2);
        shmdt(buffer1);
    }
    else if(pid == 0 && pid2 > 0) // Process B.
    {
	    id_sem2=shmget(1490, 1024, 0);
        id_sem3=shmget(1590, 1024, 0);
        id_buffer1=shmget(1790, 10, 0);
        sem_t *sem2 = (sem_t*) shmat(id_sem2, NULL, 0);
        sem_t *sem3 = (sem_t*) shmat(id_sem3, NULL, 0);
        char* buffer1 = (char*) shmat(id_buffer1, NULL, 0);
        sem_wait(sem2);
        int fd = open("2.txt", O_RDONLY);
        read(fd, buffer1, 10);
        sem_post(sem3);

        // Detach shared memory variables.
        shmdt(sem2);
        shmdt(sem3);
        shmdt(buffer1);
    }
    else if(pid > 0 && pid2 == 0) // Process C.
    {
        id_sem3=shmget(1590, 1024, 0);
        id_sem4=shmget(1690, 1024, 0);
        id_buffer1=shmget(1790, 10, 0);
        id_buffer2=shmget(1890, 10, 0);
        sem_t *sem3 = (sem_t*) shmat(id_sem3, NULL, 0);
        sem_t *sem4 = (sem_t*) shmat(id_sem4, NULL, 0);
        char* buffer1 = (char*) shmat(id_buffer1, NULL, 0);
        char* buffer2 = (char*) shmat(id_buffer2, NULL, 0);
        sem_wait(sem3);
        strncpy(buffer2, buffer1, 10);
        sem_post(sem4);
        sem_wait(sem3);
        strncpy(buffer2, buffer1, 10);
        sem_post(sem4);

        // Detach shared variables.
        shmdt(sem3);
        shmdt(sem4);
        shmdt(buffer1);
        shmdt(buffer2);
    }
    else if(pid == 0 && pid2 == 0) // Process D.
    {
        // Process D needs to get it all again so it can delete at the end.
        id_sem1=shmget(1390, 1024, 0);
	    id_sem2=shmget(1490, 1024, 0);
        id_sem3=shmget(1590, 1024, 0);
	    id_sem4=shmget(1690, 1024, 0);
        id_buffer1=shmget(1790, 10, 0);
        id_buffer2=shmget(1890, 10, 0);
        sem_t *sem1 = (sem_t*) shmat(id_sem1, NULL, 0);
        sem_t *sem2 = (sem_t*) shmat(id_sem2, NULL, 0);
        sem_t *sem3 = (sem_t*) shmat(id_sem3, NULL, 0);
        sem_t *sem4 = (sem_t*) shmat(id_sem4, NULL, 0);
        char* buffer1 = (char*) shmat(id_buffer1, NULL, 0);
        char* buffer2 = (char*) shmat(id_buffer2, NULL, 0);
        sem_wait(sem4);
        printf("\n%s\n", buffer2);
        sem_post(sem2);
        sem_wait(sem4);
        printf("\n%s\n", buffer2);

        // Detach and delete shared memory.
        shmdt(sem1);
        shmdt(sem2);
        shmdt(sem3);
        shmdt(sem4);
        shmdt(buffer1);
        shmdt(buffer2);
        shmctl(id_sem1, IPC_RMID, NULL);
        shmctl(id_sem2, IPC_RMID, NULL);
        shmctl(id_sem3, IPC_RMID, NULL);
        shmctl(id_sem4, IPC_RMID, NULL);
        shmctl(id_buffer1, IPC_RMID, NULL);
        shmctl(id_buffer2, IPC_RMID, NULL);
    }
}
