#include <stdlib.h>

#define NUM_OPS 500000
// Lower this to increase the probability of alloc. 0 is the highest probability. 5 is 50%.
#define ALLOC_THRESH 5
void test(){
    srand(20);
    int* allocated[NUM_OPS];
    int num_allocated = 0;
    int current_to_free = 0;

    // Randomly choose an action.
    for(int i = 0;i<NUM_OPS;i++){
        int choice = rand()%10;

        // Choose to allocate.
        if(choice >= ALLOC_THRESH){
            allocated[num_allocated] = alloc(rand()%400);
            num_allocated++;
        }
            // Choose to free.
        else{
            if(current_to_free < num_allocated){
                ffree(allocated[current_to_free]);
                current_to_free++;
            }
        }
    }

    // Cleanup.
    while(current_to_free < num_allocated){
        ffree(allocated[current_to_free]);
        current_to_free++;
    }
}