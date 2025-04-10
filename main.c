//this code is AI generated as it is just testing code

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "sig_semiphor.h"

#define NUM_THREADS 8
#define NUM_NODES_PER_THREAD 10000

typedef struct Node {
    struct Node* next;
    int data;
} Node;

Node* head = NULL;

void* push_worker(void* arg) {
    intptr_t tid = (intptr_t)arg;

    for (int i = 0; i < NUM_NODES_PER_THREAD; i++) {
        Node* node = malloc(sizeof(Node));
        if (!node) {
            perror("malloc");
            exit(1);
        }
        node->data = (int)tid * 1000000 + i;

        sem_down();              // acquire lock
        node->next = head;
        head = node;
        sem_up();                // release lock
    }

    return NULL;
}

int main() {
    // Initialize your signal semaphore
    sem_init(1);  // Start unlocked

    // Launch workers
    pthread_t threads[NUM_THREADS];
    for (intptr_t i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, push_worker, (void*)i);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Pop and free the list under lock
    int count = 0;
    for (;;) {
        sem_down();
        Node* node = head;
        if (!node) {
            sem_up();
            break;
        }
        head = node->next;
        sem_up();

        free(node);
        count++;
    }

    printf("Freed %d nodes\n", count);
    assert(count == NUM_THREADS * NUM_NODES_PER_THREAD);

    printf("Test passed — no corruption, no segfaults!\n");
    return 0;
}
