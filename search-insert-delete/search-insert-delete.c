#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Problema do Search-insert-delete, sessão 6.1

pthread_rwlock_t RWLock = PTHREAD_RWLOCK_INITIALIZER;

void *searcher(void *arg) {
  while(1) {
    printf("Searcher tenta pegar o lock\n");
    // tryrd lock permite pegar lock enquanto há outra thread fazendo somente leitura
    pthread_rwlock_tryrdlock(&RWLock);
    printf("Faz o search\n");
    pthread_rwlock_unlock(&RWLock);
    printf("Searcher solta o lock\n");
  }
}

void *inserter(void *arg) {
  while(1) {
    printf("Inserter tenta pegar o lock\n");
    // trywr lock blockeia qualquer outro tipo de lock e só é permitido quando ninguém possui qualquer lock (leitura ou escrita) 
    pthread_rwlock_trywrlock(&RWLock);
    printf("Faz o insert\n");
    pthread_rwlock_unlock(&RWLock);
    printf("Inserter solta o lock\n");
  }
}

void *deleter(void *arg) {
  while(1) {
    printf("Deleter tenta pegar o lock\n");
    // trywr lock blockeia qualquer outro tipo de lock e só é permitido quando ninguém possui qualquer lock (leitura ou escrita) 
    pthread_rwlock_trywrlock(&RWLock);
    printf("Faz o delete\n");
    pthread_rwlock_unlock(&RWLock);
    printf("Deleter solta o lock\n");
  }
}

int main(void) {
  pthread_t searchers[6];
  pthread_t inserters[2];
  pthread_t deleters[2];

  for (int i = 0; i < 4; i++) {
    pthread_create(&searchers[i], NULL, searcher, NULL);
    if (i%2 == 0) {
      pthread_create(&inserters[i/2], NULL, inserter, NULL);
      pthread_create(&deleters[i/2], NULL, deleter, NULL);
    }
  }

  // Dando join pro programa nao finalizar
  pthread_join(searchers[0], NULL);
  return 0;
}
