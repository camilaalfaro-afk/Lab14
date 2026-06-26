#include <stdio.h>
#include <pthread.h>

long contador = 0; 

void* rutina_hilo(void* arg) {
    for (int i = 0; i < 1000000; i++) {
        contador++;
    }
    return NULL;
}

int main() {
    pthread_t hilo1, hilo2;

    pthread_create(&hilo1, NULL, rutina_hilo, NULL);
    pthread_create(&hilo2, NULL, rutina_hilo, NULL);
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);

    printf("Valor TEÓRICO esperado : 2000000\n");
    printf("Valor REAL obtenido: %ld\n", contador);
    printf("Pérdida de datos: %ld\n", 2000000 - contador);
    return 0;
}
