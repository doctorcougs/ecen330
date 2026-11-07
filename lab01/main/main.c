#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "lcd.h"
#include "pac.h"

static const char *TAG = "lab01";

#define DELAY_MS(ms) \
	vTaskDelay(((ms)+(portTICK_PERIOD_MS-1))/portTICK_PERIOD_MS)

//----------------------------------------------------------------------------//
// Car Implementation - Begin
//----------------------------------------------------------------------------//

// Car constants
#define CAR_CLR rgb565(220,30,0)
#define WINDOW_CLR rgb565(180,210,238)
#define TIRE_CLR BLACK
#define HUB_CLR GRAY

#define CAR_W 60
#define CAR_H 32

#define BODY_X0 0
#define BODY_Y0 12
#define BODY_X1 59
#define BODY_Y1 24

#define WHEEL_RAD 4
#define TIRE_RAD 7
#define REAR_WHEEL_X 11
#define FRONT_WHEEL_X 48
#define WHEEL_Y 24

#define CAB_X0 1
#define CAB_Y0 0
#define CAB_X1 39
#define CAB_Y1 11

#define HOOD_X0 40
#define HOOD_Y0 9
#define HOOD_X1 40
#define HOOD_Y1 11
#define HOOD_X2 59
#define HOOD_Y2 11

#define REAR_WINDOW_X0 3
#define REAR_WINDOW_Y0 1
#define REAR_WINDOW_X1 18
#define REAR_WINDOW_Y1 8
#define FRONT_WINDOW_X0 21
#define FRONT_WINDOW_Y0 1
#define FRONT_WINDOW_X1 37
#define FRONT_WINDOW_Y1 8
#define WINDOW_RAD 2

// TODO: Finish car part constants

/**
 * @brief Draw a car at the specified location.
 * @param x      Top left corner X coordinate.
 * @param y      Top left corner Y coordinate.
 * @details Draw the car components relative to the anchor point (top, left).
 */
void drawCar(coord_t x, coord_t y)
{
	// drawing body of car
	lcd_fillRect2(BODY_X0 + x, BODY_Y0 + y, BODY_X1 + x, BODY_Y1 + y, CAR_CLR);
	// drawing cab of car
	lcd_fillRect2(CAB_X0 + x, CAB_Y0 + y, CAB_X1 + x, CAB_Y1 + y, CAR_CLR);
	// drawing windows of car
	lcd_fillRoundRect2(REAR_WINDOW_X0 + x, REAR_WINDOW_Y0 + y, REAR_WINDOW_X1 + x, REAR_WINDOW_Y1 + y, WINDOW_RAD, WINDOW_CLR);
	lcd_fillRoundRect2(FRONT_WINDOW_X0 + x, FRONT_WINDOW_Y0 + y, FRONT_WINDOW_X1 + x, FRONT_WINDOW_Y1 + y, WINDOW_RAD, WINDOW_CLR);
	// drawing hood of car
	lcd_fillTriangle(HOOD_X0 + x, HOOD_Y0 + y, HOOD_X1 + x, HOOD_Y1 + y, HOOD_X2 + x, HOOD_Y2 + y, CAR_CLR);
	// drawing wheels of car
	lcd_fillCircle(REAR_WHEEL_X + x, WHEEL_Y + y, TIRE_RAD, TIRE_CLR);
	lcd_fillCircle(REAR_WHEEL_X + x, WHEEL_Y + y, WHEEL_RAD, HUB_CLR);
	lcd_fillCircle(FRONT_WHEEL_X + x, WHEEL_Y + y, TIRE_RAD, TIRE_CLR);
	lcd_fillCircle(FRONT_WHEEL_X + x, WHEEL_Y + y, WHEEL_RAD, HUB_CLR);
}

//----------------------------------------------------------------------------//
// Car Implementation - End
//----------------------------------------------------------------------------//

// Main display constants
#define BACKGROUND_CLR rgb565(0,60,90)
#define TITLE_CLR GREEN
#define STATUS_CLR WHITE
#define YELLOW rgb565(255, 255, 0)
#define STR_BUF_LEN 12 // string buffer length
#define FONT_SIZE 2
#define FONT_W (LCD_CHAR_W*FONT_SIZE)
#define FONT_H (LCD_CHAR_H*FONT_SIZE)
#define STATUS_W (FONT_W*3)

#define WAIT 2000 // milliseconds
#define DELAY_EX3 20 // milliseconds

// Object position and movement
#define OBJ_X 100
#define OBJ_Y 100
#define OBJ_MOVE 3 // pixels

// LCD width and height
#define LCD_W HW_LCD_W
#define LCD_H HW_LCD_H

// Places an integer into a character array
void intToString(coord_t x, char *str) 
{
	sprintf(str, "%3ld", x);
}

// Application main
void app_main(void)
{
	ESP_LOGI(TAG, "Start up");
	lcd_init();
	lcd_fillScreen(BACKGROUND_CLR);
	lcd_setFontSize(FONT_SIZE);
	lcd_drawString(0, 0, "Hello World! (lcd)", TITLE_CLR);
	printf("Hello World! (terminal)\n");
	DELAY_MS(WAIT);

	// TODO: Exercise 1 - Draw car in one location.
	lcd_fillScreen(BACKGROUND_CLR);
	lcd_drawString(0, 0, "Exercise 1", TITLE_CLR);
	drawCar(OBJ_X, OBJ_Y);

	DELAY_MS(WAIT);

	// TODO: Exercise 2 - Draw moving car (Method 1), one pass across display.
	// Clear the entire display and redraw all objects each iteration.
	// Use a loop and increment x by OBJ_MOVE each iteration.
	// Start x off screen (negative coordinate).
	for (coord_t x = -CAR_W; x <= LCD_W; x += OBJ_MOVE) 
	{
		lcd_fillScreen(BACKGROUND_CLR);
		lcd_drawString(0, 0, "Exercise 2", TITLE_CLR);
		drawCar(x, OBJ_Y);
		char str[24];
		intToString(x, str);
		lcd_drawString(0, LCD_H - FONT_H, str, STATUS_CLR);
	}

	// TODO: Exercise 3 - Draw moving car (Method 2), one pass across display.
	// Move by erasing car at old position, then redrawing at new position.
	// Objects that don't change or move are drawn once.
	lcd_fillScreen(BACKGROUND_CLR);
	lcd_drawString(0, 0, "Exercise 3", TITLE_CLR);
	for (coord_t x = -CAR_W; x <= LCD_W; x += OBJ_MOVE) 
	{
		lcd_fillRect(x - OBJ_MOVE, OBJ_Y, CAR_W, CAR_H, BACKGROUND_CLR);
		drawCar(x, OBJ_Y);
		char str[24];
		intToString(x, str);
		lcd_fillRect(0, LCD_H - FONT_H, FONT_W * 3, FONT_H, BACKGROUND_CLR);
		lcd_drawString(0, LCD_H - FONT_H, str, STATUS_CLR);
		DELAY_MS(20);
	}

	// TODO: Exercise 4 - Draw moving car (Method 3), one pass across display.
	// First, draw all objects into a cleared, off-screen frame buffer.
	// Then, transfer the entire frame buffer to the screen.
	lcd_frameEnable();
	for (coord_t x = -CAR_W; x <= LCD_W; x += OBJ_MOVE) 
	{
		lcd_fillScreen(BACKGROUND_CLR);
		lcd_drawString(0, 0, "Exercise 4", TITLE_CLR);
		drawCar(x, OBJ_Y);
		char str[24];
		intToString(x, str);
		lcd_drawString(0, LCD_H - FONT_H, str, STATUS_CLR);
		lcd_writeFrame();
	} 

	// TODO: Exercise 5 - Draw an animated Pac-Man moving across the display.
	// Use Pac-Man sprites instead of the car object.
	// Cycle through each sprite when moving the Pac-Man character.
	while (1)
	{
		uint16_t i = 0;
		for (coord_t x = -PAC_W; x <= LCD_W; x += OBJ_MOVE) 
		{
			lcd_fillScreen(BACKGROUND_CLR);
			lcd_drawString(0, 0, "Exercise 5", TITLE_CLR);
			const uint8_t pidx[] = {0, 1, 2, 1};
			lcd_drawBitmap(x, OBJ_Y, pac[pidx[i++ % sizeof(pidx)]], PAC_W, PAC_H, YELLOW);
			char str[24];
			intToString(x, str);
			lcd_drawString(0, LCD_H - FONT_H, str, STATUS_CLR);
			lcd_writeFrame();
		} 
	}
}
