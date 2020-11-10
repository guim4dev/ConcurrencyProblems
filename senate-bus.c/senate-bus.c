#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Problema de Senate Bus, seção 7.4

int busStopped = 0;
int peopleBoarded = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t busArrived = PTHREAD_COND_INITIALIZER;
pthread_cond_t busFull = PTHREAD_COND_INITIALIZER;

void *busStop(void *arg) {
  while(1) {
    pthread_mutex_lock(&mutex);
    printf("Bus Arrives\n");
    pthread_cond_signal(&busArrived);
    pthread_cond_wait(&busFull, &mutex);
    printf("Bus leaving with %d\n", peopleBoarded);
    busStopped = 0;
    peopleBoarded = 0;
    pthread_mutex_unlock(&mutex);
    sleep(1); // Wait a minute until another bus gets here
  }
}

void *rider(void *arg) {
  int canBoard = 0;
  int didntTravel = 1;
  while (didntTravel) {
    pthread_mutex_lock(&mutex);
    while (busStopped == 0) {
      printf("Rider esperando bus chegar\n");
      canBoard = 1;
      pthread_cond_wait(&busArrived, &mutex);
    }
    if (canBoard && peopleBoarded < 50) {
      printf("Rider entrando no bus\n");
      peopleBoarded++;
      didntTravel = 0;
      if (peopleBoarded == 50) {
        printf("Rider avisa para o motorista que o bus está cheio\n");
        pthread_cond_signal(&busFull);
      }
    }
    pthread_mutex_unlock(&mutex);
  }
}


int main(void) {
  pthread_t students[100];
  pthread_t Dean;
  // Criando Thread para o Reitor
  pthread_create(&Dean, NULL, dean, NULL);

  for (int i = 0; i < 100; i++) {
    pthread_create(&students[i], NULL, student, NULL);
  }

  // Programa só finaliza quando terminar a execução da thread do Reitor (nunca)
  pthread_join(Dean, NULL);
  return 0;
}
