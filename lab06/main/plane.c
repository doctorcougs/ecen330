#include <stdlib.h>

#include "config.h"
#include "hw.h"
#include "lcd.h"
#include "missile.h"
#include "plane.h"

// definitions for screen sizes
#define LCD_W HW_LCD_W
#define LCD_H HW_LCD_H
#define QUARTER_LCD_W (LCD_W / 4)
#define PLANE_RANDOM 10

// state types
typedef enum { PLANE_IDLE, PLANE_FLYING } plane_state_t;

// static variables used throughout this file
static plane_state_t state;
static missile_t *missile;
static float x, y;
static bool missile_fired;
static uint32_t idle_ticks;
static float missile_launch_pos;

// Initializes the plane state machine and its missile.
// Arguments: plane_missile - pointer to the missile the plane will launch.
// Returns: nothing.
void plane_init(missile_t *plane_missile) {
	// create the plane's missile
	missile = plane_missile;
	missile_init(missile);
	state = PLANE_IDLE;
	idle_ticks = 0;
	missile_fired = false;
	x = y = 0;
}

// Destroys the plane (it was hit) and restarts its idle countdown.
// Arguments: none.
// Returns: nothing.
void plane_explode(void) {
	state = PLANE_IDLE;
	idle_ticks = 0;
}

// State machine tick for the plane. Transitions are evaluated first, then
// the actions of the (possibly new) state are performed.
// Arguments: none.
// Returns: nothing.
void plane_tick(void) {
	// state transitions
	switch (state) {
		// idle waits until the countdown finishes, then spawns the plane
		case PLANE_IDLE: {
			if (idle_ticks >= CONFIG_PLANE_IDLE_TIME_TICKS) {
				// spawns plane on the side of the screen with varied height
				x = LCD_W + CONFIG_PLANE_WIDTH / 2;
				y = CONFIG_PLANE_HEIGHT + (rand() % PLANE_RANDOM);
				missile_fired = false;
				// determine where the plane will shoot its missile
				missile_launch_pos = QUARTER_LCD_W + (rand() % (LCD_W / 2));
				state = PLANE_FLYING;
			}
			break;
		}
		// flying ends once the plane has flown off the screen
		case PLANE_FLYING: {
			if (x < -(CONFIG_PLANE_WIDTH / 2)) {
				state = PLANE_IDLE;
				idle_ticks = 0;
			}
			break;
		}
		// default case for safety
		default: {
			state = PLANE_IDLE;
			break;
		}
	}

	// state actions
	switch (state) {
		// idle counts up until the plane can fly again
		case PLANE_IDLE: {
			idle_ticks++;
			break;
		}
		// flying moves the plane, launches its missile and draws it
		case PLANE_FLYING: {
			x -= CONFIG_PLANE_DISTANCE_PER_TICK;
			// launch once when we reach our random launch point
			if (!missile_fired && x <= missile_launch_pos) {
				missile_fired = true;
				if (missile_is_idle(missile)) {
					missile_launch_plane(missile, (coord_t)x, (coord_t)y);
				}
			}
			// draw the plane
			lcd_fillTriangle((coord_t)x - CONFIG_PLANE_WIDTH / 2, (coord_t)y,
							 (coord_t)x + CONFIG_PLANE_WIDTH / 2, (coord_t)y - CONFIG_PLANE_HEIGHT / 2,
							 (coord_t)x + CONFIG_PLANE_WIDTH / 2, (coord_t)y + CONFIG_PLANE_HEIGHT / 2,
							 CONFIG_COLOR_PLANE);
			break;
		}
		// default case for safety
		default: {
			break;
		}
	}
}

// Gets the current plane position.
// Arguments: xq, yq - outputs for the plane's x and y position.
// Returns: nothing (the position is returned through *xq and *yq).
void plane_get_pos(coord_t *xq, coord_t *yq) {
	*xq = (coord_t)x;
	*yq = (coord_t)y;
}

// Checks whether the plane is flying.
// Arguments: none.
// Returns: true if the plane is in the flying state, else false.
bool plane_is_flying(void) {
	return state == PLANE_FLYING;
}