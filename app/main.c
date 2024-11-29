#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/mount.h>
#include <stdint-gcc.h>
#include <sys/stat.h>


#define _GNU_SOURCE

#define BUFFER_SIZE 1024 //буфер для ввода

// обработчик сигнала sighup
void handle_SIGHUP(int signal){
  if (signal == SIGHUP){
    printf("Configuration reloaded\n");
    exit(0);
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

    printf("path: %s\n", full_path);

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

//cron


// 12. по 'mem <procid>' получить дамп памяти процесса
// Функция для объединения файлов в дамп
bool appendToFile(char* path1, char* path2) {
    FILE *f1 = fopen(path1, "a");
    FILE *f2 = fopen(path2, "r");
    if (!f1 || !f2) {
        printf("Error while reading file %s\n", path2);
        return false;
    }
    char buf[256];

    while (fgets(buf, 256, f2) != NULL) {
        fputs(buf, f1);
    }
    fclose(f1);
    fclose(f2);
    return true;
}
// 12. Создание дампа памяти процесса
void makeDump(DIR* dir, char* path) {
    FILE* res = fopen("res.txt", "w+");
    fclose(res);
    struct dirent* ent;
    char* file_path;
    while ((ent = readdir(dir)) != NULL) {

        asprintf(&file_path, "%s/%s", path, ent->d_name);
        if(!appendToFile("res.txt", file_path)) {
            return;
        }
    }
    printf("Dump completed!\n");
}

int main() {
    signal(SIGHUP, handle_SIGHUP);
    
    char input[BUFFER_SIZE];               // Буфер для ввода команд
    
    FILE *file = fopen("HISTORY.txt", "a");
    
    do {  
        bool f = false;  
        printf("$ ");
        fflush(stdout);
        
        // 2. выход по (Ctrl+D)
        if (fgets(input, BUFFER_SIZE, stdin) == NULL) {
            printf("\nЗавершение работы (Ctrl+D)\n");
            fclose(file);
            break; // выход из цикла при EOF
        }

        // Убираем символ новой строки, если он есть
        input[strcspn(input, "\n")] = '\0';

        // 3. Проверяем на команды выхода
        if (strcmp(input, "exit") == 0 || strcmp(input, "\\q") == 0) {
            printf("Завершение работы (exit/\\q)\n");
            break;
        }

        // 4. Сохраняем введённую команду в файл
        if (file){
          fprintf(file, "%s\n", input);
          fclose(file);
          file = fopen("HISTORY.txt", "a");
        }
        
        
        // 7. Вывод окружения переменной
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
        // 5. Проверяем команду echo
        if (strncmp(input, "echo ", 5) == 0) {
            printf("%s\n", input + 5); // Выводим всё, что после "echo "
            f = true;
            continue;
        }
        // 8. выполнение бинарника
        if (strncmp(input, "run ", 4) == 0){
            pid_t p = fork();
            if (p == 0){
              char *argv[] = { "sh", "-c", input + 4, 0 };
              execvp(argv[0], argv);
              fprintf(stderr, "Failed to exec shell on %s\n", input + 4);
              f = true;
              exit(1);
              
            }
            sleep(1);
            continue;
        }
      
        // 10. определить является ли диск загрузочным
        if (strncmp(input, "\\l", 2) == 0) {
            // Проверка, является ли диск загрузочным

            char* device_name = input + 3; // Извлечение имени устройства, например, "sda"

            is_bootable_device(device_name);
            f = true;
            continue;
        }
        
        //11 "\cron"
        
        
        
        //12. получить дамп памяти по id процесса
        if (strncmp(input, "\\proc ", 6) == 0) {
            char* path;
            asprintf(&path, "/proc/%s/map_files", input+6);

            DIR* dir = opendir(path);
            if (dir) {
                makeDump(dir, path);
            }
            else {
                printf("Process not found\n");
            }
            f = true;
            continue;
        }
        
        // 6. Отсутствие команды
        if (f == false){
          printf("There is no such command!\n"); 
        } 
    }
    while (!feof(stdin));
    return 0;
}

