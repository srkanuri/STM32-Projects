/***************************************************************
 * main.c - Lab10 Main Program
 *
 * Authors: Srikanth Kanuri (srkanuri)
 *          Raghavendra Nataraj (natarajr)
 * Date Created: 11/09/2016
 * Last Modified by: Srikanth Kanuri
 * Date Last Modified: 11/09/2016
 * Assignment: Lab10
 * Part of: Lab10
 ***************************************************************/
#include <f3d_uart.h>
#include <stdio.h>
#include <stm32f30x.h>  // Pull in include files for F30x standard drivers 
#include <f3d_led.h>     // Pull in include file for the local drivers


int main(void) {
  f3d_led_init();
  f3d_user_btn_init();
  f3d_led_all_off();
  f3d_systick_init();
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  f3d_uart_init();
  while (1){
    putchar(getchar());
  }
}


#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
  /* Infinite loop */
  /* Use GDB to find out why we're here */
  while (1);
}
#endif

/* main.c ends here */
