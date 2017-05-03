/**
 * \file
 * \brief Snake game
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements a classic Nokia phone game: the Snake game.
 */

#ifndef SNAKE_H_
#define SNAKE_H_

/*! \todo Extend interface as needed */

typedef enum {
  GAME_STATUS_ERROR         =  0,     /* error */
  GAME_STATUS_RUN           = (1<<0), /* Game is Running */
  GAME_STATUS_PAUSE         = (1<<1), /* Pause Menu view */
  GAME_STATUS_END           = (1<<2), /* Game ENd view*/
} Game_StatusFlag;

Game_StatusFlag gameStatus;

/*!
 * \brief resets the game
 */

static void waitAnyButton(void);

static void resetGame(void);

static void snake(void);

static void showPause(void);

/*!
 * \brief sets the direction of the snake
 */
void direc(int d);

/*!
 * \brief sets the direction of the snake
 */
void moveSnake(void);

/*!
 * \brief Driver de-initialization.
 */
void SNAKE_Deinit(void);

/*!
 * \brief Driver initialization.
 */
void SNAKE_Init(void);

#endif /* SNAKE_H_ */
