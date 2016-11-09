/* f3d_systick.c --- 
 * 
 * Filename: f3d_systick.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Thu Nov 14 07:57:37 2013
 * Last-Updated: 11/09/2016
 *           By: Raghavendra Nataraj(natarajr), Srikanth Kanuri(srkanuri)
 *     Update #: 1
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
#include <f3d_uart.h>

volatile int systick_flag = 0;
int led_cnt = 0;

void f3d_systick_init(void) {
  SysTick_Config(SystemCoreClock/12);
}

void SysTick_Handler(void) {
  f3d_led_off(led_cnt);
  led_cnt++;
  if(led_cnt == 8)
    led_cnt =0;
  f3d_led_on(led_cnt);
}

/* f3d_systick.c ends here */
