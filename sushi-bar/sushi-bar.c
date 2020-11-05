#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int currentSeatsAvailable = 5;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t seatsAvailable = PTHREAD_COND_INITIALIZER; 
pthread_cond_t seatsFull = PTHREAD_COND_INITIALIZER;

void *manager(void *arg) {
  while(1) {
    printf("Manager tenta pegar o lock\n");
    pthread_mutex_lock(&mutex);
    while(currentSeatsAvailable > 0) {
      printf("Manager esperando alguém avisá-lo que a mesa está cheia\n");
      pthread_cond_wait(&seatsFull, &mutex);
    }

    printf("Manager retirando 5 pessoas da mesa (party leaving)\n");
    currentSeatsAvailable = 5;
    printf("Manager avisa que os 5 espaços estão livres\n");
    pthread_cond_signal(&seatsAvailable);
    pthread_mutex_unlock(&mutex);
  }
}

void *customerSits(void *arg) {
  while(1) {
    pthread_mutex_lock(&mutex);
    printf("CustomerSit pega o lock\n");
    while(currentSeatsAvailable == 0) {
      printf("Customer esperando as 5 pessoas saírem...\n");
      pthread_cond_wait(&seatsAvailable, &mutex);
    }
    printf("Customer ocupa um lugar.\n");
    currentSeatsAvailable--;
    pthread_mutex_unlock(&mutex);
    sleep(1);
  }
}

void *customerLeaves(void *arg) {
  while(1) {
    pthread_mutex_lock(&mutex);
    printf("CustomerLeave pega o lock\n");
    if (currentSeatsAvailable > 0 && currentSeatsAvailable < 5) {
      printf("Customer desocupa um lugar.\n");
      currentSeatsAvailable++;
    } else if (currentSeatsAvailable == 0) {
      pthread_cond_signal(&seatsFull);
    } else {
      printf("Ninguém está sentado.\n");
    }
    pthread_mutex_unlock(&mutex);
    sleep(1);
  }
}

int main(void) {
  int sitFrequency = 4;
  int leaveFrequency = 2;
  pthread_t customersSitting[sitFrequency];
  pthread_t customersLeaving[leaveFrequency];
  pthread_t sushi_manager;
  pthread_create(&sushi_manager, NULL, manager, NULL);

  for (int i = 0; i < sitFrequency; i++) {
    pthread_create(&customersSitting[i], NULL, customerSits, NULL);
  }

  for (int i = 0; i < leaveFrequency; i++) {
    pthread_create(&customersLeaving[i], NULL, customerLeaves, NULL);
  }

  // Dando join pro programa nao finalizar
  pthread_join(sushi_manager, NULL);
  return 0;
}