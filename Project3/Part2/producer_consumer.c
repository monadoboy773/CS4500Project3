#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 12
#define FILE_NAME "message.txt"

// circular queue
char buffer[BUFFER_SIZE];
int fill = 0;
int use = 0;
int count = 0;

// condition variables to sync threads and a mutex
pthread_cond_t empty, fill_c;
pthread_mutex_t mutex;

// function declarations
void put(char val);
char get();

// Producer thread
void* producer(void* arg) {
    FILE *file = fopen(FILE_NAME, "r");
    char c;

    // open file
    if(file == NULL) {
        printf("ERROR: message.txt is empty\n");
		return 0;
    }

    // read from file and put into buffer
    while ((c = fgetc(file)) != EOF) {
        pthread_mutex_lock(&mutex);
        //wait if buffer is full
        while(count == BUFFER_SIZE){
            printf("Buffer is full. producer waits.\n");
            pthread_cond_wait(&empty, &mutex);
        }
        // put the character into buffer
        put(c);
        // signal that buffer is not empty
        pthread_cond_signal(&fill_c);
        pthread_mutex_unlock(&mutex);
    }

    // close file
    fclose(file);
    return NULL;
}

// Consumer thread
void* consumer(void* arg) {
    while(1) {
        pthread_mutex_lock(&mutex);
        // wait if buffer is empty
        while(count == 0){
            printf("Buffer is empty. Consumer waits.\n");
            pthread_cond_wait(&fill_c, &mutex);
        }
        // get a character from buffer
        char value = get();
        // signal that buffer is not full
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
        // print character
        printf("%c", value);
        fflush(stdout);
    }
    return NULL;
}

// Function for putting a char into the buffer
void put(char val) {
    buffer[fill] = val;
    fill = (fill + 1) % BUFFER_SIZE;
    count++;
}

// Function to get char from buffer
char get() {
    char tmp = buffer[use];
    use = (use + 1) % BUFFER_SIZE;
    count--;
    return tmp;
}

int main() {

    // pthreads for producer and consumer
    pthread_t producer_thread, consumer_thread;
    pthread_cond_init(&empty, NULL);
    pthread_cond_init(&fill_c, NULL);
    pthread_mutex_init(&mutex, NULL);

    // create producer and consumer threads
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    // let threads finish
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    // destroy condition variables
    pthread_cond_destroy(&empty);
    pthread_cond_destroy(&fill_c);
    pthread_mutex_destroy(&mutex);

    return 0;
}