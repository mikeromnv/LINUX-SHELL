#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024 //буфер для ввода
#define HISTORY_SIZE 1000 //для истории
#define HISTORY_FILE "command_history.txt"

// Функция для записи истории команд в файл
void save_history_to_file(char history[][BUFFER_SIZE], int count) {
    FILE *file = fopen(HISTORY_FILE, "a");
    if (file == NULL) {
        perror("Не удалось открыть файл для записи истории");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s\n", history[i]);
    }
    fclose(file);
}


int main() {
    char input[BUFFER_SIZE];               // Буфер для ввода команд
    char history[HISTORY_SIZE][BUFFER_SIZE]; // История команд
    int history_count = 0;                  // Счётчик команд в истории

    do {
               
        printf("MIKE$ ");
        fflush(stdout);
        // Чтение ввода с клавиатуры, проверка на EOF (Ctrl+D)
        if (fgets(input, BUFFER_SIZE, stdin) == NULL) {
            printf("\nЗавершение работы (Ctrl+D)\n");
            break; // выход из цикла при EOF
        }

        // Убираем символ новой строки, если он есть
        input[strcspn(input, "\n")] = '\0';

        // Проверяем на команды выхода
        if (strcmp(input, "exit") == 0 || strcmp(input, "\\q") == 0) {
            printf("Завершение работы (exit/\\q)\n");
            break;
        }

        // Сохраняем введённую команду в историю (если есть место)
        if (history_count < HISTORY_SIZE) {
            strcpy(history[history_count], input);
            history_count++;
        }
        if (strcmp(input, "echo $PATH") == 0){
            char *path = getenv("PATH");
            if (path != NULL){
                printf("%s\n", path);
            } else{
                printf("Переменная PATH не найдена\n");
            }
            continue;
        }
        
        // Проверяем команду echo
        if (strncmp(input, "echo ", 5) == 0) {
            printf("%s\n", input + 5); // Выводим всё, что после "echo "
            continue;
        }
        
        
        

        // Если это не команда выхода и не echo, просто выводим строку
        printf("Вы ввели: %s\n", input);
    }
    while (!feof(stdin));
    // Сохраняем историю команд в файл перед выходом
    save_history_to_file(history, history_count);

    return 0;
}
