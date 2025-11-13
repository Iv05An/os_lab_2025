#ifndef FACTORIAL_LIB_H
#define FACTORIAL_LIB_H

#include <stdbool.h>
#include <stdint.h>

// Структура для передачи аргументов факториала
struct FactorialArgs {
    uint64_t begin;
    uint64_t end;
    uint64_t mod;
};

// Умножение по модулю
uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod);

// Вычисление факториала в диапазоне
uint64_t Factorial(const struct FactorialArgs *args);

// Функция для потока (для сервера)
void *ThreadFactorial(void *args);

// Конвертация строки в uint64_t
bool ConvertStringToUI64(const char *str, uint64_t *val);

#endif // FACTORIAL_LIB_H