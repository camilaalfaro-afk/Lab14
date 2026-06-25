#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/wait.h>

#define SEM_NAME "/semaforo"

int transacciones = 0;
pthread_mutex_t mutex_local =PTHREAD_MUTEX_INITIALIZER;
sem_t *semaforo;
int fd_compart;

void *procesar_transaccion(void *arg){
    pthread_mutex_lock(&mutex_local);
    transacciones++;
    pthread_mutex_unlock(&mutex_local);
    sem_wait(semaforo);
    write(fd_compart,"Transaccion hecha\n",18);
    sem_post(semaforo);
    pthread_exit(NULL);

}

int main(){
    pthread_t hilos[2];
    sem_unlink(SEM_NAME);
    fd_compart = open("registro.log", O_WRONLY|O_CREAT|O_TRUNC, 0644);
    if (fd_compart < 0){perror("Error al abrirrr"); exit(EXIT_FAILURE);}
    semaforo = sem_open(SEM_NAME,O_CREAT,0644,1);
    if(semaforo==SEM_FAILED){perror("Error en el semaforo");close(fd_compart);exit(EXIT_FAILURE);}
        printf("Inicializandoooo...\n");
    pid_t pid = fork();
    if(pid <0){
        perror("Error en fork()");
        sem_close(semaforo);
        sem_unlink(SEM_NAME);
        close(fd_compart);
        exit(EXIT_FAILURE);
    }
    if(pid==0){printf("Surcursal B inciado de forma idependiente -- HIJO: (PID) %d\n",getpid());}
    else{printf("Sucursal A en nodo central--PADRE: (PID) %d\n",getpid());}
    for (int i = 0; i < 2; i++) {
        if (pthread_create(&hilos[i], NULL, procesar_transaccion, NULL) != 0) {
            perror("Error al instanciar pthread");
            exit(EXIT_FAILURE);
        }}
    for (int i = 0; i < 2; i++) {
        pthread_join(hilos[i], NULL);
    }
    if (pid == 0) {
        printf("Transacciones locales exitosas en Memoria --HIJO: %d\n", transacciones);
        sem_close(semaforo);
        close(fd_compart);
        exit(EXIT_SUCCESS); 
    }
    else {
        wait(NULL);
        printf("Transacciones locales exitosas en Memoria --PADRE: %d\n", transacciones);
        sem_close(semaforo);
        sem_unlink(SEM_NAME);
        close(fd_compart);
    
        printf("Auditoría finalizada...PADRE\n"); }
    return 0;
}
