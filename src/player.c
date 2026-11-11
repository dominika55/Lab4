#include "player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    int start[INVENTORY_SIZE] = {
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
