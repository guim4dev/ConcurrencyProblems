#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#define MOLECULE_CAP 100

// Problema de Construção de H2O, sessão 5.6 

int moleculeCounter = 0;
int oxygenSpots = 1;
int hydrogenSpots = 2;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queuesFree = PTHREAD_COND_INITIALIZER;
pthread_cond_t queuesReady = PTHREAD_COND_INITIALIZER;


void *moleculeProducer(void *arg) {
  // Roda até chegar no MOLECULE_CAP
  while(moleculeCounter < MOLECULE_CAP) {
    pthread_mutex_lock(&mutex);
    // Enquanto ainde houver vaga para oxigenio ou hidrogenio, o produtor espera o sinal de filas prontas para formação de molécula
    while(oxygenSpots != 0 || hydrogenSpots != 0) {
      printf("Produtor esperando aviso de que as filas estão prontas para formar molécula\n");
      pthread_cond_wait(&queuesReady, &mutex);
    }
    printf("Formando molécula\n");
    // Após formar a molécula, aumentamos o contador de moléculas e resetamos as vagas
    moleculeCounter++;
    oxygenSpots = 1;
    hydrogenSpots = 2;
    // Produtor sinaliza que as filas estão livres para as moléculas esperando
    pthread_cond_broadcast(&queuesFree);
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

void *oxygen(void *arg) {
  pthread_mutex_lock(&mutex);
  // Enquanto não houver vaga para formar molécula, o oxigênio espera a liberação de vaga
  while (oxygenSpots == 0) {
    printf("Sem vaga para o oxigênio\n");
    pthread_cond_wait(&queuesFree, &mutex);
  }
  printf("Espaço vazio para o oxigênio. Ocupando-o...\n");
  oxygenSpots = 0;
  // Caso as vagas de hidrogênio já estejam ocupadas também, o oxigênio avisa que as filas estão prontas
  if (hydrogenSpots == 0) {
    pthread_cond_signal(&queuesReady);
  }
  pthread_mutex_unlock(&mutex);
  return NULL;
}

void *hydrogen(void *arg) {
  pthread_mutex_lock(&mutex);
  // Enquanto não houver vaga para formar molécula, o hidrogênio espera a liberação de vaga
  while (hydrogenSpots == 0) {
    printf("Sem vaga para o hidrogênio\n");
    pthread_cond_wait(&queuesFree, &mutex);
  }
  printf("Espaço vazio para o hidrogênio. Ocupando-o...\n");
  hydrogenSpots--;
  // Caso as vagas de hidrogênio e as de oxigênio já estejam ocupadas totalmente, o hidrogênio avisa que as filas estão prontas
  if (oxygenSpots == 0 && hydrogenSpots == 0) {
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

  // Programa só finaliza quando terminar a execução da thread do produtor (quando esgotam-se as moléculas)
  pthread_join(producer, NULL);
  return 0;
}