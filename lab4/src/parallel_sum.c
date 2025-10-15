#define _POSIX_C_SOURCE 199309L
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <pthread.h>

#include "sum.h"
#include "utils.h"

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  uint32_t threads_num = 0;
  uint32_t array_size = 0;
  uint32_t seed = 0;

  // Парсинг аргументов командной строки
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--threads_num") == 0 && i + 1 < argc) {
      threads_num = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--array_size") == 0 && i + 1 < argc) {
      array_size = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
      seed = atoi(argv[++i]);
    }
  }

  // Проверка корректности аргументов
  if (threads_num == 0 || array_size == 0) {
    printf("Usage: %s --threads_num <num> --array_size <size> --seed <seed>\n", argv[0]);
    return 1;
  }

  // Генерация массива
  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);

  // Создание потоков
  pthread_t threads[threads_num];
  struct SumArgs args[threads_num];
  
  int segment_size = array_size / threads_num;
  
  for (uint32_t i = 0; i < threads_num; i++) {
    args[i].array = array;
    args[i].begin = i * segment_size;
    args[i].end = (i == threads_num - 1) ? array_size : (i + 1) * segment_size;
    
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      free(array);
      return 1;
    }
  }

  // Замер времени начала вычислений
  struct timespec start, finish;
  clock_gettime(CLOCK_MONOTONIC, &start);

  // Сбор результатов
  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }

  // Замер времени окончания вычислений
  clock_gettime(CLOCK_MONOTONIC, &finish);

  // Вычисление времени выполнения
  double elapsed = (finish.tv_sec - start.tv_sec);
  elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

  // Вывод результатов
  printf("Total: %d\n", total_sum);
  printf("Elapsed time: %.6f seconds\n", elapsed);

  free(array);
  return 0;
}