#include "player.h"
#include "map.h"
#include "utils.h"
#include "tower.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FORBIDDEN_HOUR      2
#define MIN_STAMINA         1
#define FAINT_GOLD_DIVIDER  2
#define MAX_WAIT_HOURS      12


typedef enum MenuCode {
    MENU_EXIT                = 0,
    MENU_CREATE_PLAYER       = 1,
    MENU_SHOW_CARD           = 2,
    MENU_TIME                = 3,
    MENU_INVENTORY           = 4,
    MENU_PUT_ITEM            = 5,
    MENU_DROP_ITEM           = 6,
    MENU_REMOVE_DUPLICATES   = 7,
    MENU_SEARCH_ITEM         = 8,
    MENU_SAVE_DIARY          = 9,
    MENU_UNPACK_MAP          = 10,
    MENU_ADVENTURE           = 11,
    MENU_TOWER               = 12
} MenuCode;


typedef enum TimeMenuCode {
    TIME_BACK  = 0,
    TIME_SHOW  = 1,
    TIME_WAIT  = 2,
    TIME_SLEEP = 3
} TimeMenuCode;


void menu_create_player(Player **player) {
    if (*player != NULL) {
        int ans = read_int_safe("Персонаж уже существует. Удалить? (1-да, 0-нет): ", 0, 1);
        if (ans == 0) {
            return;
        }
        player_destroy(*player);
        *player = NULL;
    }

    char name[STR_NAME_LEN];
    read_line_safe(name, STR_NAME_LEN, "Введите имя персонажа: ");

    *player = player_create(name);
    if (*player != NULL) {
        printf("Персонаж создан.\n");
    }
}

void menu_show_card(Player *player, GameTime *time) {
    if (player == NULL) {
        printf("Сначала создайте персонажа!\n");
        return;
    }

    player_print_card(player);

    printf("Текущее время: ");
    time_print(time);
}

void menu_time(GameTime *time, Player *player) {
    while (1) {
        printf("\n[1] Посмотреть время\n");
        printf("[2] Ждать\n");
        printf("[3] Лечь спать\n");
        printf("[0] Назад\n");

        TimeMenuCode choice = (TimeMenuCode)read_int_safe("Выбор: ", 0, 3);

        if (choice == TIME_BACK) {
            return;
        }

        if (choice == TIME_SHOW) {
            time_print(time);
        } else if (choice == TIME_WAIT) {
            int hours = read_int_safe("Сколько часов ждать: ", 1, MAX_WAIT_HOURS);
            time_wait(time, hours);
        } else if (choice == TIME_SLEEP) {
            if (player == NULL) {
                printf("Сначала создайте персонажа!\n");
            } else {
                time_sleep(time, player);
            }
        }
    }
}

Direction read_direction(void) {
    char buf[STR_BUF_LEN];
    read_line_safe(buf, STR_BUF_LEN, "WASD — шаг, Q — выйти: ");

    switch (buf[0]) {
        case 'w':
        case 'W':
            return DIR_UP;
        case 's':
        case 'S':
            return DIR_DOWN;
        case 'a':
        case 'A':
            return DIR_LEFT;
        case 'd':
        case 'D':
            return DIR_RIGHT;
        case 'q':
        case 'Q':
            return DIR_QUIT;
        default:
            return DIR_NONE;
    }
}

void menu_adventure(Player *player, GameTime *time) {
    if (player == NULL) {
        printf("Сначала создайте персонажа!\n");
        return;
    }

    if (player->stamina == 0) {
        printf("Нет выносливости для похода!\n");
        return;
    }

    if (time->hour == FORBIDDEN_HOUR) {
        printf("Сейчас 02:00 — фермер спит!\n");
        return;
    }

    Map *map = map_create(MAP_WIDTH, MAP_HEIGHT);
    if (map == NULL) {
        printf("Не удалось создать карту.\n");
        return;
    }

    map_generate(map);
    player->x = 0;
    player->y = 0;

    while (1) {
        map_print(map, player, time);

        Direction dir = read_direction();
        if (dir == DIR_NONE) {
            continue;
        }

        int result = map_step(map, player, time, dir);

        if (result == 0) {
            printf("Штатный выход. Всё золото остаётся при вас.\n");
            break;
        }

        if (result == -1) {
            printf("\nОбморок! Спасатели доставили вас домой.\n");
            player->gold = player->gold / FAINT_GOLD_DIVIDER;

            if (player->stamina == 0) {
                player->stamina = MIN_STAMINA;
            }
            break;
        }
    }

    map_destroy(map);
}


int main(void) {
    srand((unsigned)time(NULL));

    Player *player = NULL;
    GameTime *time = time_create();

    if (time == NULL) {
        printf("Ошибка инициализации времени.\n");
        return 1;
    }

    while (1) {
        printf("[1] Создать/пересоздать персонажа\n");
        printf("[2] Карточка персонажа\n");
        printf("[3] Время\n");
        printf("[4] Посмотреть инвентарь\n");
        printf("[5] Положить предмет в слот\n");
        printf("[6] Выбросить предмет\n");
        printf("[7] Устранение дубликатов\n");
        printf("[8] Поиск предмета по названию\n");
        printf("[9] Записать в дневник фермера\n");
        printf("[10] Распаковать карту\n");
        printf("[11] Отправиться в приключение\n");
        printf("[12] Разрушающаяся башня\n");
        printf("[0] Выход\n");

        MenuCode choice = (MenuCode)read_int_safe("Выбор: ", 0, 12);

        switch (choice) {
            case MENU_EXIT:
                player_destroy(player);
                time_destroy(time);
                printf("До свидания!\n");
                return 0;

            case MENU_CREATE_PLAYER:
                menu_create_player(&player);
                break;

            case MENU_SHOW_CARD:
                menu_show_card(player, time);
                break;

            case MENU_TIME:
                menu_time(time, player);
                break;

            case MENU_INVENTORY:
                if (player == NULL) {
                    printf("Сначала создайте персонажа!\n");
                } else {
                    player_print_inventory(player);
                }
                break;

            case MENU_PUT_ITEM:
                if (player == NULL) {
                    printf("Сначала создайте персонажа!\n");
                } else {
                    player_put_item(player);
                }
                break;

            case MENU_DROP_ITEM:
                if (player == NULL) {
                    printf("Сначала создайте персонажа!\n");
                } else {
                    player_drop_item(player);
                }
                break;

            case MENU_REMOVE_DUPLICATES:
                if (player == NULL) {
                    printf("Сначала создайте персонажа!\n");
                } else {
                    player_remove_duplicates(player);
                }
                break;

            case MENU_SEARCH_ITEM:
                if (player == NULL) {
                    printf("Сначала создайте персонажа!\n");
                } else {
                    player_search_item(player);
                }
                break;

            case MENU_SAVE_DIARY:
                if (player == NULL) {
                    printf("Сначала создайте персонажа!\n");
                } else {
                    save_diary(player, time);
                }
                break;

            case MENU_UNPACK_MAP:
                unpack_map();
                break;

            case MENU_ADVENTURE:
                menu_adventure(player, time);
                break;

            case MENU_TOWER:
                if (player == NULL) {
                    printf("Сначала создайте персонажа!\n");
                } else {
                    collapsing_tower();
                }
                break;

            default:
                printf("Такого пункта нет.\n");
                break;
        }
    }
}
