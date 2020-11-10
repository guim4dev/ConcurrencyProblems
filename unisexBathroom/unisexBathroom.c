#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Problema de Unisex Bathroom, seção 6.2

int bathroomSpots = 3;
int peopleThatNeedToGoToTheBathroom = 40;
int manSwitch = 0;
int womanSwitch = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t womenLeft = PTHREAD_COND_INITIALIZER;
pthread_cond_t menLeft = PTHREAD_COND_INITIALIZER;

void *bathroom(void *arg) {
  while(peopleThatNeedToGoToTheBathroom > 0) {
    // Ainda há pessoas que necessitam ir ao toalete
  }
  return NULL;
}
void *man(void *arg) {
  int needsToGoToTheBathroom = 1;
  // Enquanto tiver que ir ao banheiro, a pessoa tentará fazê-lo
  while(needsToGoToTheBathroom) {
    pthread_mutex_lock(&mutex);
    // Enquanto houverem mulheres no banheiro, homem tem que esperar que todas saiam
    while(manSwitch == 0 && womanSwitch == 1) {
      printf("Homem esperando que todas as mulheres saiam do banheiro\n");
      pthread_cond_wait(&womenLeft, &mutex);
    }
    manSwitch = 1;
    womanSwitch = 0;
    // Se houver espaço no banheiro, pessoa ocupa
    if (bathroomSpots > 0) {
      printf("Há espaço no banheiro. Homem ocupando-o...\n");
      peopleThatNeedToGoToTheBathroom--;
      bathroomSpots--;
      needsToGoToTheBathroom = 0;
    }
    pthread_mutex_unlock(&mutex);
  }
  sleep(1); // tempo gasto dentro do banheiro

  // Adquire o lock novamente para sair do banheiro
  pthread_mutex_lock(&mutex);
  printf("Homem saindo do banheiro\n");
  bathroomSpots++;
  // Ao sair do banheiro, uma vaga é liberada. Caso o banheiro fique livre, o último homem avisa as mulheres que não há mais homens no banheiro
  if (bathroomSpots == 3) {
    manSwitch = 0;
    womanSwitch = 0;
    pthread_cond_broadcast(&menLeft);
  }
  pthread_mutex_unlock(&mutex);

  return NULL;
}

// A thread das mulheres funciona com o mesmo raciocínio da thread dos homens.

void *woman(void *arg) {
  int needsToGoToTheBathroom = 1;
  while(needsToGoToTheBathroom) {
    pthread_mutex_lock(&mutex);
    while(womanSwitch == 0 && manSwitch == 1) {
      printf("Mulher esperando que todos os homens saiam do banheiro\n");
      pthread_cond_wait(&menLeft, &mutex);
    }
    manSwitch = 0;
    womanSwitch = 1;
    if (bathroomSpots > 0) {
      printf("Há espaço no banheiro. Mulher ocupando-o e fazendo xixi...\n");
      peopleThatNeedToGoToTheBathroom--;
      bathroomSpots--;
      needsToGoToTheBathroom = 0;
    }
    pthread_mutex_unlock(&mutex);
  }
  sleep(1); // tempo dentro do banheiro
  pthread_mutex_lock(&mutex);
  printf("Mulher saindo do banheiro\n");
  bathroomSpots++;
    // Ao sair do banheiro, uma vaga é liberada. Caso o banheiro fique livre, a última mulher avisa aos homens que não há mais mulheres no banheiro
  if (bathroomSpots == 3) {
    manSwitch = 0;
    womanSwitch = 0;
    pthread_cond_broadcast(&womenLeft);
  }
  pthread_mutex_unlock(&mutex);
  return NULL;
}

int main(void) {
  pthread_t men[20];
  pthread_t women[20];
  pthread_t toilet;
  // Criando Thread para o Reitor
  pthread_create(&toilet, NULL, bathroom, NULL);

  for (int i = 0; i < 20; i++) {
    pthread_create(&men[i], NULL, man, NULL);
    pthread_create(&women[i], NULL, woman, NULL);
  }

  // Programa só finaliza quando terminar a execução da thread do banheiro (todo mundo fez xixi)
  pthread_join(toilet, NULL);
  return 0;
}
