/***************************************************************
 * main.c - Debugging code using printf
 *
 * Author: Raghavendra Nataraj (natarajr) 
 *         Quintin Lepper (qlepper) 
 *         Srikanth Kanuri (srkanuri)
 * Date Created: 09/30/2016
 * Last Modified by: Raghavendra Nataraj (natarajr)
 * Date Last Modified: 09/30/2016
 * Assignment: Lab6
 * Part of: Lab6
 ***************************************************************/

//main.c for lab6
#include <f3d_uart.h>
#include <stdio.h>
#include <f3d_gyro.h>
#include <f3d_led.h>
#include <f3d_user_btn.h>

// begin delay function
void delay(void) {
  int i = 2000000;
  while (i-- > 0){
    asm("nop");
  }
}

int main(void){
  f3d_uart_init();
  f3d_led_init();
  f3d_user_btn_init();
  delay();
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  int i =0;
  while(1){
    if(user_btn_read())
      i = !i;
    if(i){
    f3d_led_on(1);
      delay();
    f3d_led_off(1);
      delay();
    }else{
      f3d_led_on(1);
      delay();
      printf("STM321 Mode=%d\n",i);
      printf("STM322 Mode=%d\n",i);
      printf("STM323 Mode=%d\n",i);
      printf("STM324 Mode=%d\n",i);
      printf("STM325 Mode=%d\n",i);
    f3d_led_off(1);
      delay();
    }

  }
}

void assert_failed(uint8_t* file, uint32_t line) {
/* Infinite loop */
/* Use GDB to find out why we're here */
  while (1);
}
