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
            printf("Ошибка: не удалось выделить память под строки карты\n");
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
    if (m == NULL) return;

    int obstacle_chance = OBSTACLE_MIN + rand() % (OBSTACLE_MAX - OBSTACLE_MIN + 1);

    for (int i = 0; i < m->height; ++i) {
        for (int j = 0; j < m->width; ++j) {
            if (i == 0 || i == m->height - 1 || j == 0 || j == m->width - 1) {
                m->grid[i][j] = TILE_WALL;
            } else {
                int r = rand() % 100;
                if (r < obstacle_chance) {
                    m->grid[i][j] = (rand() % 2 == 0) ? TILE_TREE : TILE_ROCK;
                } else {
                    m->grid[i][j] = TILE_EMPTY;
                }
            }
        }
    }

    int cx = m->width / 2;
    int cy = m->height / 2;
    m->grid[cy][cx] = TILE_EMPTY;

    m->grid[m->height - 2][m->width - 2] = TILE_EXIT;
}

void map_print(const Map *m, const Player *p, const GameTime *t) {
    if (m == NULL || p == NULL || t == NULL) return;

    printf("\nHP: %d/%d | Выносливость: %d/%d | Золото: %d | Этаж: %d | Время: %02d:00",
           p->hp, p->max_hp, p->stamina, p->max_stamina,
           p->gold, m->floor, t->hour);

    if (p->buff.type != STATUS_HEALTHY) {
        printf(" | %s (%d)", buff_name(p->buff.type), p->buff.turns_left);
    }
    printf("\n");

    for (int i = 0; i < m->height; ++i) {
        for (int j = 0; j < m->width; ++j) {
            if (i == p->y && j == p->x) {
                putchar('@');
                continue;
            }
            switch (m->grid[i][j]) {
                case TILE_WALL:  putchar('#'); break;
                case TILE_EMPTY: putchar('.'); break;
                case TILE_TREE:  putchar('T'); break;
                case TILE_ROCK:  putchar('R'); break;
                case TILE_EXIT:  putchar('Q'); break;
                default:         putchar(' '); break;
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
        default:        return 1;
    }

    if (nx < 0 || nx >= m->width || ny < 0 || ny >= m->height) return 1;

    if (m->grid[ny][nx] == TILE_WALL ||
        m->grid[ny][nx] == TILE_TREE ||
        m->grid[ny][nx] == TILE_ROCK) {
        printf("Там препятствие!\n");
        return 1;
    }

    p->x = nx;
    p->y = ny;

    int stamina_cost = STEP_COST;
    if (p->buff.type == STATUS_STRAIN) stamina_cost *= STRAIN_STAMINA_X2;
    if (p->buff.type == STATUS_VIGOR)  stamina_cost = 0;

    if (stamina_cost > 0 && p->stamina > 0) p->stamina -= stamina_cost;
    if (p->stamina <= 0) { p->stamina = 0; return -1; }

    if (p->buff.type == STATUS_POISON) {
        p->hp -= POISON_HP_PER_STEP;
        if (p->hp <= 0) { p->hp = 0; return -1; }
    }

    if (t->hour == 2) return -1;

    buff_tick(&p->buff);

    if (rand() % 100 < BUFF_CHANCE_PERCENT) {
        int r = rand() % 3;
        if (r == 0) {
            buff_apply(&p->buff, STATUS_STRAIN);
            printf("Колючка. Эффект: %s (%d шагов).\n", buff_name(STATUS_STRAIN), BUFF_DURATION_TURNS);
        } else if (r == 1) {
            buff_apply(&p->buff, STATUS_POISON);
            printf("Странная ягода. Эффект: %s (%d шагов).\n", buff_name(STATUS_POISON), BUFF_DURATION_TURNS);
        } else {
            buff_apply(&p->buff, STATUS_VIGOR);
            printf("Странная ягода. Эффект: %s (%d шагов).\n", buff_name(STATUS_VIGOR), BUFF_DURATION_TURNS);
        }
    }

    if (m->grid[ny][nx] == TILE_EXIT) {
        m->floor += 1;
        int reward = GOLD_PER_FLOOR * m->floor;
        p->gold += reward;

        p->stamina -= STAIR_COST;
        if (p->stamina < 0) p->stamina = 0;

        t->hour += 1;
        if (t->hour >= 24) { t->hour -= 24; t->day++; }

        printf("\n>>> Переход на этаж %d! Награда: +%d золота\n", m->floor, reward);

        map_generate(m);
        p->x = m->width / 2;
        p->y = m->height / 2;

        if (p->stamina <= 0) return -1;
        if (t->hour == 2) return -1;
    }
    return 1;
}
