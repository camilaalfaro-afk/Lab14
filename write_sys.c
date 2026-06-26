#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>

#define ITERACIONES 100000

int transacciones = 0; 
pthread_mutex_t mutex_local = PTHREAD_MUTEX_INITIALIZER; 
int fd_compart;

void* rutina_hilo(void* arg) {
    char *mensaje = "Transaccion exitosa\n";
    int tamano_mensaje = sizeof("Transaccion exitosa\n") - 1; 

    for (int i = 0; i < ITERACIONES; i++) {
        pthread_mutex_lock(&mutex_local);
        transacciones++; 
        pthread_mutex_unlock(&mutex_local);
        write(fd_compart, mensaje, tamano_mensaje); 
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t hilos[2];
    struct timeval inicio, fin;

    fd_compart = open("registro.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_compart < 0) exit(EXIT_FAILURE);

    gettimeofday(&inicio, NULL);

    pid_t pid = fork(); 
    if (pid < 0) exit(EXIT_FAILURE);

    for (int i = 0; i < 2; i++) {
        pthread_create(&hilos[i], NULL, rutina_hilo, NULL);
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(hilos[i], NULL);
    }

    if (pid == 0) {
        printf("Contador RAM--HIJO: %d / %d\n", transacciones, ITERACIONES * 2);
        close(fd_compart);
        exit(EXIT_SUCCESS);
    } else {
        wait(NULL); 
        gettimeofday(&fin, NULL);

        printf("Contador RAM--PADRE: %d / %d\n", transacciones, ITERACIONES * 2);
        close(fd_compart);

        long tiempo = (fin.tv_sec - inicio.tv_sec) * 1000 + (fin.tv_usec - inicio.tv_usec) / 1000;
        printf("Tiempo total transcurrido : %ld ms\n", tiempo);
    }
    return 0;
}
