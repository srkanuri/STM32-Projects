/***************************************************************
 * main.c - Main program for Lab8 - Nunchuk
 *
 * Author: Raghavendra Nataraj (natarajr) 
 *         Srikanth Kanuri (srkanuri)
 * Date Created: 10/21/2016
 * Last Modified by: Raghavendra Nataraj (natarajr)
 * Date Last Modified: 10/27/2016
 * Assignment: Lab8
 * Part of: Lab8
 ***************************************************************/

#include <stm32f30x.h> // Pull in include files for F30x standard drivers
#include <f3d_led.h> // Pull in include file for the local drivers
#include <f3d_uart.h>
#include <f3d_user_btn.h>
#include <f3d_gyro.h>
#include <f3d_lcd_sd.h>
#include <f3d_i2c.h>
#include <f3d_accel.h>
#include <f3d_mag.h>
#include <stdio.h>
#include <math.h>
#include <f3d_delay.h>
#include <f3d_nunchuk.h>
#include <f3d_delay.h>
#include <f3d_systick.h>
#include <brick_game.h>

#define L1 0
#define L2 20
#define L3 40
#define L4 60
#define L5 80
#define SCOREBOARD 100
#define MAXY 180
#define MINY -20
#define MAX_CARS 10

int is_free = 1;
int lane_picker[] = {L1, L2, L3, L4, L5};
int speed = 5;
car user;
car ocars[MAX_CARS];

void erase_car(car val){
  int i = 0, j = 0, x = val.prev_x, y = val.prev_y, color = BLACK;
  for(i = 0; i<15; i++){
   for(j = 0; j<20; j++){
	if(j>=15){
	  if(i < 5 || i >= 10)
	    f3d_lcd_drawPixel(x+i, y+j, color);
	}
	if(j>=10 && j<15){
	  if(i >= 5 && i < 10)
	    f3d_lcd_drawPixel(x+i, y+j, color);
	}
	if(j>=5 && j<10){
	  f3d_lcd_drawPixel(x+i, y+j, color);
	}
	if(j<5){
	  if(i >= 5 && i < 10)
	    f3d_lcd_drawPixel(x+i, y+j, color);
	}
      }
  }
}

void draw_car(car val){
  int i = 0, j = 0, x = val.x, y = val.y, color = val.type;
  for(i = 0; i<15; i++){
   for(j = 0; j<20; j++){
	if(j>=15){
	  if(i < 5 || i >= 10)
	    f3d_lcd_drawPixel(x+i, y+j, color);
	}
	if(j>=10 && j<15){
	  if(i >= 5 && i < 10)
	    f3d_lcd_drawPixel(x+i, y+j, color);
	}
	if(j>=5 && j<10){
	  f3d_lcd_drawPixel(x+i, y+j, color);
	}
	if(j<5){
	  if(i >= 5 && i < 10)
	    f3d_lcd_drawPixel(x+i, y+j, color);
	}
      }
  }
}

void redraw_car(car *val){
  erase_car(*val);
  val->prev_x = val->x;
  val->prev_y = val->y;
  draw_car(*val);
}

void move_cars(){
  int i = 0;
  if(user.x != user.prev_x){
    redraw_car(&user);
  }
  for(;i<MAX_CARS;i++){
    if(ocars[i].used){
      ocars[i].prev_x = ocars[i].x;
      ocars[i].prev_y = ocars[i].y;
      ocars[i].y += speed;
      if(ocars[i].y >= 180)
	ocars[i].used = 0;
      else
	redraw_car(&ocars[i]);
    }
  }
} 



void draw_score_board(){
  int j = 0;
  for(j = 0; j<160; j++)
    f3d_lcd_drawPixel(SCOREBOARD, j, WHITE);
}

void gen_cars(){
  int ncars = (rand()%3)+1, i = 0;
  for(;i<ncars;i++){
    int lane = rand()%5, k = 0;
    for(;k<MAX_CARS;k++){
      if(!ocars[k].used)
	break;
    }
    ocars[k].x = ocars[i].prev_x = lane_picker[lane];
    ocars[k].y = ocars[i].prev_x = 0;
    ocars[k].used = 1;
    ocars[k].type = BLUE;
    draw_car(ocars[k]);
  }
}

int main(void) {
  // If you have your inits set up, this should turn your LCD screen red
  int i;
  f3d_led_init();
  f3d_user_btn_init();
  f3d_lcd_init();
  f3d_gyro_init();
  f3d_uart_init();
  f3d_i2c1_init();
  delay(10);
  f3d_accel_init();
  delay(10);
  f3d_mag_init();
  f3d_nunchuk_init();
  delay(10);
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  f3d_systick_init();
  nunchuk_t temp;
  f3d_lcd_fillScreen2(BLACK);
  draw_score_board();
  //f3d_nunchuk_read(&temp);
  int k = 10;
  user.x = 0;
  user.y = user.prev_y = 140;
  user.type = WHITE;
  draw_car(user);
  for(i=0;i<MAX_CARS;i++){
    ocars[i].used = 0;
    ocars[i].type = BLUE;
    ocars[i].x=0;
    ocars[i].y=0;
  }
  while(1){
    if(k == 10){
      printf("Gen Cars Called");
      gen_cars();
      k = 0;
    }
    printf("%d k",k);
    move_cars();
    k++;
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
/* Infinite loop */
/* Use GDB to find out why we're here */
  
}
#endif

/* main.c ends here */
