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
#include <pthread.h>

int potentialCPatients = 0;
sem_t coronaPatient, fluPatient;

int Test()
{
    return rand() % 2;
}

void patientDischarged()
{
    printf("Patient Discharged.....\n");
}

void PatientRests()
{
    printf("Patient with Corona is now resting.....\n");
    sem_wait(&coronaPatient);
}

void* patient(void* param)
{
    potentialCPatients++;
    if(!Test()) // negative (corona does not exists/healthy)
    {
        //
        sem_post(&coronaPatient);
        printf("Negative once\n");
        if(!Test())
        {
            printf("Negative twice.....\n");
	    patientDischarged();
	    potentialCPatients--;            
        }
        else
            sem_post(&fluPatient);
        
    }
    else // negative (corona does not exist/healthy)
    {
        potentialCPatients--;
	PatientRests();
        sem_post(&fluPatient);
    }
    
    pthread_exit(NULL);
}

int main()
{
    printf("\n\n          *************Corona Management System****************\n\n\n");
    printf("How many patients are there? ");
    int noOfPatients; 
    scanf("%d", &noOfPatients);
    printf("Now Treating Patients.....\n\n");
    sem_init(&coronaPatient, 0, 0);
    sem_init(&fluPatient, 0, 0);

    pthread_t patientThread;
    for(int i = 0; i < noOfPatients; i++)
        pthread_create(&patientThread, NULL, &patient, NULL);

    for(int i = 0; i < noOfPatients; i++)
        pthread_join(patientThread, NULL);
    sem_destroy(&coronaPatient);
    sem_destroy(&fluPatient);
}
