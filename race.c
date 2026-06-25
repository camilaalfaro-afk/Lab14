#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/wait.h>

#define ITERACIONES 100000

int transacciones_locales = 0;
int fd_compartido;

void* procesar_transaccion(void* arg) {
    char *mensaje = "Transaccion exitosa\n";
    int tamano_mensaje = sizeof("Transaccion exitosa\n") - 1; 

    for (int i = 0; i < ITERACIONES; i++) {
        transacciones_locales++; 
        write(fd_compartido, mensaje, tamano_mensaje); 
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t hilos[2];

    fd_compartido = open("registro.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_compartido < 0) exit(EXIT_FAILURE);
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    for (int i = 0; i < 2; i++) {
        pthread_create(&hilos[i], NULL, procesar_transaccion, NULL);
    }
    for (int i = 0; i < 2; i++) {
        pthread_join(hilos[i], NULL);
    }
    if (pid == 0) {
        printf("Esperado en memoria--Hijo: %d | Obtenido real: %d\n", 
               ITERACIONES * 2, transacciones_locales);
        close(fd_compartido);
        exit(EXIT_SUCCESS);
    } else {
        wait(NULL);
        printf("Esperado en memoria--Padre: %d | Obtenido real: %d\n", 
               ITERACIONES * 2, transacciones_locales);
        close(fd_compartido);
    }

    return 0;
}
