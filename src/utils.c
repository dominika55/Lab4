#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
