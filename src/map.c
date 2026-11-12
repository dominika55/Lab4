#include "map.h"
#include <stdio.h>
#include <stdlib.h>

Map *map_create(int width, int height) {
    Map *m = (Map *)malloc(sizeof(Map));
    if (m == NULL) {
        printf("Ошибка: не удалось выделить память под карту\n");
        return NULL;
    }

    m->grid = (TileType **)malloc(height * sizeof(TileType *));
    if (m->grid == NULL) {
        printf("Ошибка: не удалось выделить память под строки карты\n");
        free(m);
        return NULL;
    }

    for (int i = 0; i < height; ++i) {
        m->grid[i] = (TileType *)calloc(width, sizeof(TileType));
        if (m->grid[i] == NULL) {
            printf("Ошибка: не удалось выделить память под столбцы карты\n");
            for (int j = 0; j < i; ++j) free(m->grid[j]);
            free(m->grid);
            free(m);
            return NULL;
        }
    }

    m->width  = width;
    m->height = height;
    m->floor  = 1;
    return m;
}

void map_destroy(Map *m) {
    if (m == NULL) return;
    for (int i = 0; i < m->height; ++i) free(m->grid[i]);
    free(m->grid);
    printf("Память структуры по адресу %p освобождена\n", (void *)m);
    free(m);
}

void map_generate(Map *m) {
    if (m == NULL) {
        return;
    }

    for (int i = 0; i < m->height; ++i) {
        for (int j = 0; j < m->width; ++j) {
            int r = rand() % 100;
            if (r < 70) {
                m->grid[i][j] = TILE_GRASS;
            } else if (r < 90) {
                m->grid[i][j] = TILE_FOREST;
            } else {
                m->grid[i][j] = TILE_WATER;
            }
        }
    }

    m->grid[0][0] = TILE_GRASS;
    m->grid[m->height - 1][m->width - 1] = TILE_EXIT;
}

void map_print(const Map *m, const Player *p, const GameTime *t) {
    if (m == NULL || p == NULL || t == NULL) return;

    printf("\nHP: %d/%d | Выносливость: %d/%d | Золото: %d | Этаж: %d | Время: %02d:00\n",
           p->hp, p->max_hp, p->stamina, p->max_stamina,
           p->gold, m->floor, t->hour);

    for (int i = 0; i < m->height; ++i) {
        for (int j = 0; j < m->width; ++j) {
            if (i == p->y && j == p->x) { putchar('@'); continue; }
            switch (m->grid[i][j]) {
                case TILE_GRASS:  putchar('.'); break;
                case TILE_FOREST: putchar('#'); break;
                case TILE_WATER:  putchar('~'); break;
                case TILE_EXIT:   putchar('Q'); break;
                default:          putchar(' '); break;
            }
        }
        putchar('\n');
    }
}

int map_step(Map *m, Player *p, GameTime *t, Direction dir) {
    if (m == NULL || p == NULL || t == NULL) return 0;
    if (dir == DIR_QUIT) return 0;

    int nx = p->x, ny = p->y;
    switch (dir) {
        case DIR_UP:    ny--; break;
        case DIR_DOWN:  ny++; break;
        case DIR_LEFT:  nx--; break;
        case DIR_RIGHT: nx++; break;
        default: return 1;
    }

    if (nx < 0 || nx >= m->width || ny < 0 || ny >= m->height) return 1;

    if (m->grid[ny][nx] == TILE_WATER) {
        printf("Вода! Туда нельзя.\n");
        return 1;
    }

    p->x = nx;
    p->y = ny;

    if (p->stamina > 0) p->stamina -= STEP_COST;
    if (p->stamina <= 0) { p->stamina = 0; return -1; }

    if (t->hour == 2) return -1;

    if (m->grid[ny][nx] == TILE_EXIT) {
        m->floor += 1;
        int reward = GOLD_PER_FLOOR * m->floor;
        p->gold += reward;

        p->stamina -= STAIR_COST;
        if (p->stamina < 0) {
            p->stamina = 0;
        }

        t->hour += 1;
        if (t->hour >= 24) { t->hour -= 24; t->day++; }

        printf("\n>>> Переход на этаж %d! Награда: +%d золота\n", m->floor, reward);

        map_generate(m);
        p->x = 0;
        p->y = 0;

        if (p->stamina <= 0) return -1;
        if (t->hour == 2) return -1;
    }
    return 1;
}
