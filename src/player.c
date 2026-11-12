#include "player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DAY_START_HOUR   8   
#define FORBIDDEN_HOUR   2 
#define HOURS_IN_DAY     24


Player *player_create(const char *name) {
    Player *p = (Player *)malloc(sizeof(Player));
    if (p == NULL) {
        printf("Ошибка: не удалось выделить память под игрока\n");
        return NULL;
    }

    strncpy(p->name, name ? name : "Фермер", STR_NAME_LEN - 1); 
    p->name[STR_NAME_LEN - 1] = '\0'; 

    p->hp          = PLAYER_MAX_HP;
    p->max_hp      = PLAYER_MAX_HP;
    p->stamina     = PLAYER_MAX_STAMINA;
    p->max_stamina = PLAYER_MAX_STAMINA;
    p->gold        = PLAYER_START_GOLD;
    p->x           = 0;
    p->y           = 0;

    ItemType start[INVENTORY_SIZE] = {
        ITEM_EMPTY, ITEM_WOOD, ITEM_STONE, ITEM_SEEDS,
        ITEM_BERRIES, ITEM_WATER, ITEM_SAND, ITEM_STICK,
        ITEM_COAL, ITEM_BARK
    };
    memcpy(p->inventory, start, sizeof(start));

    return p;
}

void player_destroy(Player *p) {
    if (p == NULL) return;
    printf("Память структуры по адресу %p освобождена\n", (void *)p);
    free(p);
}


GameTime *time_create(void) {
    GameTime *t = (GameTime *)malloc(sizeof(GameTime));
    if (t == NULL) {
        printf("Ошибка: не удалось выделить память под время\n");
        return NULL;
    }
    t->day  = 1;
    t->hour = DAY_START_HOUR;
    return t;
}


void time_destroy(GameTime *t) {
    if (t == NULL) return;
    printf("Память структуры по адресу %p освобождена\n", (void *)t);
    free(t);
}


void time_print(const GameTime *t) {
    if (t == NULL) return;
    printf("День %d, %02d:00\n", t->day, t->hour);
}


int time_wait(GameTime *t, int hours) {
    if (t == NULL || hours <= 0) return 0;

    int target = t->hour + hours;

    if (t->hour < FORBIDDEN_HOUR) {
        if (target >= FORBIDDEN_HOUR) {
            printf("Нельзя пройти границу 02:00 без сна!\n");
            return 0;
        }
    } else {
        int to_next = (HOURS_IN_DAY - t->hour) + FORBIDDEN_HOUR;
        if (hours >= to_next) {
            printf("Нельзя пройти границу 02:00 без сна!\n");
            return 0;
        }
    }

    t->hour += hours;
    while (t->hour >= HOURS_IN_DAY) {
        t->hour -= HOURS_IN_DAY;
        t->day++;
    }
    printf("Время: день %d, %02d:00\n", t->day, t->hour);
    return 1;
}


void time_sleep(GameTime *t, Player *p) {
    if (t == NULL) return;
    t->day  += 1;
    t->hour  = DAY_START_HOUR;
    if (p != NULL) {
        p->hp      = p->max_hp;
        p->stamina = p->max_stamina;
    }
    printf("Вы поспали. День %d, %02d:00.\n", t->day, t->hour);
    if (p != NULL) printf("HP и выносливость полностью восстановлены.\n");
}


const char *item_name(ItemType t) {
    switch (t) {
        case ITEM_EMPTY:   return "пусто";
        case ITEM_WOOD:    return "дерево";
        case ITEM_STONE:   return "камень";
        case ITEM_SEEDS:   return "семена";
        case ITEM_BERRIES: return "ягоды";
        case ITEM_WATER:   return "вода";
        case ITEM_SAND:    return "песок";
        case ITEM_STICK:   return "палка";
        case ITEM_COAL:    return "уголь";
        case ITEM_BARK:    return "кора";
        default:           return "неизвестный предмет";
    }
}

void player_print_card(const Player *p) {
    if (p == NULL) {
        printf("Персонаж не создан.\n");
        return;
    }

    printf("\n Карточка персонажа \n");
    printf("Имя:          %s\n", p->name);
    printf("HP:           %d/%d\n", p->hp, p->max_hp);
    printf("Выносливость: %d/%d\n", p->stamina, p->max_stamina);
    printf("Золото:       %d\n", p->gold);
    printf("Позиция:      (%d, %d)\n", p->x, p->y);
    printf("Инвентарь:\n");
    for (int i = 0; i < INVENTORY_SIZE; ++i) {
        printf("  [%d] %s\n", i, item_name(p->inventory[i]));
    }
}
