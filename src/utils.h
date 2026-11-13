#ifndef UTILS_H
#define UTILS_H

#include "player.h"

#define STR_BUF_LEN 128

int  read_int_safe(const char *prompt, int min, int max);
void read_line_safe(char *buf, int size, const char *prompt);
void clear_input_buffer(void);

void save_diary(const Player *p, const GameTime *t);
void unpack_map(void);

#endif // UTILS_H
