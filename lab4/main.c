/***************************************************************
 * main.c - Lab3 Main Program
 *
 * Authors: Srikanth Kanuri (srkanuri)
 *          Brent Hall (hallba)
 * Date Created: 09/23/2016
 * Last Modified by: Srikanth Kanuri
 * Date Last Modified: 09/23/2016
 * Assignment: Lab4
 * Part of: Lab4
 ***************************************************************/
#include <f3d_uart.h>
#include <stdio.h>
#include <stm32f30x.h>  // Pull in include files for F30x standard drivers 
#include <f3d_led.h>     // Pull in include file for the local drivers
#include "mywc.h"


// Simple looping delay function
void delay(void) {
  int i = 2000000;
  while (i-- > 0) {
    asm("nop"); /* This stops it optimising code out */
  }
}

int main(void) {
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  f3d_uart_init();

  mywc();
  while (1){
    //putstring("hello"); 
    //putchar('a');
    //putchar(getchar());
    //printf("hello world");
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
