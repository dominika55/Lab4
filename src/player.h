#ifndef PLAYER_H
#define PLAYER_H

#define STR_NAME_LEN     33
#define INVENTORY_SIZE   10

#define PLAYER_MAX_HP        100
#define PLAYER_MAX_STAMINA   50
#define PLAYER_START_GOLD    0


typedef enum ItemType {
    ITEM_EMPTY = 0,
    ITEM_WOOD,
    ITEM_STONE,
    ITEM_SEEDS,
    ITEM_BERRIES,
    ITEM_WATER,
    ITEM_SAND,
    ITEM_STICK,
    ITEM_COAL,
    ITEM_BARK,
    ITEM_COUNT         
} ItemType;


typedef struct GameTime {
    int day;
    int hour;
} GameTime;


typedef struct Player {
    char     name[STR_NAME_LEN];
    int      hp;
    int      max_hp;
    int      stamina;
    int      max_stamina;
    int      gold;
    int      x;
    int      y;
    ItemType inventory[INVENTORY_SIZE];
} Player;


Player *player_create(const char *name);
void    player_destroy(Player *p);

GameTime *time_create(void);
void      time_destroy(GameTime *t);
void      time_print(const GameTime *t);
int       time_wait(GameTime *t, int hours);
void      time_sleep(GameTime *t, Player *p);

void player_print_card(const Player *p);
const char *item_name(ItemType t);

void player_print_inventory(const Player *p);
void player_put_item(Player *p);
void player_drop_item(Player *p);
void player_remove_duplicates(Player *p);
void player_search_item(const Player *p);

#endif // PLAYER_H
