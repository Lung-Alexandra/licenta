#include <stdlib.h>

#define NUM_OPS 500000
// Lower this to increase the probability of alloc. 0 is the highest probability. 5 is 50%.
#define ALLOC_THRESH 5
#define NUM 300
void test4() {

    srand(40);

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
                size_to_alloc = rand() % NUM;
            if (size_to_alloc <= 128)
                small++;
            if (size_to_alloc > 128 && size_to_alloc <= 496)
                medium++;
            if (size_to_alloc > 496 && size_to_alloc <= 32000)
                large++;
            if (size_to_alloc > 32000)
                extralarge++;

            allocated[num_allocated] = alloc(size_to_alloc);

//            printf("%d\n",total_size);
//            printf("%d\n",total_mem);

            if (allocated[num_allocated] != NULL)
                for (int j = 0; j < size_to_alloc; j++) {
                    allocated[num_allocated][j] = 1;
                }
            num_allocated++;
        }
            // Choose to free.
        else {
            if (current_to_free < num_allocated) {
//                printf("%d\n",total_size);
//                printf("%d\n",total_mem);
                ffree(allocated[current_to_free]);
                current_to_free++;
            }
        }
    }
    // Cleanup.
    while (current_to_free < num_allocated) {
//        printf("%d\n",total_size);
//        printf("%d\n",total_mem);
        ffree(allocated[current_to_free]);
        current_to_free++;
    }

    float total = (float) (small + medium + large + extralarge);
    printf("Small(%f):%d\n", (float) (small) / total, small);
    printf("Medium(%f):%d\n", (float) (medium) / total, medium);
    printf("Large(%f):%d\n", (float) (large) / total, large);
    printf("Extreme large(%f):%d\n", (float) (extralarge) / total, extralarge);
    printf("%d\n", total_size);
    printf("%d\n", total_mem);
}

void test4_m() {
    srand(40);

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
                size_to_alloc = rand() % NUM;
            if (size_to_alloc <= 128)
                small++;
            if (size_to_alloc > 128 && size_to_alloc <= 496)
                medium++;
            if (size_to_alloc > 496 && size_to_alloc <= 32000)
                large++;
            if (size_to_alloc > 32000)
                extralarge++;

            allocated[num_allocated] = malloc(size_to_alloc);

            if (allocated[num_allocated] != NULL)
                for (int j = 0; j < size_to_alloc; j++) {
                    allocated[num_allocated][j] = 1;
                }
            num_allocated++;
        }
            // Choose to free.
        else {
            if (current_to_free < num_allocated) {


                free(allocated[current_to_free]);
                current_to_free++;
            }
        }
    }
    // Cleanup.
    while (current_to_free < num_allocated) {

        free(allocated[current_to_free]);
        current_to_free++;
    }
    float total = (float) (small + medium + large + extralarge);
    printf("Small(%f):%d\n", (float) (small) / total, small);
    printf("Medium(%f):%d\n", (float) (medium) / total, medium);
    printf("Large(%f):%d\n", (float) (large) / total, large);
    printf("Extreme large(%f):%d\n", (float) (extralarge) / total, extralarge);

}