#include <stdio.h>
#include <stdlib.h> // rand

#include "config.h"
#include "cursor.h"
#include "game.h"
#include "hw.h"
#include "lcd.h"
#include "missile.h"
#include "pin.h"
#include "plane.h"
#include "sound.h"

// sound
#include "missileLaunch.h"

// screen locations and volume level
#define STATS_Y 2
#define STATS_SHOT_X 4
#define STATS_IMPACT_X (HW_LCD_W / 2)
#define GAME_VOLUME 80
#define BUFFER_SIZE 32

// hold all of our missiles
static missile_t missiles[CONFIG_MAX_TOTAL_MISSILES];

// holds the pointers to our missile variables, incremented to assign pointers
static missile_t *enemy_missiles = missiles + 0;
static missile_t *player_missiles = missiles + CONFIG_MAX_ENEMY_MISSILES;
static missile_t *plane_missile = missiles + CONFIG_MAX_ENEMY_MISSILES + CONFIG_MAX_PLAYER_MISSILES;

// stats counter
static uint32_t shot_count;
static uint32_t impacted_count;
static bool btn_prev;

// Checks whether a position is inside the explosion of any missile.
// Arguments: x, y - the position to test.
// Returns: true if any exploding missile is colliding with (x, y), else false.
static bool is_hit_by_explosion(coord_t x, coord_t y) {
	// compare the position against every missile's explosion
	for (uint32_t i = 0; i < CONFIG_MAX_TOTAL_MISSILES; i++) {
		if (missile_is_colliding(missiles + i, x, y)) {
			return true;
		}
	}
	return false;
}

// Initializes all missiles, the plane, the stats and the sound volume.
// Arguments: none.
// Returns: nothing.
void game_init(void) {
	// initialize all missiles
	for (uint32_t i = 0; i < CONFIG_MAX_TOTAL_MISSILES; i++) {
		missile_init(missiles + i);
	}
	// initializes plane
	plane_init(plane_missile);
	// reset score counters
	shot_count = 0;
	impacted_count = 0;
	btn_prev = false;
	sound_set_volume(GAME_VOLUME);
}

// Runs one tick of the game: ticks missiles and plane, launches enemy and
// player missiles, detects collisions, and draws the statistics.
// Arguments: none.
// Returns: nothing.
void game_tick(void) {
	// tick all missiles and the plane
	for (uint32_t i = 0; i < CONFIG_MAX_TOTAL_MISSILES; i++) {
		missile_tick(missiles + i);
	}
	plane_tick();

	// check to see if the missile is idle for enemies and shoots one
	for (uint32_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++) {
		if (missile_is_idle(enemy_missiles + i)) {
			missile_launch_enemy(enemy_missiles + i);
		}
	}

	// only one button is necessary for this game
	bool btn = (pin_get_level(HW_BTN_A) == 0) || (pin_get_level(HW_BTN_B) == 0);
	// stop from double clicking
	if (btn && !btn_prev) {
		// cycle through player missiles
		for (uint32_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++) {
			// check to see if the missile slot is idle
			if (missile_is_idle(player_missiles + i)) {
				// grab pos of cursor to shoot a missile
				coord_t cx, cy;
				cursor_get_pos(&cx, &cy);
				missile_launch_player(player_missiles + i, cx, cy);
				// play sound
				sound_start(missileLaunch, MISSILELAUNCH_SAMPLES, false);
				shot_count++;
				break;
			}
		}
	}
	btn_prev = btn;

	// cycle through enemy missiles and check their collisions
	for (uint32_t t = 0; t < CONFIG_MAX_ENEMY_MISSILES; t++) {
		missile_t *target = enemy_missiles + t;
		// skip missile if it is not moving
		if (!missile_is_moving(target)) {
			continue;
		}
		coord_t x, y;
		missile_get_pos(target, &x, &y);
		// missile explodes if it is inside an explosion
		if (is_hit_by_explosion(x, y)) {
			missile_explode(target);
		}
	}
	// check the plane missile the same way as the enemy missiles
	if (missile_is_moving(plane_missile)) {
		coord_t x, y;
		missile_get_pos(plane_missile, &x, &y);
		// explode the plane missile if it is inside an explosion
		if (is_hit_by_explosion(x, y)) {
			missile_explode(plane_missile);
		}
	}

	// check to see if we hit the plane to make it explode
	if (plane_is_flying()) {
		coord_t x, y;
		plane_get_pos(&x, &y);
		// explode the plane if it is inside an explosion
		if (is_hit_by_explosion(x, y)) {
			plane_explode();
		}
	}

	// count all impacted enemy missiles
	for (uint32_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++) {
		if (missile_is_impacted(enemy_missiles + i)) {
			impacted_count++;
		}
	}
	// count the plane missile
	if (missile_is_impacted(plane_missile)) {
		impacted_count++;
	}

	// sets our onscreen text for the scores
	char buf[BUFFER_SIZE];
	lcd_setFontSize(1);
	lcd_noFontBackground();
	snprintf(buf, sizeof(buf), "Shot: %lu", (unsigned long)shot_count);
	lcd_drawString(STATS_SHOT_X, STATS_Y, buf, CONFIG_COLOR_STATUS);
	snprintf(buf, sizeof(buf), "Impacted: %lu", (unsigned long)impacted_count);
	lcd_drawString(STATS_IMPACT_X, STATS_Y, buf, CONFIG_COLOR_STATUS);
}