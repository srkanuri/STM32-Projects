/* f3d_systick.c ---
 *
 * Filename: f3d_systick.c
 * Description:
 * Author: Bryce Himebaugh
 * Maintainer:
 * Created: Thu Nov 14 07:57:37 2013
 * Last-Updated: 11/17/2016
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

extern nunchuk_t temp;
extern int playing_audio;
int exit_interrupt = 0;
volatile int systick_flag = 0;
int led_cnt = 0;

void f3d_systick_init(void) {
  f3d_led_init();
  f3d_user_btn_init();
//f3d_nunchuk_init();
//delay(10);
  SysTick_Config(SystemCoreClock/100);
}

void f3d_systick_change(int interval){
  SysTick_Config(SystemCoreClock/interval);
}

void SysTick_Handler(void){
  /*if(user_btn_read())
    f3d_systick_change(10);
  else
    f3d_systick_change(100);
  f3d_led_off(led_cnt);
  led_cnt++;
  if(led_cnt == 8)
    led_cnt =0;
    f3d_led_on(led_cnt);*/
  f3d_nunchuk_read(&temp);
  if(temp.z && playing_audio)
    exit_interrupt = 1;
}

/* f3d_systick.c ends here */
