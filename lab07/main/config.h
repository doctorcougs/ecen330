#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_GAME_TIMER_PERIOD 40.0E-3f
#define CONFIG_COLOR_BACKGROUND BLACK

//state machine for main
typedef enum {
    MAIN_IDLE,
	MAIN_START,
    MAIN_OPTIONS,
    MAIN_QUIT,
} main_state_t;

//defines used for joystick
#define OV 30 // Over scale by a margin
#define A2X(d) (((d)*(LCD_W/2+OV))/JOY_MAX_DISP+(LCD_W/2))
#define A2Y(d) (((d)*(LCD_H/2+OV))/JOY_MAX_DISP+(LCD_H/2))
#define CLIP(x,lo,hi) ((x) < (lo) ? (lo) : ((x) > (hi) ? (hi) : (x)))

#define FONT_SIZE 2
#define FONT_W (LCD_CHAR_W*FONT_SIZE)
#define FONT_H (LCD_CHAR_H*FONT_SIZE)

#define MEDIUM_BOX_W 100
#define MEDIUM_BOX_H 30
#define MEDIUM_BOX_SPACING 5

#define TILE_SIZE 10

#define CHARACTER_SIZE 10
#define HALF_CHARACTER_SIZE (CHARACTER_SIZE / 2)
#define PLAYER_STARTING_IFRAMES_CAP 10
#define PLAYER_STARTING_HEALTH 2

#define CHARACTER_ACCEL_PER_TICK 0.4f
#define CHARACTER_JUMP_PER_TICK  5.f
#define GRAVITY_ACCEL_PER_TICK   0.5f
#define MAX_CHAR_SPEED           5.0f  
#define FRICTION                 0.85f  

#define JOYSTICK_SELECT_SENSE 500
#define JOYSTICK_MOVE_SENSE 500


#define HALF_SCREEN_W (LCD_W / 2)
#define HALF_SCREEN_H (LCD_H / 2)

#endif // CONFIG_H