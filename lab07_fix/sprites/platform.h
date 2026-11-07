#ifndef PLATFORM_SPRITE_H
#define PLATFORM_SPRITE_H
#include <stdint.h>

#include "sprite.h"

#define PLATFORM_BITS_PER_PIXEL 1
#define PLATFORM_LENGTH 20      // ceil(10 / 8) bytes per row * 10 rows
#define PLATFORM_W 10
#define PLATFORM_H 10

extern const uint8_t platform[PLATFORM_LENGTH];
extern const sprite_t platform_sprite;

#endif // PLATFORM_SPRITE_H
