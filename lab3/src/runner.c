#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <seed> <array_size>\n", argv[0]);
        return 1;
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        printf("Дочерний процесс: запуск sequential_min_max\n");
        char *args[] = {
            "./sequential_min_max",  // имя программы
            argv[1],                 // seed
            argv[2],                 // array_size
            NULL                     // обязательный NULL в конце
        };
        // Заменяем текущий процесс на sequential_min_max
        execvp(args[0], args);
        perror("execvp failed");
        exit(1);
    } else {
        printf("Родительский процесс: ждет завершения дочернего%d\n", pid);
        int status;
        
        waitpid(pid, &status, 0);  // Ждет завершения
        
        if (WIFEXITED(status)) {
            printf("Дочерний процесс завершился с кодом: %d\n", WEXITSTATUS(status));
        } else {
            printf("Дочерний процесс завершился аварийно\n");
        }
    }
    
    return 0;
}