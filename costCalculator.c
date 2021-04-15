
#include "queue.h"
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_CONSUMERS 1

/**
 * PROGRAM VARIABLES
 */
pthread_mutex_t ring;
pthread_mutex_t des;
pthread_cond_t full;
pthread_cond_t empty;
struct queue *cQueue;
const char *cFile;
FILE *descriptorP;
int total = 0;

/**
 * Open file function.
 * @param arg
 * @return
 */
FILE *openFile(const char *__restrict __filename) {
    FILE *f = fopen(__filename, "r");
    if (f == NULL) {
        perror("Could not open file.");
        exit(-1);
    }
    return f;
}

void lockMutex(pthread_mutex_t * t) {
    if (pthread_mutex_lock(t) < 0) {
        perror("Mutex error.");
        exit(-1);
    }
}
void unlockMutex(pthread_mutex_t * t) {
    if (pthread_mutex_unlock(t) < 0) {
        perror("Mutex error.");
        exit(-1);
    }
}

/**
 * Produce a thread, with it's ID and operation count.
 * @param arg
 * @return
 */
void *produce(void *arg) {
    struct param *p = arg;

    /**
     * Get first index and lock it.
     */
    lockMutex(&des);

    descriptorP = openFile(cFile);
    int counter = 0;
    char chr;
    while (counter < p->id) {
        chr = fgetc(descriptorP);
        if (chr == '\n') {
            counter++;
        }
    }
    /**
     * Store current position.
     */
    FILE *current = descriptorP;

    unlockMutex(&des);

    int i, i1, i2 = 0;
    for (int j = p->op; j > 0; j--) {

        /**
         * Extract buffer values
         */
        lockMutex(&des);

        descriptorP = current;
        if (fscanf(descriptorP, "%d %d %d", &i, &i1, &i2) < 0) {
            perror("Error while extracting data.");
            exit(-1);
        }
        current = descriptorP;


        struct element temporal = {i1, i2};
        unlockMutex(&des);
        lockMutex(&ring);
        while (queue_full(cQueue))
            if (pthread_cond_wait(&full, &ring) < 0) {
                perror("Condition error.");
                exit(-1);
            }


        if (queue_put(cQueue, &temporal) < 0) {
            perror("Insert error.");
            exit(-1);
        }
        if (pthread_cond_signal(&empty) < 0) {
            perror("Condition error.");
            exit(-1);
        }
        unlockMutex(&ring);
    }
    pthread_exit(0);
}

/**
 * Consume threads.
 * @param numValores
 * @return
 */
void *consume(int *numValores) {
    struct element data;
    /**
     * Read for operations.
     */
    for (int k = 0; k < *numValores; k++) {


        lockMutex(&ring);

        while (queue_empty(cQueue)) {
            if (pthread_cond_wait(&empty, &ring) < 0) {
                perror("Condition error.\n");
                exit(-1);
            }
        }

        struct element *data = queue_get(cQueue);
        if (data == NULL) {
            perror("Extraction error.\n");
            exit(-1);
        }

        switch (data->type) {
            case 1:
                total += 1 * data->time;

                break;
            case 2:
                total += 3 * data->time;

                break;
            case 3:
                total += 10 * data->time;

                break;
            default:
                perror("Invalid value on type.\n");
        }
        if (pthread_cond_signal(&full) < 0) {
            perror("Condition error.\n");
            exit(-1);
        }
        unlockMutex(&ring);
    }
    pthread_exit(0);
}

int calculateLines(const char filename[]) {
    FILE *file = openFile(filename);
    char chr;
    int lines = 0;
    while (!feof(file)) {
        chr = fgetc(file);
        if (chr == '\n') {
            lines++;
        }
    }
    fclose(file);
    return lines;
}

/**
 * Initialization of the code
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char *argv[]) {
    /**
     * Check input arguments.
     */
    if (argc > 4) {
        perror("Invalid args number.\n");
        return -1;
    }

    FILE *descriptor = openFile(argv[1]);

    int numVal;
    if (fscanf(descriptor, "%d", &numVal) < 0) {
        perror("Error extracting file data.\n");
        exit(-1);
    }
    int numLin = calculateLines(argv[1]);
    if (numVal > (numLin - 1)) {
        perror("Wrong operations number.\n");
        return -1;
    }

    int productores = atoi(argv[2]);
    if (productores <= 0) {
        perror("Invalid productors count.\n");
        return -1;
    }
    int size = atoi(argv[3]);
    if (size <= 0) {
        perror("Invalid size.\n");
        return -1;
    }

    cQueue = queue_init(size);
    if (pthread_mutex_init(&ring, NULL) < 0 || pthread_mutex_init(&des, NULL) < 0) {
        perror("Error initializing condition variable.\n");
        exit(-1);
    }
    if (pthread_cond_init(&full, NULL) < 0 || pthread_cond_init(&empty, NULL) < 0) {
        perror("Error while initializing mutex.\n");
        exit(-1);
    }
    //Creamos los hilos y establecemos el nº de operaciones que hará cada uno
    int operaciones = floor((numVal / productores));
    int idcio = 1;
    pthread_t hilosP[productores];
    pthread_t hiloC;
    if (pthread_create(&hiloC, NULL, (void *) consume, &numVal) < 0) {
        perror("Error creating thread.");
        exit(-1);
    }
    //Ejecución de las operaciones
    int i;
    cFile = malloc(sizeof(char[strlen(argv[1])]));
    cFile = argv[1];
    struct param args[productores];
    for (i = 0; i < (productores - 1); i++) {
        args[i].op = operaciones;
        args[i].id = idcio;

        if (pthread_create(&hilosP[i], NULL, (void *) produce, &args[i]) < 0) {
            perror("Error creating thread.\n");
            exit(-1);
        }

        idcio += operaciones;
    }
    int op_ultimo = numVal - (i * operaciones);
    args[productores - 1].op = op_ultimo;
    args[productores - 1].id = idcio;

    //Control de errores de operaciones y valores finales
    if (pthread_create(&hilosP[productores - 1], NULL, (void *) produce, &args[productores - 1]) < 0) {
        perror("Error creating thread.\n");
        exit(-1);
    }

    for (int i = 0; i < productores; i++) {
        if (pthread_join(hilosP[i], NULL) < 0) {
            perror("Error while waiting for thread.\n");
            exit(-1);
        }
    }
    if (pthread_join(hiloC, NULL) < 0) {
        perror("Error joining thread.\n");
        exit(-1);
    }

    printf("Total: %i €.\n", total);
    queue_destroy(cQueue);
    if (pthread_mutex_destroy(&des) < 0 || pthread_mutex_destroy(&ring) < 0) {
        perror("Could not destroy mutex.\n");
        exit(-1);
    }
    if (pthread_cond_destroy(&full) < 0 || pthread_cond_destroy(&empty) < 0) {
        perror("Could not destroy condition variable.\n");
        exit(-1);
    }
    if (fclose(descriptorP) < 0 || fclose(descriptor) < 0) {
        perror("Error while closing descriptor.\n");
        exit(-1);
    }
    return 0;
}
