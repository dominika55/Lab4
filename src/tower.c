#include "tower.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static char **generate_map(int *out_size, int *hero_y, int *hero_x) {
    int size = rand() % (TOWER_MAX_MAP_SIZE - TOWER_MIN_MAP_SIZE + 1) + TOWER_MIN_MAP_SIZE;
    *out_size = size;

    char **map;
    printf("\nГенерируем карту размером %dx%d\n", size, size);

    map = (char **)malloc(size * sizeof(char *));
    if (map == NULL) {
        printf("Ошибка памяти!\n");
        return NULL;
    }

    int obstacle_chance = rand() % (TOWER_MAX_VERGE - TOWER_MIN_VERGE + 1) + TOWER_MIN_VERGE;
    int hero_placed = 0;

    for (int i = 0; i < size; i++) {
        map[i] = malloc((size + 1) * sizeof(char));

        if (map[i] == NULL) {
            for (int j = 0; j < i; j++) free(map[j]);
            free(map);
            return NULL;
        }

        for (int j = 0; j < size; j++) {
            if (i == 0 || i == size - 1 || j == 0 || j == size - 1) {
                map[i][j] = '#';
            } else {
                map[i][j] = '.';

                if (rand() % 100 < obstacle_chance)
                    map[i][j] = (rand() % 2 == 0) ? 'T' : 'R';

                if (!hero_placed && map[i][j] == '.' && i == size / 2 && j == size / 2) {
                    map[i][j] = '@';
                    *hero_y = i;
                    *hero_x = j;
                    hero_placed = 1;
                }
            }
        }
        map[i][size] = '\0';
    }
    return map;
}

static void setup_stair_U(char **map, int size, int hero_y, int hero_x) {
    int free_cells[TOWER_MAX_MAP_SIZE * TOWER_MAX_MAP_SIZE][2];
    int count = 0;

    for (int i = 1; i < size - 1; ++i)
        for (int j = 1; j < size - 1; ++j)
            if (map[i][j] == '.' && !(i == hero_y && j == hero_x)) {
                free_cells[count][0] = i;
                free_cells[count][1] = j;
                count++;
            }

    if (count > 0) {
        int r = rand() % count;
        map[free_cells[r][0]][free_cells[r][1]] = 'U';
        return;
    }

    for (int i = 1; i < size - 1; ++i)
        for (int j = 1; j < size - 1; ++j)
            if (!(i == hero_y && j == hero_x)) {
                map[i][j] = 'U';
                return;
            }
}

static bool move_hero(char **map, int size, int *y, int *x, char direction) {
    int new_y = *y, new_x = *x;

    switch (direction) {
        case 'W': case 'w': new_y--; break;
        case 'S': case 's': new_y++; break;
        case 'A': case 'a': new_x--; break;
        case 'D': case 'd': new_x++; break;
        default: return false;
    }

    if (new_y < 0 || new_y >= size || new_x < 0 || new_x >= size) {
        printf("Нельзя идти туда!\n");
        return false;
    }

    char cell = map[new_y][new_x];
    if (cell == '#' || cell == 'T' || cell == 'R') {
        printf("Там препятствие (%c)\n", cell);
        return false;
    }

    if (cell == TOWER_ABYSS) {
        printf("Там бездна\n");
        return false;
    }

    *y = new_y;
    *x = new_x;
    return true;
}

static void draw_map(char **map, int size, int hero_y, int hero_x) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == hero_y && j == hero_x) putchar('@');
            else putchar(map[i][j]);
        }
        putchar('\n');
    }
}

static void collapse_perimeter(char **map, int size, int level) {
    int top = level, bottom = size - 1 - level;
    int left = level, right = size - 1 - level;

    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (i == top || i == bottom || j == left || j == right)
                map[i][j] = TOWER_ABYSS;
}

void collapsing_tower(void) {
    char **map;
    int map_size;
    int hero_y, hero_x;
    int floor_level = 1;

    map = generate_map(&map_size, &hero_y, &hero_x);
    if (map == NULL) {
        printf("Ошибка создания карты!\n");
        return;
    }

    map[hero_y][hero_x] = 'D';
    setup_stair_U(map, map_size, hero_y, hero_x);

    int turn_count = 0;
    bool game_over = false;
    int level = 0;
    bool stair_lost = false;

    while (!game_over) {
        draw_map(map, map_size, hero_y, hero_x);

        printf("\nХодов: %d | Этаж: %d\n", turn_count, floor_level);

        if (stair_lost) {
            printf("Башня рушится! ЛЕСТНИЦА U ПРОВАЛИЛАСЬ!\n");
            printf("СРОЧНО эвакуируйся: нажми Q!\n");
        } else {
            printf("W/A/S/D — идти. Q — выйти домой (только стоя на D)\n");
        }
        printf("Твой ход: ");

        char direction;
        if (scanf(" %c", &direction) != 1) {
            clear_input_buffer();
            continue;
        }

        if (direction == 'Q' || direction == 'q') {
            if (stair_lost || map[hero_y][hero_x] == 'D') {
                printf("\nТы вернулся домой\n");
                game_over = true;
                break;
            }
            printf("Ты не у выхода! Найди D.\n");
            continue;
        }

        if (!move_hero(map, map_size, &hero_y, &hero_x, direction)) continue;
        turn_count++;

        if (map[hero_y][hero_x] == 'U') {
            printf("\nТы нашел лестницу! Переходим на следующий этаж...\n");
            floor_level++;

            for (int i = 0; i < map_size; i++) free(map[i]);
            free(map);

            map = generate_map(&map_size, &hero_y, &hero_x);
            if (map == NULL) {
                printf("Критическая ошибка!\n");
                return;
            }

            map[hero_y][hero_x] = 'D';
            setup_stair_U(map, map_size, hero_y, hero_x);

            turn_count = 0;
            level = 0;
            stair_lost = false;

            printf("Этаж %d! Продолжай путь\n\n", floor_level);
            continue;
        }

        if (turn_count % TOWER_COLLAPSE_INTERVAL == 0) {
            collapse_perimeter(map, map_size, level);
            level++;

            bool stair_exists = false;
            for (int i = 0; i < map_size; i++) {
                for (int j = 0; j < map_size; j++)
                    if (map[i][j] == 'U') { stair_exists = true; break; }
                if (stair_exists) break;
            }

            bool exit_exists = false;
            for (int i = 0; i < map_size; i++) {
                for (int j = 0; j < map_size; j++)
                    if (map[i][j] == 'D') { exit_exists = true; break; }
                if (exit_exists) break;
            }

            if (!stair_exists && !exit_exists) {
                printf("\nОбе лестницы провалились! Тебе не выбраться... Нажми Q\n");
                stair_lost = true;
                continue;
            }

            if (!stair_exists && !stair_lost) {
                stair_lost = true;
                printf("\nЛестница U провалилась в бездну!\n");
                printf("Башня рушится! Нажми Q!\n\n");
            } else if (stair_exists) {
                printf("Карта сужается к центру.\n");
            }

            if (map[hero_y][hero_x] == TOWER_ABYSS) {
                printf("Пол исчез! Ты упал в бездну.\n");
                game_over = true;
            }
        }
        printf("\n");
    }

    for (int i = 0; i < map_size; i++) free(map[i]);
    free(map);

    printf("Возврат в главное меню.\n\n");
}
