#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

// Глобальные переменные для обработки сигналов
volatile sig_atomic_t timeout_occurred = 0;
pid_t *child_pids = NULL;
int child_count = 0;

// Обработчик сигнала SIGALRM
void timeout_handler(int sig) {
    timeout_occurred = 1;
    printf("Timeout occurred! Sending SIGKILL to child processes...\n");
    
    // Отправляем SIGKILL только действительно активным дочерним процессам
    for (int i = 0; i < child_count; i++) {
        if (child_pids[i] > 0) {
            printf("Killing child process %d\n", child_pids[i]);
            kill(child_pids[i], SIGKILL);
        }
    }
}

int main(int argc, char **argv) {
    int seed = -1;
    int array_size = -1;
    int pnum = -1;
    int timeout = 0; // 0 означает отсутствие таймаута
    bool with_files = false;

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {
            {"seed", required_argument, 0, 0},
            {"array_size", required_argument, 0, 0},
            {"pnum", required_argument, 0, 0},
            {"timeout", required_argument, 0, 0},
            {"by_files", no_argument, 0, 'f'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "f", options, &option_index);

        if (c == -1) break;

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0:
                        seed = atoi(optarg);
                        if (seed <= 0) {
                            printf("seed must be a positive number\n");
                            return 1;
                        }
                        break;
                    case 1:
                        array_size = atoi(optarg);
                        if (array_size <= 0) {
                            printf("array_size must be a positive number\n");
                            return 1;
                        }
                        break;
                    case 2:
                        pnum = atoi(optarg);
                        if (pnum <= 0) {
                            printf("pnum must be a positive number\n");
                            return 1;
                        }
                        break;
                    case 3:
                        timeout = atoi(optarg);
                        if (timeout <= 0) {
                            printf("timeout must be a positive number\n");
                            return 1;
                        }
                        break;
                    case 4:
                        with_files = true;
                        break;
                    default:
                        printf("Index %d is out of options\n", option_index);
                }
                break;
            case 'f':
                with_files = true;
                break;
            case '?':
                break;
            default:
                printf("getopt returned character code 0%o?\n", c);
        }
    }

    if (optind < argc) {
        printf("Has at least one no option argument\n");
        return 1;
    }

    if (seed == -1 || array_size == -1 || pnum == -1) {
        printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--timeout \"num\"] [--by_files]\n",
               argv[0]);
        return 1;
    }

    // Выделяем память для хранения PID дочерних процессов
    child_pids = malloc(sizeof(pid_t) * pnum);
    child_count = pnum;

    // Устанавливаем обработчик сигнала SIGALRM
    if (timeout > 0) {
        signal(SIGALRM, timeout_handler);
        alarm(timeout); // Устанавливаем таймер
        printf("Timeout set to %d seconds\n", timeout);
    }

    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);
    
    // Создаем пайпы для каждого процесса (если не используем файлы)
    int pipes[pnum][2];
    if (!with_files) {
        for (int i = 0; i < pnum; i++) {
            if (pipe(pipes[i]) == -1) {
                printf("Pipe creation failed!\n");
                return 1;
            }
        }
    }

    int active_child_processes = 0;
    int killed_by_timeout = 0;  // ← НОВАЯ ПЕРЕМЕННАЯ: счетчик убитых по таймауту

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    // Создаем дочерние процессы
    for (int i = 0; i < pnum; i++) {
        pid_t child_pid = fork();
        child_pids[i] = child_pid; // Сохраняем PID дочернего процесса
        if (child_pid >= 0) {
            // successful fork
            active_child_processes += 1;
            if (child_pid == 0) {
                // child process
                
                // Вычисляем границы для текущего процесса
                int segment_size = array_size / pnum;
                int start = i * segment_size;
                int end = (i == pnum - 1) ? array_size : (i + 1) * segment_size;
                
                // Находим min и max в своем сегменте
                struct MinMax local_min_max = GetMinMax(array, start, end);
                if (with_files) {
                    // use files here
                    char filename_min[20], filename_max[20];
                    sprintf(filename_min, "min_%d.txt", i);
                    sprintf(filename_max, "max_%d.txt", i);
                    
                    FILE *file_min = fopen(filename_min, "w");
                    FILE *file_max = fopen(filename_max, "w");
                    
                    if (file_min && file_max) {
                        fprintf(file_min, "%d", local_min_max.min);
                        fprintf(file_max, "%d", local_min_max.max);
                        fclose(file_min);
                        fclose(file_max);
                    }
                } else {
                    // use pipe here
                    close(pipes[i][0]); // закрываем чтение в дочернем процессе
                    
                    write(pipes[i][1], &local_min_max.min, sizeof(int));
                    write(pipes[i][1], &local_min_max.max, sizeof(int));
                    
                    close(pipes[i][1]);
                }
                free(array);
                return 0;
            }

        } else {
            printf("Fork failed!\n");
            return 1;
        }
    }

    // Родительский процесс ждет завершения всех дочерних с неблокирующим wait
    while (active_child_processes > 0) {
        int status;
        pid_t finished_pid = waitpid(-1, &status, WNOHANG);
        
        if (finished_pid > 0) {
            // Дочерний процесс завершился
            active_child_processes -= 1;
            
            // Находим индекс завершенного процесса в массиве child_pids
            for (int i = 0; i < pnum; i++) {
                if (child_pids[i] == finished_pid) {
                    child_pids[i] = 0; // Помечаем как завершенный
                    
                    // ← НОВАЯ ЛОГИКА: отслеживаем как завершился процесс
                    if (WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL) {
                        killed_by_timeout++;  // Увеличиваем счетчик убитых по таймауту
                        printf("Child process %d was terminated by timeout (SIGKILL)\n", finished_pid);
                    } else if (WIFSIGNALED(status)) {
                        printf("Child process %d was terminated by signal %d\n", finished_pid, WTERMSIG(status));
                    }
                    break;
                }
            }
        } else if (finished_pid == 0) {
            // Дочерние процессы еще работают, проверяем таймаут
            if (timeout_occurred) {
                // Таймаут уже обработан в обработчике сигнала
                usleep(100000); // Небольшая задержка перед следующей проверкой
            } else {
                usleep(10000); // Короткая задержка перед следующей проверкой
            }
        } else {
            // Ошибка waitpid
            if (errno != ECHILD) {
                perror("waitpid failed");
            }
            break;
        }
    }

    // Отменяем таймер, если он еще не сработал
    if (timeout > 0 && !timeout_occurred) {
        alarm(0);
    }

    struct MinMax min_max;
    min_max.min = INT_MAX;
    min_max.max = INT_MIN;

    int results_collected = 0;
    
    // Собираем результаты от всех процессов, которые успели завершиться нормально
    for (int i = 0; i < pnum; i++) {
        int min = INT_MAX;
        int max = INT_MIN;
        int result_available = 0;

        if (with_files) {
            // read from files
            char filename_min[20], filename_max[20];
            sprintf(filename_min, "min_%d.txt", i);
            sprintf(filename_max, "max_%d.txt", i);
            
            FILE *file_min = fopen(filename_min, "r");
            FILE *file_max = fopen(filename_max, "r");
            
            if (file_min && file_max) {
                if (fscanf(file_min, "%d", &min) == 1 && fscanf(file_max, "%d", &max) == 1) {
                    result_available = 1;
                    results_collected++;
                }
                fclose(file_min);
                fclose(file_max);
                
                // Удаляем временные файлы
                remove(filename_min);
                remove(filename_max);
            }
        } else {
            // read from pipes
            close(pipes[i][1]); // закрываем запись в родительском процессе
            
            // ← ИСПРАВЛЕННАЯ ЛОГИКА: проверяем, что оба значения прочитаны
            if (read(pipes[i][0], &min, sizeof(int)) == sizeof(int) && 
                read(pipes[i][0], &max, sizeof(int)) == sizeof(int)) {
                result_available = 1;
                results_collected++;
            }
            
            close(pipes[i][0]);
        }

        if (result_available) {
            if (min < min_max.min) min_max.min = min;
            if (max > min_max.max) min_max.max = max;
        }
    }

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    free(array);
    free(child_pids);

    printf("Min: %d\n", min_max.min);
    printf("Max: %d\n", min_max.max);
    printf("Elapsed time: %fms\n", elapsed_time);
    printf("Number of processes: %d\n", pnum);
    printf("Results collected from %d processes\n", results_collected);
    
    // ← ИСПРАВЛЕННАЯ ЛОГИКА: показываем предупреждение только если действительно были убитые процессы
    if (killed_by_timeout > 0) {
        printf("WARNING: %d processes were terminated by timeout!\n", killed_by_timeout);
    } else if (timeout_occurred && results_collected == pnum) {
        printf("NOTE: Timeout occurred but all processes completed normally\n");
    } else if (timeout_occurred) {
        printf("WARNING: Execution was terminated by timeout! (%d processes killed)\n", killed_by_timeout);
    }
    
    fflush(NULL);
    return 0;
}