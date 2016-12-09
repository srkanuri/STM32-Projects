/* f3d_systick.c ---
 *
 * Filename: f3d_systick.c
 * Description:
 * Author: Bryce Himebaugh
 * Maintainer:
 * Created: Thu Nov 14 07:57:37 2013
 * Last-Updated: 12/08/2016
 *           By: Raghavendra Nataraj(natarajr), Srikanth Kanuri(srkanuri)
 *     Update #: 2
 * Keywords:
 * Compatibility:
 *
 */

/* Commentary:
 *
 *
 *
 */

/* Change log:
 *
 *
 */

/* Copyright (c) 2004-2007 The Trustees of Indiana University and
 * Indiana University Research and Technology Corporation.
 *
 * All rights reserved.
 *
 * Additional copyrights may follow
 */

/* Code: */

#include <f3d_systick.h>
#include <f3d_led.h>
#include <f3d_user_btn.h>
#include <f3d_nunchuk.h>
#include <brick_game.h>
#include <f3d_lcd_sd.h>
#include <math.h>
#include <stdio.h>

extern int hiscore;

nunchuk_t temp;
int exit_interrupt = 0, play_g = 0;
volatile int systick_flag = 0;
int led_cnt = 0;
float pitch, roll;
int time = 0;
//comment
//int pause = 0, speed = 0;
//car user;

void f3d_systick_init(void) {
  f3d_led_init();
  f3d_led_all_off();
  f3d_user_btn_init();
  SysTick_Config(SystemCoreClock/100);
}

void f3d_systick_change(int interval){
  SysTick_Config(SystemCoreClock/interval);
}

void accel(){
  //Variables
  float adata[3];
  float xa, ya, za;
  //Read Accel values
  f3d_accel_read(adata);
  xa  = adata[0];
  ya = adata[1];
  za = adata[2];
  //Calculation of pitch. roll and heading
  pitch = atan2(xa, (sqrt(pow(ya,2)+pow(za,2))));
  roll = atan2(ya, sqrt(pow(xa,2)+pow(za,2)));
}


void SysTick_Handler(void){
  char str[50];
  int i;
  f3d_nunchuk_read(&temp);
  /* if(temp.z && playing_audio) */
  /*   exit_interrupt = 1; */
  if(play_g){
    exit_interrupt = 0;
    accel();
    if(user_btn_read()){
      pause = !pause;
      delay(10);
    }
    if(!pause){
      time+=1;
      if(time%100==0){
	f3d_led_on(led_cnt);
	led_cnt++;
      }

      if(time>=900){
	
	time = 0;
	led_cnt =0;
	old_level=level++;
	char lvl[3];
	sprintf(lvl,"%d",level);
	f3d_lcd_drawString(SCOREBOARD+1,50,lvl,WHITE,RED);
	f3d_led_all_off();
      }
    }
    if(user.prev_x == user.x && user.prev_y == user.y){
      user.prev_x = user.x;
      user.prev_y = user.y;
      if(temp.jx == 0x00 || roll >= 0.2){
	if(user.x >= 5){
	  user.x -= speed;
	}
      }
      if(temp.jx == 0xff || roll <= -0.2){
	if(user.x <= 80){
	  user.x += speed;
	}
      }
      if(temp.jy == 0x00 || pitch <= -0.2){
	if(user.y <= 135){
	  user.y += speed;
	}
      }
      if(temp.jy == 0xff || pitch >= 0.2){
	if(user.y >= 5){
	  user.y -= speed;
	}
      }
    }
  }
  else{
    if(temp.c){
	play_g = 1;
	exit_interrupt = 1;
    }
  }
}

/* f3d_systick.c ends here */
