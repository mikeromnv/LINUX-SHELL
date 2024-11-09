#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <stdint.h>
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

//8 чтение бинарника
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

/*void handle_sighup() {
    printf("The program interrupted");
    exit(0);
}
*/
void handle_SIGHUP(int signal){
  if (signal == SIGHUP){
    printf("The program interrupted");
  }
}

// 10 определить является ли диск загрузочным
// Функция для проверки загрузочного диска
void is_bootable_device(char* device_name) {
    // Удаляем лишние пробелы
    while (*device_name == ' ') {
        device_name++;
    }

    // Формируем полный путь к устройству
    char full_path[128];
    snprintf(full_path, sizeof(full_path), "/dev/%s", device_name);

    // Открываем устройство для чтения
    FILE* device_file = fopen(full_path, "rb");
    if (device_file == NULL) {
        printf("Не удалось открыть диск %s!\n", device_name);
        return;
    }

    // Переходим к 510 байту
    if (fseek(device_file, 510, SEEK_SET) != 0) {
        printf("Ошибка при смещении к сигнатуре на диске!\n");
        fclose(device_file);
        return;
    }

    // Считываем последние два байта
    uint8_t signature[2];
    if (fread(signature, 1, 2, device_file) != 2) {
        printf("Ошибка при чтении сигнатуры диска!\n");
        fclose(device_file);
        return;
    }
    fclose(device_file);

    // Проверка сигнатуры на 55 AA
    if (signature[0] == 0x55 && signature[1] == 0xAA) {
        printf("Диск %s является загрузочным.\n", device_name);
    } else {
        printf("Диск %s не является загрузочным.\n", device_name);
    }
}


int main() {
    
    char input[BUFFER_SIZE];               // Буфер для ввода команд
    char history[HISTORY_SIZE][BUFFER_SIZE]; // История команд
    int history_count = 0;                  // Счётчик команд в истории
    bool f = false;
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
            f = true;
            continue;
        }
        //чтение бинарника
        if (strncmp(input, "cat ", 4) == 0){
            read_binary(input + 4);
            f = true;
            continue;
        }
        
        signal(SIGHUP, handle_SIGHUP);
        
        // 10 определить является ли диск загрузочным
        if (strncmp(input, "\\l", 2) == 0) {
            // Проверка, является ли диск загрузочным

            char* device_name = input + 3; // Извлечение имени устройства, например, "sda"

            is_bootable_device(device_name);

        }
        
        // Проверяем команду echo
        if (strncmp(input, "echo ", 5) == 0) {
            printf("%s\n", input + 5); // Выводим всё, что после "echo "
            f = true;
            continue;
        }
        if (f == false){
          printf("There is no such command!\n");
        }
        
        
        
        
    }
    while (!feof(stdin));
    // Сохраняем историю команд в файл перед выходом
    save_history_to_file(history, history_count);

    return 0;
}

// kill
// хендлеры 
// signal тип сигнала что хотим сделать
