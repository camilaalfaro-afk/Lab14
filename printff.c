#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>

#define ITERACIONES 100000

int transacciones= 0; 
pthread_mutex_t mutex_local = PTHREAD_MUTEX_INITIALIZER; 
FILE *stream_compart;

void* rutina_hilo(void* arg) {
    for (int i = 0; i < ITERACIONES; i++) {
        pthread_mutex_lock(&mutex_local);
        transacciones++; 
        pthread_mutex_unlock(&mutex_local);
        fprintf(stream_compart, "Transaccion exitosa\n");
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t hilos[2];
    struct timeval inicio, fin;

    stream_compart = fopen("registro.log", "w");
    if (!stream_compart) exit(EXIT_FAILURE);

    gettimeofday(&inicio, NULL);

    pid_t pid = fork(); 
    if (pid < 0) exit(EXIT_FAILURE);

    for (int i = 0; i < 2; i++) {
        pthread_create(&hilos[i], NULL, rutina_hilo, NULL);
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(hilos[i], NULL);
    }
    fflush(stream_compart);

    if (pid == 0) {
        printf("Contador RAM--HIJO: %d / %d\n", transacciones, ITERACIONES * 2);
        fclose(stream_compart);
        exit(EXIT_SUCCESS);
    } else {
        wait(NULL);
        gettimeofday(&fin, NULL);

        printf("Contador RAM--PADRE: %d / %d\n", transacciones, ITERACIONES * 2);
        fclose(stream_compart);

        long tiempo = (fin.tv_sec - inicio.tv_sec) * 1000 + (fin.tv_usec - inicio.tv_usec) / 1000;
        printf("Tiempo total transcurrido : %ld ms\n", tiempo);
    }
    return 0;
}
