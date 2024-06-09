#include <iostream> /* Подключение библиотеки для работы с вводом/выводом. */
#include <fstream> /* Подключение библиотеки для работы с файлами. */

const double pi = 3.14159265358979323846; /* Определение константы pi. */
const double eps = 1e-15; /* Определение точности вычислений. */

/* Функция для вычисления модуля числа. */
double dmAbs(double x) {
    return (x < 0) ? -x : x; /* Возвращает отрицание числа, если оно отрицательное, иначе само число. */
}

/* Функция для вычисления остатка от деления. */
double dmMod(double x, double y) {
    return x - y * int(x / y); /* Вычисление остатка от деления x на y. */
}

/* Функция для вычисления синуса угла в радианах с использованием ряда Тейлора. */
double dmSin(double x) {
    int sign = (x < 0) ? -1 : 1; /* Определение знака угла. */
    x = dmMod(dmAbs(x), 2 * pi); /* Приведение угла к интервалу [0, 2*pi) для обеспечения периодичности синуса. */

    if (x > pi) {
        x -= pi; /* Если угол больше pi, уменьшаем его на pi. */
        sign *= -1; /* Меняем знак угла. */
    }

    double sum = x, term = x; /* Инициализация суммы и текущего члена ряда. */
    for (int n = 3; dmAbs(term) > eps; n += 2) {
        term = -term * x * x / n / (n - 1); /* Вычисление следующего члена ряда Тейлора. */
        sum += term; /* Обновление суммы ряда. */
    }

    return sum * sign; /* Возвращение итоговой суммы с учетом знака. */
}

/* Функция для вычисления косинуса угла в радианах с использованием ряда Тейлора. */
double dmCos(double x) {
    x = dmMod(dmAbs(x), 2 * pi); /* Приведение угла к интервалу [0, 2*pi) для обеспечения периодичности косинуса. */

    double sum = 1, term = 1; /* Инициализация суммы и текущего члена ряда. */
    for (int n = 0; dmAbs(term) > eps; n++) {
        term = -(term * x * x) / ((2 * n + 2) * (2 * n + 1)); /* Вычисление следующего члена ряда Тейлора. */
        sum += term; /** Обновление суммы ряда. */
    }
    return sum; /* Возвращение итоговой суммы. */
}

/* Класс для представления комплексного числа.
Содержит действительную и мнимую части, а также перегруженные операторы для сложения, вычитания и умножения комплексных чисел. */
class ComplexNumber {
private:
    double real; /* Действительная часть. */
    double imaginary; /* Мнимая часть. */
public:
    /* Конструктор для инициализации комплексного числа. */
    ComplexNumber(double realPart, double imaginaryPart) : real(realPart), imaginary(imaginaryPart) {}

    /* Метод для получения действительной части. */
    double getReal() const { return real; }

    /* Метод для получения мнимой части. */
    double getImaginary() const { return imaginary; }

    /* Оператор сложения для комплексных чисел. */
    ComplexNumber operator+(const ComplexNumber& other) const {
        return ComplexNumber(real + other.real, imaginary + other.imaginary); /* Возвращение результата сложения. */
    }

    /* Оператор вычитания для комплексных чисел. */
    ComplexNumber operator-(const ComplexNumber& other) const {
        return ComplexNumber(real - other.real, imaginary - other.imaginary); /* Возвращение результата вычитания. */
    }

    /* Оператор умножения для комплексных чисел. */
    ComplexNumber operator*(const ComplexNumber& other) const {
        double realPart = real * other.real - imaginary * other.imaginary; /* Вычисление действительной части результата умножения. */
        double imaginaryPart = real * other.imaginary + imaginary * other.real; /* Вычисление мнимой части результата умножения. */
        return ComplexNumber(realPart, imaginaryPart); /* Возвращение результата умножения. */
    }
};

/* Функция для вычисления поворотного множителя. */
ComplexNumber W(int N, int n, int m) {
    return ComplexNumber(dmCos(-2 * pi * m * n / N), dmSin(-2 * pi * m * n / N)); /** Возвращение комплексного числа, представляющего поворотный множитель. */
}

/* Функция для выполнения быстрого преобразования Фурье (БПФ). */
ComplexNumber** FFT(int N, const double* signal) {
    ComplexNumber** result = new ComplexNumber * [N]; /* Создание массива для хранения результата. */
    for (int i = 0; i < N; i++) {
        result[i] = new ComplexNumber(0, 0); /* Инициализация каждого элемента массива нулевым комплексным числом. */
    }

    if (N == 1) {
        *result[0] = ComplexNumber(signal[0], 0); /* Если размер сигнала равен 1, просто возвращаем его как комплексное число. */
        return result;
    }

    double* evenSignal = new double[N / 2]; /* Создание массива для хранения чётных элементов сигнала. */
    double* oddSignal = new double[N / 2]; /* Создание массива для хранения нечётных элементов сигнала. */
    for (int i = 0; i < N / 2; ++i) {
        evenSignal[i] = signal[2 * i]; /* Заполнение массива чётных элементов. */
        oddSignal[i] = signal[2 * i + 1]; /* Заполнение массива нечётных элементов. */
    }

    ComplexNumber** evenResult = FFT(N / 2, evenSignal); /* Рекурсивный вызов БПФ для чётных элементов. */
    ComplexNumber** oddResult = FFT(N / 2, oddSignal); /* Рекурсивный вызов БПФ для нечётных элементов. */
    delete[] evenSignal; /* Освобождение памяти, выделенной под массив чётных элементов. */
    delete[] oddSignal; /* Освобождение памяти, выделенной под массив нечётных элементов. */

    for (int k = 0; k < N / 2; ++k) {
        ComplexNumber wk = W(N, k, 1); /* Вычисление поворотного множителя для текущего индекса. */
        ComplexNumber t = wk * *(oddResult[k]); /* Умножение поворотного множителя на результат для нечётного индекса. */
        *result[k] = *(evenResult[k]) + t; /* Вычисление и сохранение результата для текущего индекса. */
        *result[k + N / 2] = *(evenResult[k]) - t; /* Вычисление и сохранение результата для текущего индекса + N/2. */
    }

    for (int i = 0; i < N / 2; i++) {
        delete evenResult[i]; /* Освобождение памяти для каждого элемента результата чётных индексов. */
        delete oddResult[i]; /* Освобождение памяти для каждого элемента результата нечётных индексов. */
    }
    delete[] evenResult; /* Освобождение памяти, выделенной под массив результата чётных индексов. */
    delete[] oddResult; /* Освобождение памяти, выделенной под массив результата нечётных индексов. */

    return result; /* Возвращение результата БПФ. */
}

int main() {
    setlocale(LC_ALL, "Russian"); /* Устанавливаем локализацию на русский язык. */
    const int N = 1024; /* Длина сигнала. */
    int fs = 14000; /* Частота дискретизации. */
    double ts = 1.0 / fs; /* Шаг по времени. */
    double signal[N] = {}; /* Инициализация массива для входного сигнала. */

    /* Генерация входного сигнала.
    Входной сигнал состоит из суммы трёх косинусоид с разными частотами и амплитудами. */
    for (int n = 0; n < N; ++n) {
        signal[n] = (2 * dmCos(2 * pi * 1400 * n * ts + (pi / 2))) +
            (0.5 * dmCos(2 * pi * 11900 * n * ts + (pi / 2))) +
            (3 * dmCos(2 * pi * 12000 * n * ts + (pi / 2)));
    }

    /* Применение быстрого преобразования Фурье (БПФ) к сигналу.
    Результатом является спектр сигнала. */
    ComplexNumber** spectrum = FFT(N, signal);

    std::ofstream outFile("result.txt"); /* Открытие файла для записи результатов. */
    if (!outFile) {
        std::cerr << "Не удалось открыть файл для записи." << std::endl; /* Сообщение об ошибке, если файл не удалось открыть. */
        return 1;
    }

    /* Запись результатов БПФ в файл и вывод на экран.
    Каждая строка файла содержит действительную и мнимую части комплексного числа. */
    for (int i = 0; i < N; ++i) {
        double realPart = spectrum[i]->getReal(); /* Получение действительной части текущего элемента спектра. */
        double imaginaryPart = spectrum[i]->getImaginary(); /* Получение мнимой части текущего элемента спектра. */

        outFile << realPart << " " << imaginaryPart << std::endl; /* Запись действительной и мнимой частей в файл. */

        if (imaginaryPart < 0) {
            std::cout << "X[" << i << "] = " << realPart << " - " << -imaginaryPart << "i" << std::endl; /* Вывод результата на экран в случае отрицательной мнимой части. */
        }
        else {
            std::cout << "X[" << i << "] = " << realPart << " + " << imaginaryPart << "i" << std::endl; /* Вывод результата на экран в случае положительной мнимой части. */
        }
    }

    outFile.close(); /* Закрытие файла. */

    /* Освобождение памяти, выделенной для спектра.
    Для каждого элемента спектра вызываем delete, затем освобождаем массив. */
    for (int i = 0; i < N; ++i) {
        delete spectrum[i]; /* Освобождение памяти для каждого элемента спектра. */
    }
    delete[] spectrum; /* Освобождение памяти для массива спектра. */

    std::cout << "Результаты БПФ записаны в файл result.txt." << std::endl; /* Вывод сообщения об успешном завершении программы. */

    return 0; /* Завершение программы. */
}
