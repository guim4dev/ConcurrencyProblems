#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#define min(X, Y) (((X) < (Y)) ? (X) : (Y))

// Problema de Senate Bus, seção 7.4

int busStopped = 0;
int peopleBoarded = 0;
int peopleAbleToBoard = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t busArrived = PTHREAD_COND_INITIALIZER;
pthread_cond_t busFull = PTHREAD_COND_INITIALIZER;

void *busStop(void *arg) {
  while(1) {
    pthread_mutex_lock(&mutex);
    printf("Bus Arrives\n");
    busStopped = 1;
    if (peopleAbleToBoard != 0 ) {
      pthread_cond_broadcast(&busArrived);
      pthread_cond_wait(&busFull, &mutex);
    }
    printf("Bus leaving with %d people\n", peopleBoarded);
    peopleAbleToBoard = peopleAbleToBoard - peopleBoarded;
    busStopped = 0;
    peopleBoarded = 0;
    pthread_mutex_unlock(&mutex);
  }
}

void *rider(void *arg) {
  int canBoard = 0;
  int didntTravel = 1;
  while (didntTravel) {
    pthread_mutex_lock(&mutex);
    while (busStopped == 0) {
      if (canBoard == 0) {
        printf("Rider esperando bus chegar\n");
        peopleAbleToBoard++;
      }
      canBoard = 1;
      pthread_cond_wait(&busArrived, &mutex);
    }
    if (canBoard && peopleBoarded < 50) {
      printf("Rider entrando no bus\n");
      peopleBoarded++;
      didntTravel = 0;
      if (peopleBoarded == min(peopleAbleToBoard, 50)) {
        printf("Rider avisa para o motorista que o bus está completo com as pessoas que podem subir\n");
        pthread_cond_signal(&busFull);
      }
    }
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}


int main(void) {
  pthread_t travelers[200];
  pthread_t station;

  pthread_create(&station, NULL, busStop, NULL);

  for (int i = 0; i < 200; i++) {
    pthread_create(&travelers[i], NULL, rider, NULL);
  }

  // Programa só finaliza quando terminar a execução da thread do Reitor (nunca)
  pthread_join(station, NULL);
  return 0;
}
