#include <stdio.h>
#include <pthread.h>

int servings = 3; // começamos com 3 servings dentro do pot

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t emptyPot = PTHREAD_COND_INITIALIZER;
pthread_cond_t fullPot = PTHREAD_COND_INITIALIZER;

void *savage() {
  // Savage come pra sempre
  while(1) {
    pthread_mutex_lock(&mutex);
    while (servings == 0) {
      printf("Savage não encontrou comida, avisando o cozinheiro...\n");
      pthread_cond_signal(&emptyPot);
      printf("Esperando que o pot fique cheio...\n");
      pthread_cond_wait(&fullPot, &mutex);
    }
    printf("Savage pega 1 serving.\n");
    servings = servings - 1;
    pthread_mutex_unlock(&mutex);
  }
}

void *cook() {
  // Cook fica sempre esperando para encher o pot
  while (1) {
    pthread_mutex_lock(&mutex);
    while (servings > 0) {
      printf("Cook esperando alguém avisá-lo que o pot está vazio...\n");
      pthread_cond_wait(&emptyPot, &mutex);
    }
    printf("Cook enchendo o pot...\n");
    servings = 3;
    pthread_cond_signal(&fullPot);
    pthread_mutex_unlock(&mutex);
  }
}

int main() {
  pthread_t cookT; // 1 cook
  pthread_create(&cookT, NULL, cook, NULL);
  pthread_t savageTs[5]; // 5 savages
  for (int i = 0; i < 5; i++) {
    pthread_create(&savageTs[i], NULL, savage, NULL);
  }
  pthread_join(cookT, NULL);
  return 0;
}