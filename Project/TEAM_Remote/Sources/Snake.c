/**
 * \file
 * \brief Snake game
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements a classic Nokia phone game: the Snake game.
 *    Based on: https://bitbucket.org/hewerthomn/snake-duino-iii/src/303e1c2a016eb0746b2a7ad1176b16ee78f0177b/SnakeDuinoIII.ino?at=master
 */
/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@FreeBSD.ORG> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Poul-Henning Kamp
 * ----------------------------------------------------------------------------
 */
#include "Platform.h"

#if PL_CONFIG_HAS_SNAKE_GAME
#include "PDC1.h"
#include "WAIT1.h"
#include "GDisp1.h"
#include "FDisp1.h"
#include "GFONT1.h"
#include "Snake.h"
#include "UTIL1.h"
#include "FRTOS1.h"
#include "Event.h"

/* constants */
#define UP    1
#define RIGHT 2
#define DOWN  3
#define LEFT  4
#define PUSH  5

/* frame size */
#define MAX_WIDTH  GDisp1_GetWidth()
#define MAX_HEIGHT GDisp1_GetHeight()

/* defaults */
#define SNAKE_LEN   10 /* initial snake len */
#define SNAKE_SPEED 20 /* initial delay in ms */

/* snake length */
static int snakeLen = SNAKE_LEN;
static int point = 0, points = 10;
static int level = 0, time = SNAKE_SPEED;

static GDisp1_PixelDim xSnake, ySnake; /* snake face */
static GDisp1_PixelDim xFood = 0, yFood = 0; /* location of food */

/* directions */
static int dr = 0, dc = 1;
static bool left = FALSE, right = TRUE, up = FALSE, down = FALSE, push = FALSE;

#define SNAKE_MAX_LEN   48 /* maximum length of snake */

/* Button pressed flag*/
bool buttonPressedFlag = 0;

/* vector containing the coordinates of the individual parts */
/* of the snake {cols[0], row[0]}, corresponding to the head */
static GDisp1_PixelDim snakeCols[SNAKE_MAX_LEN];

/* Vector containing the coordinates of the individual parts */
/* of the snake {cols [snake_lenght], row [snake_lenght]} correspond to the tail */
static GDisp1_PixelDim snakeRow[SNAKE_MAX_LEN];

static void waitAnyButton(void) {
	/*! \todo Wait for any button pressed */
	buttonPressedFlag = 0;
	for (;;) {

		if (buttonPressedFlag) {
			buttonPressedFlag = 0;
			return;
		}
	}
}

static void delay(int ms) {
	WAIT1_WaitOSms(ms);
}

static int random(int min, int max) {
	TickType_t cnt;

	cnt = xTaskGetTickCount();
	cnt &= 0xff; /* reduce to 8 bits */
	if (max > 64) {
		cnt >>= 1;
	} else {
		cnt >>= 2;
	}
	if (cnt < min) {
		cnt = min;
	}
	if (cnt > max) {
		cnt = max / 2;
	}
	return cnt;
}

static void resetGame(void) {

	int i;
	FDisp1_PixelDim x, y;
	FDisp1_PixelDim charHeight, totalHeight;
	GFONT_Callbacks *font = GFONT1_GetFont();

	GDisp1_Clear();
	FDisp1_GetFontHeight(font, &charHeight, &totalHeight);

	x = (GDisp1_GetWidth()
			- FDisp1_GetStringWidth((unsigned char*) "Ready?", font, NULL)) / 2; /* center text */
	y = totalHeight;
	FDisp1_WriteString((unsigned char*) "Ready?", GDisp1_COLOR_BLACK, &x, &y,
			font);

	x = (GDisp1_GetWidth()
			- FDisp1_GetStringWidth((unsigned char*) "(Press Button)", font,
			NULL)) / 2; /* center text */
	y += totalHeight;
	FDisp1_WriteString((unsigned char*) "(Press Button)", GDisp1_COLOR_BLACK,
			&x, &y, font);
	GDisp1_UpdateFull();
	waitAnyButton();

	GDisp1_Clear();
	FDisp1_GetFontHeight(font, &charHeight, &totalHeight);
	x = (GDisp1_GetWidth()
			- FDisp1_GetStringWidth((unsigned char*) "Go!", font, NULL)) / 2; /* center text */
	y = GDisp1_GetHeight() / 2 - totalHeight / 2;
	FDisp1_WriteString((unsigned char*) "Go!", GDisp1_COLOR_BLACK, &x, &y,
			font);
	GDisp1_UpdateFull();
	delay(1000);
	GDisp1_Clear();
	GDisp1_UpdateFull();

	snakeLen = SNAKE_LEN;
	for (i = 0; i < (snakeLen - 1) && i < SNAKE_MAX_LEN; i++) {
		snakeCols[i] = i;
		snakeRow[i] = (MAX_HEIGHT / 2);
	}

	xSnake = 0;
	ySnake = (MAX_WIDTH / 2);

	xFood = (GDisp1_GetWidth() / 2);
	yFood = (GDisp1_GetHeight() / 2);

	level = 0;
	point = 0;
	points = SNAKE_LEN;
	time = SNAKE_SPEED;

	up = FALSE;
	right = TRUE;
	down = FALSE;
	left = FALSE;
	dr = 0;
	dc = 1;

	gameStatus = GAME_STATUS_RUN;
}

static void drawSnake(void) {
	int i;

	GDisp1_DrawBox(0, 0, MAX_WIDTH, MAX_HEIGHT, 1, GDisp1_COLOR_BLACK);
	for (i = snakeLen; i > 0; i--) {
		GDisp1_DrawCircle(snakeCols[i], snakeRow[i], 1, GDisp1_COLOR_BLACK);
	}
	GDisp1_DrawFilledBox(xFood, yFood, 3, 3, GDisp1_COLOR_BLACK);
	GDisp1_UpdateFull();
	for (i = snakeLen; i > 0; i--) {
		snakeRow[i] = snakeRow[i - 1];
		snakeCols[i] = snakeCols[i - 1];
	}
	snakeRow[0] += dr;
	snakeCols[0] += dc;
}

static void eatFood(void) {
	/* increase the point and snake length */
	point++;
	snakeLen += 2;
	/* new coordinates food randomly */
	xFood = random(3, MAX_WIDTH - 5);
	yFood = random(3, MAX_HEIGHT - 5);
	drawSnake();
}

static void upLevel(void) {
	level++;
	point = 1;
	points += 10;
	if (level > 1) {
		time -= 4;
	}
}

void direc(int d) {
	switch (d) {
	case UP: {
		buttonPressedFlag = 1;
		left = FALSE;
		right = FALSE;
		up = TRUE;
		down = FALSE;
		dr = -1;
		dc = 0;
	}
		break;
	case RIGHT: {
		buttonPressedFlag = 1;
		left = FALSE;
		right = TRUE;
		up = FALSE;
		down = FALSE;
		dr = 0;
		dc = 1;
	}
		break;
	case DOWN: {
		buttonPressedFlag = 1;
		left = FALSE;
		right = FALSE;
		up = FALSE;
		down = TRUE;
		dr = 1;
		dc = 0;
	}
		break;
	case LEFT: {
		buttonPressedFlag = 1;
		left = TRUE;
		right = FALSE;
		up = FALSE;
		down = FALSE;
		dr = 0;
		dc = -1;
	}
		break;
	case PUSH: {
		buttonPressedFlag = 1;
		if (gameStatus == GAME_STATUS_RUN) {
			push = TRUE;
		}
	}
		break;
	}
}

void moveSnake(void) {
	/* LEFT */
	/*! \todo handle events */
	if ("left event" && !right && !down && !up) {
		if ((xSnake > 0 || xSnake <= GDisp1_GetWidth() - xSnake)) {
			direc(LEFT);
		}
		return;
	}
	/* RIGHT */
	if ("right event" && !left && !down && !up) {
		if ((xSnake > 0 || xSnake <= GDisp1_GetWidth() - xSnake)) {
			direc(RIGHT);
		}
		return;
	}
	/* UP */
	if ("up event" && !down && !left && !right) {
		if ((ySnake > 0 || ySnake <= GDisp1_GetHeight() - ySnake)) {
			direc(UP);
		}
		return;
	}
	/* DOWN */
	if ("down event" && !up && !right && !left) {
		if ((ySnake > 0 || ySnake <= GDisp1_GetHeight() - ySnake)) {
			direc(DOWN);
		}
		return;
	}
	/* START/PAUSE */
	if ("start/pause event") {
		//showPause();
	}

}

static void gameover(void) {
	gameStatus = GAME_STATUS_END;
	buttonPressedFlag = 0;
	FDisp1_PixelDim x, y;
	FDisp1_PixelDim charHeight, totalHeight;
	GFONT_Callbacks *font = GFONT1_GetFont();
	uint8_t buf[16];

	GDisp1_Clear();
	FDisp1_GetFontHeight(font, &charHeight, &totalHeight);

	x = (GDisp1_GetWidth()
			- FDisp1_GetStringWidth((unsigned char*) "End of Game", font, NULL))
			/ 2; /* center text */
	y = 0;
	FDisp1_WriteString((unsigned char*) "End of Game", GDisp1_COLOR_BLACK, &x,
			&y, font);

	x = 0;
	y += totalHeight;
	FDisp1_WriteString((unsigned char*) "Level: ", GDisp1_COLOR_BLACK, &x, &y,
			font);
	UTIL1_Num16sToStr(buf, sizeof(buf), level);
	FDisp1_WriteString(buf, GDisp1_COLOR_BLACK, &x, &y, font);

	x = 0;
	y += totalHeight;
	FDisp1_WriteString((unsigned char*) "Points: ", GDisp1_COLOR_BLACK, &x, &y,
			font);
	UTIL1_Num16sToStr(buf, sizeof(buf), point - 1);
	FDisp1_WriteString(buf, GDisp1_COLOR_BLACK, &x, &y, font);

	x = (GDisp1_GetWidth()
			- FDisp1_GetStringWidth((unsigned char*) "(Press Button)", font,
			NULL)) / 2; /* center text */
	y += totalHeight;
	FDisp1_WriteString((unsigned char*) "(Press Button)", GDisp1_COLOR_BLACK,
			&x, &y, font);
	GDisp1_UpdateFull();
	delay(500);
	waitAnyButton();

	resetGame();
}

void showPause(void) {

	gameStatus = GAME_STATUS_PAUSE;
	buttonPressedFlag = 0;
	FDisp1_PixelDim x, y;
	FDisp1_PixelDim charHeight, totalHeight;
	GFONT_Callbacks *font = GFONT1_GetFont();
	uint8_t buf[16];

	GDisp1_Clear();
	FDisp1_GetFontHeight(font, &charHeight, &totalHeight);

	x = (GDisp1_GetWidth()
			- FDisp1_GetStringWidth((unsigned char*) "Pause", font, NULL)) / 2; /* center text */
	y = 0;
	FDisp1_WriteString((unsigned char*) "Pause", GDisp1_COLOR_BLACK, &x, &y,
			font);

	x = 0;
	y += totalHeight;
	FDisp1_WriteString((unsigned char*) "Level: ", GDisp1_COLOR_BLACK, &x, &y,
			font);
	UTIL1_Num16sToStr(buf, sizeof(buf), level);
	FDisp1_WriteString(buf, GDisp1_COLOR_BLACK, &x, &y, font);

	x = 0;
	y += totalHeight;
	FDisp1_WriteString((unsigned char*) "Points: ", GDisp1_COLOR_BLACK, &x, &y,
			font);
	UTIL1_Num16sToStr(buf, sizeof(buf), point - 1);
	FDisp1_WriteString(buf, GDisp1_COLOR_BLACK, &x, &y, font);

	x = (GDisp1_GetWidth()
			- FDisp1_GetStringWidth((unsigned char*) "(Press Button)", font,
			NULL)) / 2; /* center text */
	y += totalHeight;
	FDisp1_WriteString((unsigned char*) "(Press Button)", GDisp1_COLOR_BLACK,
			&x, &y, font);

	GDisp1_UpdateFull();
	delay(500);
	waitAnyButton();

	gameStatus = GAME_STATUS_RUN;
	push=FALSE;
	snake();
}

static void snake(void) {
	int i;

	xSnake = snakeCols[0];
	ySnake = snakeRow[0];
	if (point == 0 || point >= points) {
		upLevel();
	}
	GDisp1_Clear();
	/* GDisp1_UpdateFull(); */
	moveSnake();
	/* the snake has eaten the food (right or left) */
	for (i = 0; i < 3; i++) {
		/* control the snake's head (x) with x-coordinates of the food */
		if ((xSnake + 1 == xFood) || (xSnake == xFood + 1)) {
			/* control the snake's head (y) with y-coordinates of the food */
			if ((ySnake == yFood) || (ySnake + 1 == yFood)
					|| (ySnake == yFood + 1)) {
				eatFood();
			}
		}
		/* the snake has eaten the food (from above or from below) */
		if ((ySnake == yFood) || (ySnake == yFood + i)) {
			if ((xSnake == xFood) || (xSnake + i == xFood)
					|| (xSnake == xFood + i)) {
				eatFood();
			}
		}
	}
	/* LEFT */
	if (left) {
		/* snake touches the left wall */
		if (xSnake == 1) {
			gameover();

		}
		if (xSnake > 1) {
			drawSnake();
		}
	}
	/* RIGHT */
	if (right) {
		/* snake touches the top wall */
		if (xSnake == MAX_WIDTH - 1) {
			gameover();
		}
		if (xSnake < MAX_WIDTH - 1) {
			drawSnake();
		}
	}
	/* UP */
	if (up) {
		/* snake touches the above wall */
		if (ySnake == 1) {
			gameover();
		}
		if (ySnake > 1) {
			drawSnake();
		}
	}
	/* DOWN */
	if (down) {
		/* snake touches the ground */
		if (ySnake == MAX_HEIGHT - 1) {
			gameover();
		}
		if (ySnake < MAX_HEIGHT - 1) {
			drawSnake();
		}
	}
	/* PUSH */
	if (push) {
		/* button pushed */
		showPause();
	}
}

static void intro(void) {
	FDisp1_PixelDim x, y;
	FDisp1_PixelDim charHeight, totalHeight;
	GFONT_Callbacks *font = GFONT1_GetFont();

	GDisp1_Clear();
	FDisp1_GetFontHeight(font, &charHeight, &totalHeight);
	x = (GDisp1_GetWidth()
			- FDisp1_GetStringWidth((unsigned char*) "Snake Game", font, NULL))
			/ 2; /* center text */
	y = totalHeight;
	FDisp1_WriteString((unsigned char*) "Snake Game", GDisp1_COLOR_BLACK, &x,
			&y, font);
	y += totalHeight;
	x =
			(GDisp1_GetWidth()
					- FDisp1_GetStringWidth((unsigned char*) "McuOnEclipse",
							font, NULL)) / 2; /* center text */
	FDisp1_WriteString((unsigned char*) "McuOnEclipse", GDisp1_COLOR_BLACK, &x,
			&y, font);
	GDisp1_UpdateFull();
	WAIT1_WaitOSms(3000);
}

static void SnakeTask(void *pvParameters) {
	intro();
	resetGame();
	for (;;) {
		snake();
		vTaskDelay(time / portTICK_RATE_MS);
	}
}

void SNAKE_Deinit(void) {
	/* nothing to do? */
}

void SNAKE_Init(void) {
	if (xTaskCreate(SnakeTask, "SnakeTask", configMINIMAL_STACK_SIZE, NULL,
	tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		for (;;) {
		} /* error */
	}
}
#endif /* PL_HAS_SNAKE_GAME */
