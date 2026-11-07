#ifndef UI_H
#define UI_H

// Screen / menu layout and joystick->screen helpers.

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

#define JOYSTICK_SELECT_SENSE 500

#define HALF_SCREEN_W (LCD_W / 2)
#define HALF_SCREEN_H (LCD_H / 2)

#endif // UI_H
