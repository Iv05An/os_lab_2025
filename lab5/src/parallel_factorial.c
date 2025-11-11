#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    int start;
    int end;
    long long mod;
    long long partial_result;
} thread_data_t;

long long global_result = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* calculate_partial_factorial(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    data->partial_result = 1;
    
    for (int i = data->start; i <= data->end; i++) {
        data->partial_result = (data->partial_result * i) % data->mod;
    }
    
    pthread_mutex_lock(&mutex);
    global_result = (global_result * data->partial_result) % data->mod;
    pthread_mutex_unlock(&mutex);
    
    return NULL;
}

void parse_arguments(int argc, char* argv[], int* k, int* pnum, long long* mod) {
    *k = 0;
    *pnum = 1;
    *mod = 1000000007;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) {
            *k = atoi(argv[++i]);
        } else if (strncmp(argv[i], "--pnum=", 7) == 0) {
            *pnum = atoi(argv[i] + 7);
        } else if (strncmp(argv[i], "--mod=", 6) == 0) {
            *mod = atoll(argv[i] + 6);
        }
    }
    
    if (*k <= 0) {
        fprintf(stderr, "Error: k must be positive integer\n");
        exit(1);
    }
    if (*pnum <= 0) {
        fprintf(stderr, "Error: pnum must be positive integer\n");
        exit(1);
    }
    if (*mod <= 0) {
        fprintf(stderr, "Error: mod must be positive integer\n");
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    int k, pnum;
    long long mod;
    
    parse_arguments(argc, argv, &k, &pnum, &mod);
    
    printf("Calculating %d! mod %lld using %d threads\n", k, mod, pnum);
    
    if (k == 0 || k == 1) {
        printf("Result: 1\n");
        return 0;
    }
    
    pthread_t* threads = malloc(pnum * sizeof(pthread_t));
    thread_data_t* thread_data = malloc(pnum * sizeof(thread_data_t));
    
    int numbers_per_thread = k / pnum;
    int remainder = k % pnum;
    int current_start = 1;
    
    for (int i = 0; i < pnum; i++) {
        thread_data[i].start = current_start;
        thread_data[i].end = current_start + numbers_per_thread - 1;
        thread_data[i].mod = mod;
        
        if (i < remainder) {
            thread_data[i].end++;
        }
        
        current_start = thread_data[i].end + 1;
        
        if (thread_data[i].start > k) {
            thread_data[i].start = thread_data[i].end = k;
        }
        
        printf("Thread %d: numbers from %d to %d\n", 
               i, thread_data[i].start, thread_data[i].end);
    }
    
    for (int i = 0; i < pnum; i++) {
        if (pthread_create(&threads[i], NULL, 
                          calculate_partial_factorial, &thread_data[i]) != 0) {
            perror("pthread_create");
            exit(1);
        }
    }
    
    for (int i = 0; i < pnum; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            exit(1);
        }
    }
    
    printf("Result: %lld\n", global_result);
    
    free(threads);
    free(thread_data);
    
    return 0;
}