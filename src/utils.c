#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

int read_int_safe(const char *prompt, int min, int max) {
    char buf[STR_BUF_LEN];

    while (1) {
        printf("%s", prompt);

        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            clearerr(stdin);
            continue;
        }

        if (strchr(buf, '\n') == NULL) {
            clear_input_buffer();
        }

        char *end;
        long v = strtol(buf, &end, 10);

        while (*end == ' ' || *end == '\t') {
            end++;
        }

        if (end == buf || (*end != '\n' && *end != '\0')) {
            printf("Некорректный ввод. Введите целое число.\n");
            continue;
        }

        if (v < min || v > max) {
            printf("Значение должно быть в диапазоне [%d..%d].\n", min, max);
            continue;
        }

        return (int)v;
    }
}

void read_line_safe(char *buf, int size, const char *prompt) {
    printf("%s", prompt);

    if (fgets(buf, size, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }

    size_t len = strlen(buf);

    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else if (len == (size_t)(size - 1)) {
        clear_input_buffer();
    }
}

void save_diary(const Player *p, const GameTime *t) {
    if (p == NULL || t == NULL) return;

    FILE *fp = fopen("diary.txt", "a");
    if (fp == NULL) {
        printf("Не удалось открыть diary.txt\n");
        return;
    }

    fseek(fp, 0, SEEK_END);
    if (ftell(fp) > 0) {
        fprintf(fp, "\n\n");
    }

    fprintf(fp, "Фермер: %s\n", p->name);
    fprintf(fp, "День: %d, час: %02d:00\n", t->day, t->hour);
    fprintf(fp, "Предметы в инвентаре:\n");

    for (int i = 0; i < INVENTORY_SIZE; ++i) {
        if (p->inventory[i] != ITEM_EMPTY) {
            fprintf(fp, "   • %s\n", item_name(p->inventory[i]));
        }
    }
    fclose(fp);
    printf("Сводка записана в diary.txt!\n");
}

void unpack_map(void) {
    FILE *fin = fopen("input.txt", "r");
    FILE *fout = fopen("output.txt", "w");

    if (fin == NULL) {
        printf("Ошибка: файл input.txt не найден!\n");
        return;
    }
    if (fout == NULL) {
        printf("Ошибка: не удалось создать output.txt\n");
        fclose(fin);
        return;
    }

    char line[256];
    int expectedLen = -1;
    int success = 1;

    while (fgets(line, sizeof(line), fin) != NULL && success) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }

        int currentLen = 0;
        int i = 0;

        while (i < (int)len && success) {
            char currentChar = line[i];

            if (isdigit((unsigned char)currentChar)) {
                int count = currentChar - '0';
                i++;
                if (i >= (int)len) {
                    printf("Ошибка: цифра без символа!\n");
                    success = 0;
                    break;
                }
                char symbol = line[i];
                for (int k = 0; k < count; ++k) {
                    putchar(symbol);
                    fprintf(fout, "%c", symbol);
                    currentLen++;
                }
                i++;
            } else {
                putchar(currentChar);
                fprintf(fout, "%c", currentChar);
                currentLen++;
                i++;
            }
        }

        putchar('\n');
        fprintf(fout, "\n");

        if (expectedLen == -1) {
            expectedLen = currentLen;
        } else if (currentLen != expectedLen) {
            printf("Ошибка геометрии: строки разной длины!\n");
            success = 0;
        }
    }

    fclose(fin);
    fclose(fout);

    if (success) {
        printf("Карта распакована в output.txt!\n");
    } else {
        printf("Распаковка завершена с ошибками\n");
    }
}
