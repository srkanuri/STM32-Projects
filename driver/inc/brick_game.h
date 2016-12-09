#ifndef BRICK_H
#define BRICK_H
/***************************************************************
 * brick_game.h - Header file for brick game
 *
 * Author: Raghavendra Nataraj (natarajr)
 *         Srikanth Kanuri (srkanuri)
 * Date Created: 11/21/2016
 * Last Modified by: Srikanth Kanuri, Raghavendra Nataraj
 * Date Last Modified: 12/09/2016
 * Assignment: Final Project
 * Part of: Final Project
 ***************************************************************/
#define L1 0
#define L2 20
#define L3 40
#define L4 60
#define L5 80
#define SCOREBOARD 100
#define MAXY 180
#define MINY -20
#define MAX_CARS 10

struct ckhd {
  uint32_t ckID;
  uint32_t cksize;
};

struct fmtck {
  uint16_t wFormatTag;
  uint16_t nChannels;
  uint32_t nSamplesPerSec;
  uint32_t nAvgBytesPerSec;
  uint16_t nBlockAlign;
  uint16_t wBitsPerSample;
};

//Car structure
typedef struct car_struct{
  int used;
  int x;
  int prev_x;
  int y;
  int prev_y;
  int type;//WHITE(User), BLUE(Opponent Cars), RED(Red Cars)
}car;

//Extern Variables
extern nunchuk_t temp;
extern int playing_audio;
extern int brick_racing;
extern car user;
extern int speed;
extern int pause;
extern int level;
extern int score;

#endif /*BRICK_H*/
