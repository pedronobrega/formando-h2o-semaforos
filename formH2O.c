#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

pthread_mutex_t criticalRegionMutex;
pthread_mutex_t threadMutex;
sem_t HSem;
sem_t OSem;

int HWaiting = 0;
int OWaiting = 0;

void* H(){
    pthread_mutex_lock(&threadMutex);
    pthread_mutex_lock(&criticalRegionMutex);
    HWaiting++;
    pthread_mutex_unlock(&criticalRegionMutex);
    if(HWaiting>=2 && OWaiting >=1){
        formH2O(1);
        printf("Duas molecuas de H usado\n");
        pthread_mutex_unlock(&threadMutex);
    } else {
        pthread_mutex_unlock(&threadMutex);
        sem_wait(&HSem);
        printf("Uma molecula de H usado\n");
    }
    pthread_exit("");

}

void* O(){
    pthread_mutex_lock(&threadMutex);
    pthread_mutex_lock(&criticalRegionMutex);
    OWaiting++;
    pthread_mutex_unlock(&criticalRegionMutex);
    if(HWaiting>=2 && OWaiting >=1){
        formH2O(0);
        printf("Uma molecula de O usado\n");
        pthread_mutex_unlock(&threadMutex);
    } else {
        pthread_mutex_unlock(&threadMutex);
        sem_wait(&OSem);
        printf("Uma molecula de O usado\n");
    }
    pthread_exit("");
}

void formH2O(int isH){
    pthread_mutex_lock(&criticalRegionMutex);
    printf("Formando H2O...\n");

    OWaiting--;

    HWaiting--;
    HWaiting--;

    pthread_mutex_unlock(&criticalRegionMutex);
    sem_post(&HSem);
    if(isH>0){
        sem_post(&OSem);
    } else {
        sem_post(&HSem);
    }
}

int main(int argc, char **argv){
    pthread_mutex_init(&criticalRegionMutex, NULL);
    pthread_mutex_init(&threadMutex, NULL);
    sem_init(&HSem, 0, 0);
    sem_init(&OSem, 0, 0);
    srand((unsigned)time(NULL));
    int numberOfMolecules = 100;
    pthread_t molecules[numberOfMolecules];
    for(int i=0;i<numberOfMolecules; i++){
        int isH = rand()%3;
        if(isH>0){
            pthread_create(&molecules[i], NULL, &H, (void *) i);
        } else {
            pthread_create(&molecules[i], NULL, &O, (void *) i);  
        }
    }
    for(int j=0; j<numberOfMolecules; j++){
        pthread_join(molecules[j], NULL);
    }
    return 0;
}