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
#include "brick_game.h"
#include <ff.h>
#include <diskio.h>
#include "bmp.h"

int is_free = 1, pause;
int lane_picker[] = {L1, L2, L3, L4, L5};
int speed = 5, car_width = 15, car_length = 20;
car user;
int inc;
int score = 0;
car ocars[MAX_CARS];
uint16_t blank[300];
int level = 1;
FATFS Fatfs;		/* File system object */
FIL Fil;		/* File object */
BYTE Buff[5];		/* File read buffer */
FRESULT rc;			/* Result code */
DIR dir;			/* Directory object */
FILINFO fno;			/* File information object */
UINT bw, br;
unsigned int retval;

void die (FRESULT rc) {
  printf("Failed with rc=%u.\n", rc);
  while (1);
}

void erase_car(car val){
  int i = 0, j = 0, x = val.prev_x, y = val.prev_y;
  f3d_lcd_setAddrWindow(x,y,x+14,y+19,MADCTLGRAPHICS);
  f3d_lcd_pushColor(blank,300);
}

void draw_car(car val){
  int i = 0, j = 0, x = val.x, y = val.y, color = val.type;
  uint16_t buff[300];
  for(i = 0; i<15; i++){
    for(j = 0; j<20; j++){
      if(j>=15){
	if(i < 5 || i >= 10)
	  buff[(j*car_width)+i]=color;
	else
	  buff[(j*car_width)+i]=BLACK;
      }
      if(j>=10 && j<15){
	if(i >= 5 && i < 10)
	  buff[(j*car_width)+i]=color;
	else
	  buff[(j*car_width)+i]=BLACK;
      }
      if(j>=5 && j<10)
	buff[(j*car_width)+i]=color;
      if(j<5){
	if(i >= 5 && i < 10)
	  buff[(j*car_width)+i]=color;
	else
	  buff[(j*car_width)+i]=BLACK;
      }
    }
  }
  f3d_lcd_setAddrWindow(x,y,x+14,y+19,MADCTLGRAPHICS);
  f3d_lcd_pushColor(buff,300);
}

void redraw_car(car *val){
  erase_car(*val);
  val->prev_x = val->x;
  val->prev_y = val->y;
  draw_car(*val);
}

int move_cars(){
  int i=0, collision_x=0, collision_y=0;
  if(user.x != user.prev_x || user.y != user.prev_y){
    redraw_car(&user);
  }
  for(;i<MAX_CARS;i++){
    collision_x = 0;
    collision_y = 0;
    if(ocars[i].used){
      ocars[i].prev_x = ocars[i].x;
      ocars[i].prev_y = ocars[i].y;
      ocars[i].y += speed;
      if(ocars[i].y >= 165){
	ocars[i].used = 0;
	score+=inc;
	char scr[3];
	sprintf(scr,"%d",score);
	f3d_lcd_drawString(SCOREBOARD+1,110,scr,RED,BLACK);
      }
      else
	redraw_car(&ocars[i]);
      //Check Collision condition
      if(user.x > ocars[i].x && user.x < ocars[i].x+car_width)
	collision_x = 1;
      if(user.x+car_width > ocars[i].x && user.x+car_width < ocars[i].x+car_width)
	collision_x = 1;
      if(user.y > ocars[i].y && user.y < ocars[i].y+car_length)
	collision_y = 1;
      if(user.y+car_length > ocars[i].y && user.y+car_length < ocars[i].y+car_length)
	collision_y = 1;
      if(collision_y && user.x==ocars[i].x && user.x+car_width==ocars[i].x+car_width)
	collision_x = 1;
      if((collision_x && collision_y)){
	printf("Crash details %d,%d,%d,%d",ocars[i].x,user.x,ocars[i].y,user.y);
	return 1;
      }                                                                                                                                                                                                                                                                                   
    }
  }
  return 0;
} 



void draw_score_board(){
  int j = 0;
  for(j = 0; j<160; j++)
    f3d_lcd_drawPixel(SCOREBOARD, j, WHITE);
  f3d_lcd_drawString(SCOREBOARD+1,40,"LVL",RED,BLACK);
  f3d_lcd_drawString(SCOREBOARD+1,50,"1",RED,BLACK);
  f3d_lcd_drawString(SCOREBOARD+1,100,"SCR",RED,BLACK);
  f3d_lcd_drawString(SCOREBOARD+1,110,"0",RED,BLACK);
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
    ocars[k].y = ocars[i].prev_y = -20;
    ocars[k].used = 1;
    ocars[k].type = BLUE;
    draw_car(ocars[k]);
  }
}

void get_hi_score(){
  printf("In get hi score\n");
  rc = f_open(&Fil, "brick.hs", FA_READ);
  if(rc){
    rc = f_open(&Fil, "brick.hs", FA_WRITE | FA_CREATE_ALWAYS);
    if (rc) die(rc);
  }
  rc = f_read(&Fil, Buff, sizeof Buff, &br);
  if (rc) die(rc);
  printf("Hi Score %s",Buff);
  rc = f_close(&Fil);
  if (rc) die(rc);
}

void set_hi_score(int value){
  char val[5];
  rc = f_open(&Fil, "_brick.hs", FA_WRITE);
  if (rc) die(rc);
  sprintf(val, "%d", value);
  rc = f_write(&Fil, val, 5, &bw);
  if (rc) die(rc);
  rc = f_close(&Fil);
  if (rc) die(rc);
}

int main(void) {
  // If you have your inits set up, this should turn your LCD screen red
  int i;
  int duration = 100;
  f3d_delay_init();
  f3d_rtc_init();
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
  delay(10);
  f3d_nunchuk_init();
  delay(10);
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  f3d_systick_init();
  printf("Before mounting FS\n");
  f_mount(0, &Fatfs);
  f3d_lcd_fillScreen2(BLACK);
  draw_score_board();
  //f3d_nunchuk_read(&temp);
  int k = 15;
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
  get_hi_score();
  while(1){
    printf("Pause %d:\n", pause);
    if(!pause){
      if(k == 15){
	printf("Gen Cars Called");
	gen_cars();
	k = 0;
      }
      printf("Z value %d\n", temp.z);
      if(temp.z){
	delay(20);
	inc = 2;
      }else{
	delay(duration/level);
	inc = 1;
      }
      printf("%d k",k);
      if(move_cars()){
	printf("Crash at %d, %d\n",user.x, user.y);
	f3d_lcd_fillScreen2(BLACK);
	f3d_lcd_drawString(0,10,"GAME OVER !!!",RED,BLACK);
	pause = 1;
	printf("outside");
	while(1){
	  printf("Inside");
	  f3d_led_all_off();
	  delay(100);
	  f3d_led_all_on();
	  delay(100);
	}
	break;
      }
      k++;
    }
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
/* Infinite loop */
/* Use GDB to find out why we're here */
  
}
#endif

/* main.c ends here */
