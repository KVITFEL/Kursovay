#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <limits.h>
#include <float.h>

#define MAX_WEBCAMS_COUNT 1000 // максимальное количество веб-камер в массиве
#define MAX_CONNECTION_TYPE_LEN 20 // максимальная длина строки типа подключения
#define MAX_FILENAME_LEN 20 // максимальная длина имени файла
#define MAX_STR_INT_LEN 10 // максимальная длина строки целого числа
#define MAX_STR_DOUBLE_LEN 46 // максимальная длина строки вещественного числа
#define MAX_VIEW_ANGLE 360 // максимальный угол обзора

#define ERR_OK 0 // OK
#define ERR_READ 1 // ошибка при считывании данных
#define ERR_OVERFLOW_ARRAY 2 // ошибка при переполнении массива веб-камер
#define ERR_MEMORY 3 // ошибка при выделении памяти
#define ERR_WRONG_DOUBLE_NUM_RECORD 4 // ошибка при записи вещественного числа
#define ERR_POINTER 5 // ошибка при передаче пустого указателя
#define ERR_EMPTY_ARRAY 6 // ошибка при пустом массиве веб-камер

typedef struct
{
    int width; // ширина
    int height; // высота
} resolution_t;

typedef struct
{
    resolution_t resolution; // разрешение
    double megapixels; // число мегапикселей
    int view_angle; // угол обзора
    int microphone_presence; // наличие микрофона
    char connection_type[MAX_CONNECTION_TYPE_LEN]; // тип подключения
} webcam_t;

typedef struct
{
    webcam_t arr[MAX_WEBCAMS_COUNT]; // массив веб-камер
    int count; // количество веб-камер в массиве
} webcams_t;

/**
 * Данная функция выводит пункты меню в соответствующий поток
 * @param stream Указатель на поток вывода
 * @return Код возврата
 */
int print_menu(FILE* stream);

/**
 * Данная функция запрашивает количество новых веб-камер, считывает и добавляет их в соответствующий массив
 * @param webcams Указатель на массив веб-камер
 * @return Код возврата
 */
int add_webcams(webcams_t* webcams);

/**
 * Данная функция сбрасывает буфер
 * @param str Указатель на буфер
 */
void flush(char* str);

/**
 * Данная функция проверяет, является ли заданный символ цифрой
 * @param symb Символ для проверки
 * @return Код возврата
 */
int is_digit(char symb);

/**
 * Данная функция проверяет, является ли заданная последовательность числом
 * @param str Указатель на строку
 * @param count Количество символов в строке
 * @return Код возврата
 */
int is_sequence_of_digits(char* str, int count);

/**
 * Данная функция считывает из соответствующего потока целое число из заданного диапазона
 * @param stream Указатель на поток ввода
 * @param num Указатель на переменную для хранения целого числа
 * @param message Сообщение при запросе ввода
 * @param first Начальное значение диапазона
 * @param second Конечное значение диапазона
 * @param len Максимальная длина числа при его переводе в строку
 * @return Код возврата
 */
int read_int_num(FILE* stream, int* num, char* message, int first, int second, size_t len);

/**
 * Данная функция проверяет, является ли заданная последовательность вещественным числом
 * @param str Указатель на строку
 * @param count Количество символов в строке
 * @return Код возврата
 */
int is_double_sequence(char* str, int count);

/**
 * Данная функция считывает из соответствующего потока вещественное число из заданного диапазона
 * @param stream Указатель на поток ввода
 * @param num Указатель на переменную для хранения вещественного числа
 * @param message Сообщение при запросе ввода
 * @param first Начальное значение диапазона
 * @param second Конечное значение диапазона
 * @param len Максимальная длина числа при его переводе в строку
 * @return Код возврата
 */
int read_double_num(FILE* stream, double* num, char* message, double first, double second, size_t len);

/**
 * Данная функция считывает из соответствующего потока строку определенной длины
 * @param stream Указатель на поток ввода
 * @param string Указатель на переменную для хранения строки
 * @param message Сообщение при запросе ввода
 * @param len Максимальная длина строки
 * @return Код возврата
 */
int read_string(FILE* stream, char* string, char* message, size_t len);

/**
 * Данная функция считывает веб-камеру из консоли и возвращает ее в качестве результата
 * @return Считанная веб-камера
 */
webcam_t read_webcam_from_console();

/**
 * Данная функция выводит в консоль соответствующий массив веб-камер
 * @param webcams Указатель на массив веб-камер
 * @return Код возврата
 */
int print_webcams_in_console(webcams_t* webcams);

/**
 * Данная функция выводит в консоль соответствующую веб-камеру
 * @param webcam Указатель на переменную для хранения веб-камеры
 * @return Код возврата
 */
int print_webcam_in_console(webcam_t* webcam);

/**
 * Данная функция запрашивает имя выходного файла и записывает в него соответствующий массив веб-камер
 * @param webcams Указатель на массив веб-камер
 * @return Код возврата
 */
int save_webcams_in_file(webcams_t* webcams);

/**
 * Данная функция выводит в соответствующий файл массив веб-камер
 * @param file Файловый указатель
 * @param webcams Указатель на массив веб-камер
 * @return Код возврата
 */
int print_webcams_in_file(FILE* file, webcams_t* webcams);

/**
 * Данная функция запрашивает имя файла с данными, считывает из него веб-камеры и заполняет ими соответствующий массив
 * @param webcams Указатель на массив веб-камер
 * @param flag_clear Флаг для очистки предыдущих веб-камер
 * @return Код возврата
 */
int read_webcams_from_file(webcams_t* webcams, int flag_clear);

/**
 * Данная функция считывает из файла веб-камеру и записывает полученные данные в соответствующую переменную
 * @param file Файловый указатель
 * @param webcam Указатель на переменную для хранения информации о веб-камере
 * @return Код возврата
 */
int read_webcam_from_file(FILE* file, webcam_t* webcam);

/**
 * Данная функция запрашивает угол обзора, производит поиск данных веб-камер в соответствующем массиве и выводит их в консоль
 * @param webcams Указатель на массив веб-камер
 * @return Код возврата
 */
int search_webcams_by_view_angle(webcams_t* webcams);

/**
 * Данная функция запрашивает тип подключения, производит поиск данных веб-камер в соответствующем массиве и выводит их в консоль
 * @param webcams Указатель на массив веб-камер
 * @return Код возврата
 */
int search_webcams_by_connection_type(webcams_t* webcams);

/**
 * Данная функция сортирует соответствующий массив веб-камер по возрастанию разрешений с помощью функции qsort (быстрая сортировка) и выводит его в консоль
 * @param webcams Указатель на массив веб-камер
 * @return Код возврата
 */
int sort_by_resolution_and_print_webcams(webcams_t* webcams);

/**
 * Данная функция сравнивает разрешения соответствующих веб-камер и возвращает в качестве результата их разность
 * (> 0 - разрешение первой веб-камеры больше, == 0 - разрешения равны, < 0 - разрешение второй веб-камеры больше)
 * @param first Указатель на первую веб-камеру
 * @param second Указатель на вторую веб-камеру
 * @return Разность разрешений
 */
int webcams_cmp_resolution(const void* first, const void* second);

/**
 * Данная функция меняет местами соответствующие элементы массива веб-камер
 * @param first Указатель на первую структур
 * @param second Указатель на вторую структуру
 * @return Код возврата
 */
int swap(webcam_t* first, webcam_t* second);

/**
 * Данная функция очищает массив веб-камер
 * @param webcams Указатель на массив веб-камер
 * @return Код возврата
 */
int clear_webcams(webcams_t* webcams);

int main()
{
    setlocale(LC_ALL, "ru");
    printf("\n");

    webcams_t webcams = { .count = 0 };

    int option;
    int flag_menu = 1;

    while (flag_menu)
    {
        print_menu(stdout);

        read_int_num(stdin, &option, "Введите номер действия", 0, 9, 1);

        printf("\n");

        if (option == 0)
        {
            printf("Программа завершена\n");
            flag_menu = 0;
        }
        else
        {
            if (option == 1)
                add_webcams(&webcams);
            else if (option == 2)
                print_webcams_in_console(&webcams);
            else if (option == 3)
                save_webcams_in_file(&webcams);
            else if (option == 4)
                read_webcams_from_file(&webcams, 0);
            else if (option == 5)
                read_webcams_from_file(&webcams, 1);
            else if (option == 6)
                search_webcams_by_view_angle(&webcams);
            else if (option == 7)
                search_webcams_by_connection_type(&webcams);
            else if (option == 8)
                sort_by_resolution_and_print_webcams(&webcams);
            else if (option == 9)
                clear_webcams(&webcams);
            printf("\n");
        }
    }

    return 0;
}

int print_menu(FILE* stream)
{
    if (!stream)
    {
        printf("Передача пустого указателя\n");
        return ERR_POINTER;
    }

    fprintf(stream, "Меню:\n");
    fprintf(stream, "0. Завершить программу\n");
    fprintf(stream, "1. Добавить веб-камеру\n");
    fprintf(stream, "2. Вывести веб-камеры в консоль\n");
    fprintf(stream, "3. Сохранить веб-камеры в файл\n");
    fprintf(stream, "4. Считать веб-камеры из файла в массив с сохранением текущих веб-камер\n");
    fprintf(stream, "5. Считать веб-камеры из файла в массив с удалением текущих веб-камер\n");
    fprintf(stream, "6. Произвести поиск по углу обзора\n");
    fprintf(stream, "7. Произвести поиск по интерфейсу подключения\n");
    fprintf(stream, "8. Сортировать веб-камеры по разрешению\n");
    fprintf(stream, "9. Очистить массив веб-камер\n\n");

    return ERR_OK;
}

int add_webcams(webcams_t* webcams)
{
    if (!webcams)
    {
        printf("Передача пустого указателя\n");
        return ERR_POINTER;
    }

    if (webcams->count >= MAX_WEBCAMS_COUNT)
    {
        printf("Массив веб-камер заполнен\n");
        return ERR_OVERFLOW_ARRAY;
    }

    int count;
    read_int_num(stdin, &count, "Введите количество добавляемых веб-камер", 1, MAX_WEBCAMS_COUNT - webcams->count, 4);

    for (int i = webcams->count; i < webcams->count + count; ++i)
    {
        webcams->arr[i] = read_webcam_from_console();
        printf("\nВеб-камера успешно добавлена\n");
    }

    webcams->count += count;

    return ERR_OK;
}

void flush(char* str)
{
    if (str[strlen(str) - 1] != '\n')
        while (getchar() != '\n');
}

int is_digit(char symb)
{
    char digits[] = "0123456789";
    if (!strchr(digits, symb))
        return 0;
    return 1;
}

int is_sequence_of_digits(char* str, int count)
{
    int flag = 1;

    for (int i = 0; i < count && flag; ++i)
    {
        if (*str && is_digit(*str))
            str++;
        else
            flag = 0;
    }

    return flag;
}

int read_int_num(FILE* stream, int* num, char* message, int first, int second, size_t len)
{
    int flag = 1;

    char* tmp_str = malloc(sizeof(char) * (len + 2));

    if (!tmp_str)
        return ERR_MEMORY;

    while (flag)
    {
        printf("%s: ", message);

        fgets(tmp_str, len + 2, stream);

        if (!is_sequence_of_digits(tmp_str, strlen(tmp_str) - 1))
        {
            flush(tmp_str);
            printf("В введенной строке присутствуют некорректные символы\n\n");
        }
        else if (tmp_str[strlen(tmp_str) - 1] != '\n')
        {
            flush(tmp_str);
            printf("Введено неверное количество символов\n\n");
        }
        else if (strlen(tmp_str) < 2)
        {
            flush(tmp_str);
            printf("Введена пустая строка\n\n");
        }
        else if (tmp_str[0] == '0' && tmp_str[1] != '\n')
        {
            flush(tmp_str);
            printf("В строке присутствуют лидирующие нули\n\n");
        }
        else
        {
            int tmp_num = atoi(tmp_str);

            if ((tmp_num < first) || (tmp_num > second))
                printf("Число не входит в заданный диапазон\n\n");
            else
            {
                *num = tmp_num;
                flag = 0;
            }
        }
    }

    free(tmp_str);

    return ERR_OK;
}

int is_double_sequence(char* str, int count)
{
    char* p_plus = strrchr(str, '+');
    char* p_minus = strrchr(str, '-');
    char* p_dot = strrchr(str, '.');

    int diff;
    int dot_count = 0;

    if (p_dot)
        diff = count - (p_dot - str);

    for (int i = 0; i < count; ++i, ++str)
    {
        if (!strchr("0123456789+-.", *str))
        {
            printf("В введенной строке присутствуют некорректные символы\n\n");
            return ERR_WRONG_DOUBLE_NUM_RECORD;
        }

        if (*str == '.')
            ++dot_count;

        if (dot_count > 1)
        {
            printf("Неверная запись вещественного числа\n\n");
            return ERR_WRONG_DOUBLE_NUM_RECORD;
        }
    }

    if ((p_plus && p_plus - str) || (p_minus && p_minus - str) || (p_dot && diff > 7))
    {
        printf("Неверная запись вещественного числа\n\n");
        return ERR_WRONG_DOUBLE_NUM_RECORD;
    }

    return ERR_OK;
}

int read_double_num(FILE* stream, double* num, char* message, double first, double second, size_t len)
{
    int flag = 1;

    char* tmp_str = malloc(sizeof(char) * (len + 2));

    if (!tmp_str)
        return ERR_MEMORY;

    while (flag)
    {
        printf("%s: ", message);

        fgets(tmp_str, len + 2, stream);

        if (is_double_sequence(tmp_str, strlen(tmp_str) - 1) != ERR_OK)
            flush(tmp_str);
        else if (tmp_str[strlen(tmp_str) - 1] != '\n')
        {
            flush(tmp_str);
            printf("Введено неверное количество символов\n\n");
        }
        else if (strlen(tmp_str) < 2)
        {
            flush(tmp_str);
            printf("Введена пустая строка\n\n");
        }
        else
        {
            double tmp_num = atof(tmp_str);

            if (tmp_num < first || tmp_num > second)
                printf("Число не входит в заданный диапазон\n\n");
            else
            {
                *num = tmp_num;
                flag = 0;
            }
        }
    }

    free(tmp_str);

    return ERR_OK;
}

int read_string(FILE* stream, char* string, char* message, size_t len)
{
    int flag = 1;

    char* tmp_str = malloc(sizeof(char) * (len + 2));

    if (!tmp_str)
        return ERR_MEMORY;

    while (flag)
    {
        printf("%s: ", message);

        fgets(tmp_str, sizeof(tmp_str), stream);

        if (tmp_str[strlen(tmp_str) - 1] != '\n')
        {
            flush(tmp_str);
            printf("Введено неверное количество символов\n\n");
        }
        else if (strlen(tmp_str) < 2)
        {
            flush(tmp_str);
            printf("Введена пустая строка\n\n");
        }
        else
        {
            tmp_str[strlen(tmp_str) - 1] = '\0';
            strcpy(string, tmp_str);
            flag = 0;
        }
    }

    free(tmp_str);

    return ERR_OK;
}

webcam_t read_webcam_from_console()
{
    FILE* stream = stdin;

    webcam_t webcam;

    read_int_num(stream, &(webcam.resolution.width), "Введите ширину", 1, INT_MAX, MAX_STR_INT_LEN);
    read_int_num(stream, &(webcam.resolution.height), "Введите высоту", 1, INT_MAX, MAX_STR_INT_LEN);
    read_double_num(stream, &(webcam.megapixels), "Введите число мегапикселей", 1, FLT_MAX, MAX_STR_DOUBLE_LEN);
    read_int_num(stream, &(webcam.view_angle), "Введите угол обзора", 1, MAX_VIEW_ANGLE, 3);
    read_int_num(stream, &(webcam.microphone_presence), "Наличие микрофона", 0, 1, 1);
    read_string(stream, webcam.connection_type, "Введите тип подключения", MAX_CONNECTION_TYPE_LEN);

    return webcam;
}

int print_webcams_in_console(webcams_t* webcams)
{
    if (!webcams)
    {
        printf("Передача пустого указателя\n");
        return ERR_POINTER;
    }

    if (webcams->count == 0)
    {
        printf("Данные отсутствуют\n");
        return ERR_EMPTY_ARRAY;
    }

    printf("---------------------------------\n");
    for (int i = 0; i < webcams->count; ++i)
    {
        print_webcam_in_console(webcams->arr + i);
        printf("---------------------------------\n");
    }

    return ERR_OK;
}

int print_webcam_in_console(webcam_t* webcam)
{
    if (!webcam)
    {
        printf("Передача пустого указателя\n");
        return ERR_POINTER;
    }

    printf("Ширина: %d\n", webcam->resolution.width);
    printf("Высота: %d\n", webcam->resolution.height);
    printf("Число мегапикселей: %lf\n", webcam->megapixels);
    printf("Угол обзора: %d\n", webcam->view_angle);
    printf("Наличие микрофона: %d\n", webcam->microphone_presence);
    printf("Тип подключения: %s\n", webcam->connection_type);

    return ERR_OK;
}

int save_webcams_in_file(webcams_t* webcams)
{
    if (!webcams)
    {
        printf("Передача пустого указателя\n");
        return ERR_POINTER;
    }

    if (webcams->count == 0)
    {
        printf("Данные отсутствуют\n");
        return ERR_EMPTY_ARRAY;
    }

    char filename[MAX_FILENAME_LEN + 1];

    FILE* file;

    int flag = 1;

    while (flag)
    {
        read_string(stdin, filename, "Введите имя выходного файла", MAX_FILENAME_LEN);

        file = fopen(filename, "w");

        if (!file)
            perror("Ошибка при открытии файла");
        else
            flag = 0;
    }

    print_webcams_in_file(file, webcams);
    printf("Данные успешно выгружены в файл\n");
    fclose(file);

    return ERR_OK;
}

int print_webcams_in_file(FILE* file, webcams_t* webcams)
{
    if (!file || !webcams)
    {
        printf("Передача пустого указателя\n");
        return ERR_POINTER;
    }

    for (int i = 0; i < webcams->count; ++i)
    {
        fprintf(file, "%d\n", webcams->arr[i].resolution.width);
        fprintf(file, "%d\n", webcams->arr[i].resolution.height);
        fprintf(file, "%lf\n", webcams->arr[i].megapixels);
        fprintf(file, "%d\n", webcams->arr[i].view_angle);
        fprintf(file, "%d\n", webcams->arr[i].microphone_presence);
        fprintf(file, "%s\n", webcams->arr[i].connection_type);
    }

    return ERR_OK;
}

int read_webcams_from_file(webcams_t* webcams, int flag_clear)
{
    if (!webcams)
    {
        printf("Передача пустого указателя\n");
        return ERR_POINTER;
    }

    if (flag_clear)
        webcams->count = 0;

    char filename[MAX_FILENAME_LEN + 1];

    FILE* file;

    int flag = 1;

    while (flag)
    {
        read_string(stdin, filename, "Введите имя файла с данными", MAX_FILENAME_LEN);

        file = fopen(filename, "r");

        if (!file)
            perror("Ошибка при открытии файла");
        else
            flag = 0;
    }

    printf("\n");

    webcam_t webcam;

    while (!feof(file))
    {
        read_webcam_from_file(file, &webcam);

        if (webcams->count < MAX_WEBCAMS_COUNT)
        {
            webcams->arr[webcams->count] = webcam;
            webcams->count++;
        }
        else
        {
            printf("Массив веб-камер заполнен\n");
            fclose(file);
            return ERR_OVERFLOW_ARRAY;
        }
    }

    printf("Данные успешно считаны из файла\n");
    fclose(file);

    return ERR_OK;
}

int read_webcam_from_file(FILE* file, webcam_t* webcam)
{
    if (!file || !webcam)
    {
        printf("Передача пустого указателя\n");
        return ERR_POINTER;
    }

    fscanf(file, "%d\n%d\n%lf\n%d\n%d\n", &(webcam->resolution.width), &(webcam->resolution.height), &(webcam->megapixels), &(webcam->view_angle), &(webcam->microphone_presence));

    char tmp_str[MAX_CONNECTION_TYPE_LEN + 2];
    fgets(tmp_str, sizeof(tmp_str), file);
    tmp_str[strlen(tmp_str) - 1] = '\0';
    strcpy(webcam->connection_type, tmp_str);

    fscanf(file, "\n");

    return ERR_OK;
}

int search_webcams_by_view_angle(webcams_t* webcams)
{
    if (!webcams)
    {
        printf("Передача пустого указателя\n");
        return ERR_POINTER;
    }

    if (webcams->count == 0)
    {
        printf("Данные отсутствуют\n");
        return ERR_EMPTY_ARRAY;
    }

    int view_angle;
    int count = 0;

    read_int_num(stdin, &view_angle, "Введите угол обзора", 1, MAX_VIEW_ANGLE, 4);

    printf("\n");

    for (int i = 0; i < webcams->count; ++i)
        if (webcams->arr[i].view_angle == view_angle)
        {
            if (count == 0)
                printf("---------------------------------\n");
            print_webcam_in_console(webcams->arr + i);
            printf("---------------------------------\n");
            count++;
        }

    if (count == 0)
        printf("Веб-камер с таким углом обзора нет\n");

    return ERR_OK;
}

int search_webcams_by_connection_type(webcams_t* webcams)
{
    if (!webcams)
    {
        printf("Передача пустого указателя\n");
        return ERR_POINTER;
    }

    if (webcams->count == 0)
    {
        printf("Данные отсутствуют\n");
        return ERR_EMPTY_ARRAY;
    }

    char connection_type[MAX_CONNECTION_TYPE_LEN];

    int count = 0;

    read_string(stdin, connection_type, "Введите тип подключения", MAX_CONNECTION_TYPE_LEN);

    printf("\n");

    for (int i = 0; i < webcams->count; ++i)
        if (!strcmp(webcams->arr[i].connection_type, connection_type))
        {
            print_webcam_in_console(webcams->arr + i);
            count++;
        }

    if (count == 0)
        printf("Веб-камер с таким типом подключения нет\n");

    return ERR_OK;
}

int sort_by_resolution_and_print_webcams(webcams_t* webcams)
{
    if (!webcams)
    {
        printf("Передача пустого указателя\n");
        return ERR_POINTER;
    }

    if (webcams->count == 0)
    {
        printf("Данные отсутствуют\n");
        return ERR_EMPTY_ARRAY;
    }

    qsort(webcams, webcams->count, sizeof(webcam_t), webcams_cmp_resolution);
    printf("Данные отсортированы!\n\n");
    print_webcams_in_console(webcams);

    return ERR_OK;
}

int webcams_cmp_resolution(const void* first, const void* second)
{
    webcam_t first_webcam = *(webcam_t*)first;
    webcam_t second_webcam = *(webcam_t*)second;
    return (first_webcam.resolution.width * first_webcam.resolution.height) - (second_webcam.resolution.width * second_webcam.resolution.height);
}

int swap(webcam_t* first, webcam_t* second)
{
    if (!first || !second)
    {
        printf("Передача пустого указателя\n");
        return ERR_POINTER;
    }

    webcam_t tmp = *first;
    *first = *second;
    *second = tmp;

    return ERR_OK;
}

int clear_webcams(webcams_t* webcams)
{
    if (!webcams)
    {
        printf("Передача пустого указателя\n");
        return ERR_POINTER;
    }

    webcams->count = 0;
    printf("Массив веб-камер очищен\n");

    return ERR_OK;
}
