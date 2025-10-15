#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("=== Демонстрация зомби-процессов ===\n");
    printf("Родительский процесс PID: %d\n", getpid());
    
    // Создаем дочерний процесс
    pid_t child_pid = fork();
    
    if (child_pid == 0) {
        // Дочерний процесс
        printf("Дочерний процесс PID: %d (родитель: %d)\n", getpid(), getppid());
        printf("Дочерний процесс завершается через 5 секунд...\n");
        sleep(5);
        printf("Дочерний процесс завершен!\n");
        exit(0);
    } else {
        // Родительский процесс
        printf("Родитель создал дочерний процесс с PID: %d\n", child_pid);
        printf("Родитель НЕ вызывает wait() - дочерний процесс станет зомби!\n");
        printf("Родитель спит 30 секунд...\n");
        
        // Родитель НЕ вызывает wait() - создаем зомби
        sleep(30);
        
        printf("Родитель завершается, зомби будет убран\n");
    }
    
    return 0;
}