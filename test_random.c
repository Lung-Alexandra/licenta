#include <stdlib.h>

#define NUM_OPS 500000
// Lower this to increase the probability of alloc. 0 is the highest probability. 5 is 50%.
#define ALLOC_THRESH 5

void test() {
    srand(20);
    int *allocated[NUM_OPS];
    int num_allocated = 0;
    int current_to_free = 0;

    // Randomly choose an action.
    for (int i = 0; i < NUM_OPS; i++) {
        int choice = rand() % 10;

        // Choose to allocate.
        if (choice >= ALLOC_THRESH) {
            allocated[num_allocated] = alloc(rand() % 400);
            num_allocated++;
        }
            // Choose to free.
        else {
            if (current_to_free < num_allocated) {
                ffree(allocated[current_to_free]);
                current_to_free++;
            }
        }
    }

    // Cleanup.
    while (current_to_free < num_allocated) {
        ffree(allocated[current_to_free]);
        current_to_free++;
    }
}

void test2() {
    srand(20);
    char *allocated[NUM_OPS];
    int num_allocated = 0;
    int current_to_free = 0;

    // Randomly choose an action.
    for (int i = 0; i < NUM_OPS; i++) {
        int choice = rand() % 10;

        // Choose to allocate.
        if (choice >= ALLOC_THRESH) {
            int size_to_alloc = rand() % 496;
            allocated[num_allocated] = alloc(size_to_alloc);
            for (int i = 0; i < size_to_alloc; i++) {
                *allocated[num_allocated] = rand();
            }
            num_allocated++;
        }
            // Choose to free.
        else {
            if (current_to_free < num_allocated) {
                ffree(allocated[current_to_free]);
                current_to_free++;
            }
        }
    }

    // Cleanup.
    while (current_to_free < num_allocated) {
        ffree(allocated[current_to_free]);
        current_to_free++;
    }
}

void test3() {
    srand(20);
    char v[10] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};
    char *allocated[NUM_OPS];
    int num_allocated = 0;
    int current_to_free = 0;

    // Randomly choose an action.
    for (int i = 0; i < NUM_OPS; i++) {
        int choice = rand() % 10;

        // Choose to allocate.
        if (choice >= ALLOC_THRESH) {
            int size_to_alloc = rand() % 500;

            allocated[num_allocated] = alloc(size_to_alloc);

            for (int j = 0; j < size_to_alloc; j++) {
                allocated[num_allocated][j] = v[rand() % 10];
            }
            num_allocated++;
        }
            // Choose to free.
        else {
            if (current_to_free < num_allocated) {
                ffree(allocated[current_to_free]);
                current_to_free++;
            }
        }
    }
    printf("FREEE");
    // Cleanup.
    while (current_to_free < num_allocated) {
        ffree(allocated[current_to_free]);
        current_to_free++;
    }
}