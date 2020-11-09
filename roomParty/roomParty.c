#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Problema de Room Party, seção 7.3

int studentsInRoom = 0;
int deanInRoom = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t deanLeft = PTHREAD_COND_INITIALIZER;
pthread_cond_t studentsLeft = PTHREAD_COND_INITIALIZER;

void *dean(void *arg) {
  while(1) {
    // dean tentando entrar na Room
    while(deanInRoom == 0) {
      pthread_mutex_lock(&mutex);
      if (studentsInRoom == 0 || studentsInRoom > 50) {
        deanInRoom = 1;
        printf("Dean entrou na sala pois há %d alunos na sala\n", studentsInRoom);
      }
      pthread_mutex_unlock(&mutex);
    }

    // dean expulsando galera (se necessário) e saindo da room
    pthread_mutex_lock(&mutex);
    while(studentsInRoom > 0) {
      pthread_cond_wait(&studentsLeft, &mutex);
    }
    printf("Dean saindo da sala\n");
    deanInRoom = 0;
    pthread_cond_signal(&deanLeft);
    pthread_mutex_unlock(&mutex);
  }
}

void *student(void *arg) {
  int insideRoom = 0;
  // alunos ficam tentando entrar e sair da sala sempre
  while(1) {
    pthread_mutex_lock(&mutex);
    while (insideRoom == 0 && deanInRoom) {
      printf("Aluno esperando dean sair para entrar na sala\n");
      pthread_cond_wait(&deanLeft, &mutex);
    }
    printf("Aluno entrando na sala\n");
    insideRoom = 1;
    studentsInRoom++;
    pthread_mutex_unlock(&mutex);

    sleep(1); // Aluno fica algum tempo na sala

    pthread_mutex_lock(&mutex);
    printf("Aluno saindo da sala\n");
    insideRoom = 0;
    studentsInRoom--;
    if (studentsInRoom == 0) {
      pthread_cond_signal(&studentsLeft);
    }
    pthread_mutex_unlock(&mutex);

    sleep(1); // Aluno espera um pouco para entrar na sala novamente
  }
}


int main(void) {
  pthread_t students[200];
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