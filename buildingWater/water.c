#include <stdio.h>
#include <pthread.h>

int oxygenWaiting = 0;
int hydrogenWaiting = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t oxyQueueFree = PTHREAD_COND_INITIALIZER;
pthread_cond_t oxyQueueFull = PTHREAD_COND_INITIALIZER;
pthread_cond_t hydroQueueFree = PTHREAD_COND_INITIALIZER;
pthread_cond_t hydroQueueFull = PTHREAD_COND_INITIALIZER;

void *bond() {
  printf("Formando molécula");
  hydrogenWaiting = 0;
  oxygenWaiting = 0;
  pthread_cond_signal(&oxyQueueFree);
  pthread_cond_signal(&hydroQueueFree);
  return NULL;
}

void *oxygen(void *arg) {
  printf("Oxigênio tenta pegar o lock.\n");
  pthread_mutex_lock(&mutex);
  printf("Oxigênio verifica se tem vaga para formar molécula.\n");
  while (oxygenWaiting == 1) {
    printf("Não há espaço, esperando...\n");
    pthread_cond_wait(&oxyQueueFree, &mutex);
  }
  printf("Espaço vazio para o oxigênio. Ocupando-o...\n");
  oxygenWaiting = 1;
  printf("Enviando sinal de fila de oxigênio cheia.\n");
  pthread_cond_signal(&oxyQueueFull);
  if (hydrogenWaiting == 2) {
    bond();
  }
  pthread_mutex_unlock(&mutex);
  return NULL;
}

void *hydrogen(void *arg) {
  printf("Hidrogênio tenta pegar o lock.\n");
  pthread_mutex_lock(&mutex);
  printf("Hidrogênio verifica se tem vaga para formar molécula.\n");
  while (hydrogenWaiting == 2) {
    printf("Não há espaço, esperando...\n");
    pthread_cond_wait(&hydroQueueFree, &mutex);
  }
  printf("Espaço vazio para o hidrogênio. Ocupando-o...\n");
  hydrogenWaiting = hydrogenWaiting + 1;
  if (hydrogenWaiting == 2) {
    printf("Enviando sinal de fila de hidrogênio cheia.\n");
    pthread_cond_signal(&hydroQueueFull);
  }
  if (oxygenWaiting == 1) {
    bond();
  }
  pthread_mutex_unlock(&mutex);
  return NULL;
}

// void *barrier(void *arg) {
//   while(1) {
//     pthread_mutex_lock(&mutex);
//     while()
//     pthread_cond_wait(&oxyQueueFull, &mutex);
//     pthread_cond_wait(&hydroQueueFull, &mutex);
//     printf("Formando molécula, passando pela barreira e limpando fila.\n");
//     hydrogenWaiting = 0;
//     oxygenWaiting = 0;
//     pthread_cond_signal(&oxyQueueFree);
//     pthread_cond_signal(&hydroQueueFree);
//   }
//   pthread_mutex_lock(&mutex);

//   pthread_cond_wait(&oxyQueueFull, &mutex);
//   pthread_cond_wait(&hydroQueueFull, &mutex);
//   printf("Formando molécula, passando pela barreira e limpando fila.\n");
//   hydrogenWaiting = 0;
//   oxygenWaiting = 0;
//   pthread_cond_signal(&oxyQueueFree);
//   pthread_cond_signal(&hydroQueueFree);
//   return NULL;
// }

int main(void) {
  pthread_t oxy[10]; // 10 oxygens
  pthread_t hydro[20]; // 20 hydrogens

  for (int i = 0; i < 20; i++) {
    pthread_create(&hydro[i], NULL, hydrogen, NULL);
    if (i%2 == 0) {
      pthread_create(&oxy[i/2], NULL, hydrogen, NULL);
    }
  }

  for (int i = 0; i < 20; i++) {
    pthread_join(&hydro[i], NULL);
    if (i%2 == 0) {
      pthread_join(&oxy[i/2], NULL);
    }
  }
  return 0;
}