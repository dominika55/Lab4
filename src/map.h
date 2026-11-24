#ifndef MAP_H
#define MAP_H

#include "player.h"

#define MAP_WIDTH       10
#define MAP_HEIGHT      10
#define GOLD_PER_FLOOR  10
#define STAIR_COST      5
#define STEP_COST       1

#define OBSTACLE_MIN    15
#define OBSTACLE_MAX    30


typedef enum TileType {
    TILE_WALL = 0,
    TILE_EMPTY,
    TILE_TREE,
    TILE_ROCK,
    TILE_EXIT,
    TILE_COUNT
} TileType;


typedef enum Direction {
    DIR_NONE = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_QUIT
} Direction;


typedef struct Map {
    TileType **grid;
    int        width;
    int        height;
    int        floor;
} Map;


Map  *map_create(int width, int height);
void  map_destroy(Map *m);
void  map_generate(Map *m);
void  map_print(const Map *m, const Player *p, const GameTime *t);
int   map_step(Map *m, Player *p, GameTime *t, Direction dir);

#endif // MAP_H
