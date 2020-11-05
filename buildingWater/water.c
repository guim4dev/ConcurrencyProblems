#include <stdio.h>
#include <pthread.h>
#define MOLECULE_CAP 10

int moleculeCounter = 0;
int oxygenSpots = 1;
int hydrogenSpots = 2;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queuesFree = PTHREAD_COND_INITIALIZER;
pthread_cond_t queuesReady = PTHREAD_COND_INITIALIZER;


void *moleculeProducer(void *arg) {
  // Roda até chegar no MOLECULE_CAP
  while(moleculeCounter < MOLECULE_CAP) {
    printf("Produtor de Moléculas tenta pegar o lock...\n");
    pthread_mutex_lock(&mutex);
    while(oxygenSpots != 0 || hydrogenSpots != 0) {
      printf("Produtor esperando pelo sinal de filas prontas para formação de molécula...\n");
      pthread_cond_wait(&queuesReady, &mutex);
    }

    printf("\033[1;32m");
    printf("Formando molécula\n");
    moleculeCounter++;
    printf("\033[0m");
    oxygenSpots = 1;
    hydrogenSpots = 2;
    printf("Produtor sinaliza que as filas estão livres...\n");
    pthread_cond_signal(&queuesFree);
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

void *oxygen(void *arg) {
  printf("Oxigênio tenta pegar o lock.\n");
  pthread_mutex_lock(&mutex);
  printf("Oxigênio verifica se tem vaga para formar molécula.\n");
  while (oxygenSpots == 0) {
    printf("Não há espaço, esperando...\n");
    pthread_cond_wait(&queuesFree, &mutex);
  }
  printf("Espaço vazio para o oxigênio. Ocupando-o...\n");
  oxygenSpots = 0;
  if (hydrogenSpots == 0) {
    printf("Oxigênio avisa que as filas estão prontas\n");
    pthread_cond_signal(&queuesReady);
  }
  pthread_mutex_unlock(&mutex);
  return NULL;
}

void *hydrogen(void *arg) {
  printf("Hidrogênio tenta pegar o lock.\n");
  pthread_mutex_lock(&mutex);
  printf("Hidrogênio verifica se tem vaga para formar molécula.\n");
  while (hydrogenSpots == 0) {
    printf("Não há espaço, esperando...\n");
    pthread_cond_wait(&queuesFree, &mutex);
  }
  printf("Espaço vazio para o hidrogênio. Ocupando-o...\n");
  hydrogenSpots--;
  if (oxygenSpots == 0 && hydrogenSpots == 0) {
    printf("Hidrogênio avisa que as filas estão prontas\n");
    pthread_cond_signal(&queuesReady);
  }
  pthread_mutex_unlock(&mutex);
  return NULL;
}

int main(void) {
  pthread_t oxy[MOLECULE_CAP]; // molecule_cap oxygens
  pthread_t hydro[2*MOLECULE_CAP]; // 2*molecule_cap hydrogens
  pthread_t producer;
  // Criando Thread para produtor de moleculas
  pthread_create(&producer, NULL, moleculeProducer, NULL);

  for (int i = 0; i < 2*MOLECULE_CAP; i++) {
    pthread_create(&hydro[i], NULL, hydrogen, NULL);
    if (i%2 == 0) {
      pthread_create(&oxy[i/2], NULL, oxygen, NULL);
    }
  }

  // Programa só finaliza quando terminar a execução da thread do produtor (nunca)
  pthread_join(producer, NULL);
  return 0;
}