#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Problema do Search-insert-delete, sessão 6.1

pthread_rwlock_t RWLock = PTHREAD_RWLOCK_INITIALIZER;

void *searcher(void *arg) {
  while(1) {
    printf("Searcher tenta pegar o lock\n");
    pthread_rwlock_tryrdlock(&RWLock);
    printf("Faz o search\n");
    pthread_rwlock_unlock(&RWLock);
    sleep(1);
  }
}

void *inserter(void *arg) {
  while(1) {
    printf("Inserter tenta pegar o lock\n");
    pthread_rwlock_trywrlock(&RWLock);
    printf("Faz o insert\n");
    pthread_rwlock_unlock(&RWLock);
    sleep(1);
  }
}

void *deleter(void *arg) {
  while(1) {
    printf("Deleter tenta pegar o lock\n");
    pthread_rwlock_trywrlock(&RWLock);
    printf("Faz o delete\n");
    pthread_rwlock_unlock(&RWLock);
    sleep(1);
  }
}

int main(void) {
  pthread_t searchers[4];
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
