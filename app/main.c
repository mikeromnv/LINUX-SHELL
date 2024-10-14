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

// чтение бинарника
void read_binary(const char *filename){
  FILE *file = fopen(filename, "rb");
  if (file == NULL){
    perror ("Не удалось открыть бинарный файл");
    return;
  }
  // определяем размер файла
  fseek(file, 0, SEEK_END);
  long file_s = ftell(file);
  rewind(file); // устанавливает указатель на начло файла
  
  unsigned char *buffer = (unsigned char *)malloc(file_s);
  if (buffer == NULL){
    perror("Ошибка выделения памяти");
    fclose(file);
    return;
  }
  
  size_t count = fread(buffer, 1, file_s, file);
  if (count != file_s){
    perror("ошибка чтения данных");
    free(buffer); //освобождаем память
    fclose(file);
    return;
  }
  
  //выводд
  printf("содержимое бинарного файла %s:\n", filename);
  for (long i=0;i<file_s;i++){
    printf("%02X ", buffer[i]);
  }
  printf("\n");
  free(buffer);
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
        //чтение бинарника
        if (strncmp(input, "cat ", 4) == 0){
            read_binary(input + 4);
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
