#ifndef BRICK_H
#define BRICK_H

#define L1 0
#define L2 20
#define L3 40
#define L4 60
#define L5 80
#define SCOREBOARD 100
#define MAXY 180
#define MINY -20
#define MAX_CARS 10


typedef struct car_struct{
  int used;
  int x;
  int prev_x;
  int y;
  int prev_y;
  int type;//WHITE(User), BLUE(Opponent Cars), RED(Red Cars)
}car;
extern nunchuk_t temp;
extern int playing_audio;
extern int brick_racing;
extern car user;
extern int speed;
extern int pause;
extern int level;
extern int score;

#endif /*BRICK_H*/
