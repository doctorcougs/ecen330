#include "missile.h"
#include "config.h"
#include "missileLaunch.h"
// for sqrtf
#include <math.h>
#include <stdio.h>
// for rand
#include <stdlib.h>

// defines for player shoot positions and screen size
#define LCD_W HW_LCD_W
#define LCD_H HW_LCD_H
#define PLAYER_SHOOT_POS1 (LCD_W / 4)
#define PLAYER_SHOOT_POS2 (LCD_W / 2)
#define PLAYER_SHOOT_POS3 ((LCD_W / 4) * 3)
#define NUMBER_OF_POSITIONS 3

// state definition
typedef enum {
	MISSILE_IDLE,
	MISSILE_MOVING,
	MISSILE_EXPLODING,
	MISSILE_FADING,
	MISSILE_IMPACTED,
} missile_state_t;

// put these into an array to access easier
static const int16_t player_launch_pos[NUMBER_OF_POSITIONS] = {PLAYER_SHOOT_POS1, PLAYER_SHOOT_POS2,
															   PLAYER_SHOOT_POS3};

// Sets a missile to its idle state and clears its parameters.
// Arguments: missile - pointer to the missile to initialize.
// Returns: nothing.
void missile_init(missile_t *missile) {
	// set all parameters to default
	missile->currentState = MISSILE_IDLE;
	missile->launch = false;
	missile->explode_me = false;
	missile->radius = 0;
	missile->length = 0;
	missile->total_length = 0;
}

// Computes the straight-line distance between two points.
// Arguments: Y1, Y2 - y coordinates of the points; X1, X2 - x coordinates.
// Returns: the distance between (X1, Y1) and (X2, Y2).
static float get_distance_mag(float Y1, float Y2, float X1, float X2) {
	// math from the learningsuite
	float x = X2 - X1;
	float y = Y2 - Y1;
	return sqrtf(x * x + y * y);
}

// Sets up a missile to be ready for launch; used for all missile types.
// Arguments: missile - the missile to set up; x_dest, y_dest - destination;
//            x_origin, y_origin - starting point.
// Returns: nothing.
static void finalize_launch_setup(missile_t *missile, coord_t x_dest, coord_t y_dest,
								  coord_t x_origin, coord_t y_origin) {
	// set all parameters to be ready for launch
	missile->length = 0;
	missile->launch = true;
	missile->explode_me = false;
	missile->total_length = get_distance_mag(y_origin, y_dest, x_origin, x_dest);
	missile->x_origin = x_origin;
	missile->y_origin = y_origin;
	missile->x_current = x_origin;
	missile->y_current = y_origin;
	missile->x_dest = x_dest;
	missile->y_dest = y_dest;
	missile->currentState = MISSILE_MOVING;
}

// Launches a player missile from the firing location (one of three along the
// bottom of the screen) nearest to the destination.
// Arguments: missile - the missile to launch; x_dest, y_dest - destination.
// Returns: nothing.
void missile_launch_player(missile_t *missile, coord_t x_dest, coord_t y_dest) {
	// player must compute distance early to figure out best shoot position
	missile->type = MISSILE_TYPE_PLAYER;
	float distance_from_launch = 0;
	uint8_t best_pos = 0;
	for (uint8_t i = 0; i < NUMBER_OF_POSITIONS; i++) {
		// player decides where to shoot from
		float temp_distance_from_launch = get_distance_mag(LCD_H, y_dest, player_launch_pos[i], x_dest);
		if (distance_from_launch == 0 || distance_from_launch > temp_distance_from_launch) {
			// selects the best position
			distance_from_launch = temp_distance_from_launch;
			best_pos = i;
		}
	}
	finalize_launch_setup(missile, x_dest, y_dest, player_launch_pos[best_pos], LCD_H);
}

// Launches an enemy missile with a random origin along the top of the screen
// and a random destination along the bottom.
// Arguments: missile - the missile to launch.
// Returns: nothing.
void missile_launch_enemy(missile_t *missile) {
	// set type, and random location for firing and hitting
	missile->type = MISSILE_TYPE_ENEMY;
	coord_t origin_x = (rand() % LCD_W);
	coord_t dest_x = (rand() % LCD_W);
	finalize_launch_setup(missile, dest_x, LCD_H, origin_x, 0);
}

// Launches a plane missile from the plane's position to a random destination
// along the bottom of the screen.
// Arguments: missile - the missile to launch; x_orig, y_orig - plane position.
// Returns: nothing.
void missile_launch_plane(missile_t *missile, coord_t x_orig, coord_t y_orig) {
	// set random target, however starting point is on plane
	missile->type = MISSILE_TYPE_PLANE;
	coord_t dest_x = (rand() % LCD_W);
	finalize_launch_setup(missile, dest_x, LCD_H, x_orig, y_orig);
}

// Requests that a moving missile explode on the next tick.
// Arguments: missile - the missile to detonate.
// Returns: nothing.
void missile_explode(missile_t *missile) {
	// error check to make sure the missile is moving
	if (missile->currentState != MISSILE_MOVING) {
		return;
	}
	missile->explode_me = true;
}

// Looks up the draw color for a missile based on its type, so missile_tick
// can handle all types of missiles.
// Arguments: missile - the missile to look up; color - output for the color.
// Returns: nothing (the color is returned through *color).
static void missile_get_color(missile_t *missile, color_t *color) {
	// checks missile type and assigns a color
	if (missile->type == MISSILE_TYPE_PLAYER) {
		*color = CONFIG_COLOR_PLAYER_MISSILE;
	} else if (missile->type == MISSILE_TYPE_ENEMY) {
		*color = CONFIG_COLOR_ENEMY_MISSILE;
	} else {
		*color = CONFIG_COLOR_PLANE_MISSILE;
	}
}

// State machine tick for one missile. Transitions are evaluated first, then
// the actions of the (possibly new) state are performed.
// Arguments: missile - the missile to tick.
// Returns: nothing.
void missile_tick(missile_t *missile) {
	// state transitions
	switch (missile->currentState) {
		// idle stays idle until a launch function starts the missile
		case MISSILE_IDLE: {
			break;
		}
		// moving ends by exploding (hit or player arrival) or impacting
		case MISSILE_MOVING: {
			if (missile->explode_me) {
				// state change to missile explode
				missile->explode_me = false;
				missile->radius = 0;
				missile->currentState = MISSILE_EXPLODING;
			} else if (missile->length >= missile->total_length) {
				// once the missile hits its max length, player missiles explode
				if (missile->type == MISSILE_TYPE_PLAYER) {
					missile->radius = 0;
					missile->currentState = MISSILE_EXPLODING;
				} else {
					missile->currentState = MISSILE_IMPACTED;
				}
			}
			break;
		}
		// exploding ends once the explosion reaches its max radius
		case MISSILE_EXPLODING: {
			if (missile->radius >= CONFIG_EXPLOSION_MAX_RADIUS) {
				missile->currentState = MISSILE_FADING;
			}
			break;
		}
		// fading ends back at idle once we have shrunk all the way
		case MISSILE_FADING: {
			if (missile->radius <= 0) {
				missile->launch = false;
				missile->currentState = MISSILE_IDLE;
			}
			break;
		}
		// impacted missiles go straight back to idle
		case MISSILE_IMPACTED: {
			missile->launch = false;
			missile->currentState = MISSILE_IDLE;
			break;
		}
		// default case for safety
		default: {
			missile->currentState = MISSILE_IDLE;
			break;
		}
	}

	// state actions
	switch (missile->currentState) {
		// idle and impacted missiles do nothing
		case MISSILE_IDLE:
		case MISSILE_IMPACTED: {
			break;
		}
		// moving computes and draws the movement of the missile
		case MISSILE_MOVING: {
			// set variables dependent on missile type
			float added_length;
			if (missile->type == MISSILE_TYPE_PLAYER) {
				added_length = CONFIG_PLAYER_MISSILE_DISTANCE_PER_TICK;
			} else {
				added_length = CONFIG_ENEMY_MISSILE_DISTANCE_PER_TICK;
			}
			color_t color;
			missile_get_color(missile, &color);
			// length math
			missile->length += added_length;
			float length_fraction = missile->length / missile->total_length;
			// moves the missile
			missile->x_current = missile->x_origin + length_fraction * (missile->x_dest - missile->x_origin);
			missile->y_current = missile->y_origin + length_fraction * (missile->y_dest - missile->y_origin);
			// draws the missile
			lcd_drawLine(missile->x_origin, missile->y_origin, missile->x_current, missile->y_current, color);
			break;
		}
		// exploding grows and draws the explosion
		case MISSILE_EXPLODING: {
			missile->radius += CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK;
			color_t color;
			missile_get_color(missile, &color);
			lcd_fillCircle(missile->x_current, missile->y_current, missile->radius, color);
			break;
		}
		// fading shrinks the explosion and draws it while it is still visible
		case MISSILE_FADING: {
			color_t color;
			missile_get_color(missile, &color);
			missile->radius -= CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK;
			if (missile->radius <= 0) {
				// clamp so a negative radius never produces a collision area
				missile->radius = 0;
			} else {
				lcd_fillCircle(missile->x_current, missile->y_current, missile->radius, color);
			}
			break;
		}
		// default case for safety
		default: {
			break;
		}
	}
}

// Gets the current position of a missile.
// Arguments: missile - the missile to query; x, y - outputs for the position.
// Returns: nothing (the position is returned through *x and *y).
void missile_get_pos(missile_t *missile, coord_t *x, coord_t *y) {
	*x = missile->x_current;
	*y = missile->y_current;
}

// Gets the type of a missile.
// Arguments: missile - the missile to query.
// Returns: the missile's type.
missile_type_t missile_get_type(missile_t *missile) {
	return missile->type;
}

// Checks whether a missile is moving.
// Arguments: missile - the missile to query.
// Returns: true if the missile is in the moving state, else false.
bool missile_is_moving(missile_t *missile) {
	return missile->currentState == MISSILE_MOVING;
}

// Checks whether a missile is exploding (including fading out).
// Arguments: missile - the missile to query.
// Returns: true if the missile is exploding or fading, else false.
bool missile_is_exploding(missile_t *missile) {
	return missile->currentState == MISSILE_EXPLODING || missile->currentState == MISSILE_FADING;
}

// Checks whether a missile is idle.
// Arguments: missile - the missile to query.
// Returns: true if the missile is idle, else false.
bool missile_is_idle(missile_t *missile) {
	return missile->currentState == MISSILE_IDLE;
}

// Checks whether a missile has impacted the ground.
// Arguments: missile - the missile to query.
// Returns: true if the missile is in the impacted state, else false.
bool missile_is_impacted(missile_t *missile) {
	return missile->currentState == MISSILE_IMPACTED;
}

// Checks whether a position is inside a missile's explosion.
// Arguments: missile - the missile whose explosion is tested; x, y - the position.
// Returns: true if the missile is exploding and (x, y) is within its radius.
bool missile_is_colliding(missile_t *missile, coord_t x, coord_t y) {
	// if the missile is not exploding there is nothing to collide with
	if (!missile_is_exploding(missile)) {
		return false;
	}
	// if pos is inside of the missile explosion it collides
	float xq = (float)x - missile->x_current;
	float yq = (float)y - missile->y_current;
	return (xq * xq + yq * yq) <= (missile->radius * missile->radius);
}