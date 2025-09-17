# Проверяем, что аргументы есть
if [ $# -eq 0 ]; then
    echo "Usage: $0 number1 number2 ... numberN"
    exit 1
fi

# Инициализируем переменные
count=$#
sum=0

# Считаем сумму всех аргументов
for num in "$@"; do
    sum=$((sum + num))
done

# Вычисляем среднее арифметическое
average=$((sum / count))

# Выводим результаты
echo "Количество чисел: $count"
echo "Сумма чисел: $sum"
echo "Среднее арифметическое: $average"
