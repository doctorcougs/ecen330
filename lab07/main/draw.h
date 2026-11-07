#ifndef DRAW_H
#define DRAW_H

#include <stdbool.h>
#include <stdint.h>

#include "lcd.h"

void better_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, color_t color);
void better_rect_fill(uint32_t x, uint32_t y, uint32_t w, uint32_t h, color_t color);

void better_text(uint32_t x, uint32_t y, const char *text, color_t color);

#endif // DRAW_H