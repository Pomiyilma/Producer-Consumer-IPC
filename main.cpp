#include <stdio.h>  //for printf(), perror()
#include <stdlib.h>  //for exit(), atoi()
#include <unistd.h>   //fork(), usleep()
#include <sys/wait.h>  //to wait for a child process
#include "semaphor.cpp" 
#include "shared_mf.cpp"

int main(int argc, char *argv[]) {  //default vals if no user arguments
    int buffer_size = 5;
    int num_producers = 2;
    int num_consumers = 2;
    int total_items = 40;     //total produced items

    if (argc == 4) {   //allows the running of ./prodcons 1 N 0 on terminal. ANY buffer size + more producer/consumer achieved
        buffer_size = atoi(argv[1]);
        num_producers = atoi(argv[2]);
        num_consumers = atoi(argv[3]);
    }

    printf("=== Producer-Consumer Problem (Semaphores) ===\n");  //readability
    printf("Buffer Size: %d | Producers: %d | Consumers: %d | Total Items: %d\n\n", 
           buffer_size, num_producers, num_consumers, total_items);

    // Initialize shared resources
    initialise_buffer(buffer_size + 2);     //function from shared_mf.cpp, +2 for in & out indices
    initialise_sem(1, buffer_size, 0);      //function from semaphor.cpp, mutex=1, empty=N, full=0

    //to create circular buffer (for no data loss, flexible buffer size), placing 'in' and 'out' at the end of the buffer in shared memory so all processes can modify them
    int *in  = BUFFER + buffer_size;   
    int *out = BUFFER + buffer_size + 1;
    *in = 0;
    *out = 0;

    // Fork Producers
    for(int i = 1; i <= num_producers; i++) {
        if (fork() == 0) {  // if child producer 
            for(int j = 1; j <= (total_items / num_producers) + 2; j++) {
                int item = i * 100 + j;

                down(SECOND);        // wait for empty
                down(FIRST);         // then enter critical sec, mutex

                BUFFER[*in] = item;
                printf("Producer %d produced %d  [pos %d]\n", i, item, *in);
                *in = (*in + 1) % buffer_size;  //circular (%) move to the next position

                up(FIRST);  //leave critical sec
                up(THIRD);   // signal / say full
                usleep(60000);   //small delay to see interleaving
            }
            exit(0); //child producer finishes
        }
    }

    // Fork Consumers
    for(int i = 1; i <= num_consumers; i++) { 
        if (fork() == 0) {  //if child consumer
            for(int j = 1; j <= (total_items / num_consumers) + 2; j++) {
                down(THIRD);         // wait for full
                down(FIRST);         // then enter critical sec, mutex

                int item = BUFFER[*out];
                printf("Consumer %d consumed %d  [pos %d]\n", i, item, *out);
                *out = (*out + 1) % buffer_size;  //circular (%) move to the next position

                up(FIRST);      //leave critical sec
                up(SECOND);      //signal / say empty
                usleep(70000);     //small delay to see interleaving
            }
            exit(0);
        }
    }

    // Parent waits for all children
    for(int i = 0; i < num_producers + num_consumers; i++) {
        wait(NULL);   //until producers and consumers finish
    }

    close_sem();   //releases semaphore resources
    close_buffer();   //releases shared mem

    printf("\n=== All processes completed successfully ===\n");
    return 0;
}