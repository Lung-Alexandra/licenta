#include <stdlib.h>
#include <time.h>

#define NUM_OPS 500000
// Lower this to increase the probability of alloc. 0 is the highest probability. 5 is 50%.
#define ALLOC_THRESH 5

void test4() {
//    FILE *file = fopen("size_to_alloc.txt", "w");
//    if (file == NULL) {
//        perror("Failed to open file");
//        return;
//    }
    srand(40);
    char v[10] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};
    char *allocated[NUM_OPS];
    int num_allocated = 0;
    int current_to_free = 0;
    int medium = 0, small = 0, large = 0, extralarge = 0;

    // Randomly choose an action.
    for (int i = 0; i < NUM_OPS; i++) {

        int choice = rand() % 10;

        // Choose to allocate.
        if (choice >= ALLOC_THRESH) {
            int size_to_alloc = 0;
            while (size_to_alloc == 0)
                size_to_alloc = rand() % 300;
            if (size_to_alloc <= 128)
                small++;
            if (size_to_alloc > 128 && size_to_alloc <= 496)
                medium++;
            if (size_to_alloc > 496 && size_to_alloc <= 32000)
                large++;
            if (size_to_alloc > 32000)
                extralarge++;
//            fprintf(file, "a %d %d ", num_allocated, size_to_alloc);
            allocated[num_allocated] = alloc(size_to_alloc);

            for (int j = 0; j < size_to_alloc; j++) {
                allocated[num_allocated][j] = 1;
            }
            num_allocated++;
        }
            // Choose to free.
        else {
            if (current_to_free < num_allocated) {
//                fprintf(file, "f %d ", current_to_free);

                ffree(allocated[current_to_free]);
                current_to_free++;
            }
        }
    }

//    printf("FREEE\n");
    // Cleanup.
    while (current_to_free < num_allocated) {
//        fprintf(file, "f %d ", current_to_free);
        ffree(allocated[current_to_free]);
        current_to_free++;
    }
    float total = (float) (small + medium + large + extralarge);
    printf("Small(%f):%d\n", (float) (small) / total, small);
    printf("Medium(%f):%d\n", (float) (medium) / total, medium);
    printf("Large(%f):%d\n", (float) (large) / total, large);
    printf("Extreme large(%f):%d\n", (float) (extralarge) / total, extralarge);
//    fclose(file);
}

void test4_m() {
    FILE *file = fopen("size_to_alloc.txt", "r");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    srand(40);
    char operation;
    int size_to_alloc;
    char v[10] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};
    char *allocated[NUM_OPS];
    int num_allocated = 0;
    int current_to_free = 0;
    int medium = 0, small = 0, large = 0, extralarge = 0;

    while (fscanf(file, " %c", &operation) != EOF) {
        if (operation == 'a') { // Allocate
            if (fscanf(file, " %d %d", &num_allocated, &size_to_alloc) == 2) {
                if (size_to_alloc <= 128)
                    small++;
                if (size_to_alloc > 128 && size_to_alloc <= 496)
                    medium++;
                if (size_to_alloc > 496 && size_to_alloc <= 32000)
                    large++;
                if (size_to_alloc > 32000)
                    extralarge++;

                allocated[num_allocated] = malloc(size_to_alloc);

                for (int j = 0; j < size_to_alloc; j++) {
                    allocated[num_allocated][j] = 1;
                }
                num_allocated++;
            }
        }
            // Choose to free.
        else {
            if (fscanf(file, " %d", &current_to_free) == 1) {
                if (current_to_free < num_allocated) {
                    free(allocated[current_to_free]);
                    current_to_free++;
                }
            }
        }
    }

    float total = (float) (small + medium + large + extralarge);
    printf("Small(%f):%d\n", (float) (small) / total, small);
    printf("Medium(%f):%d\n", (float) (medium) / total, medium);
    printf("Large(%f):%d\n", (float) (large) / total, large);
    printf("Extreme large(%f):%d\n", (float) (extralarge) / total, extralarge);
}
