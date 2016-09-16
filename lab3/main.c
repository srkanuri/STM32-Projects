/***************************************************************
 * main.c - Lab3 Main Program
 *
 * Authors: Srikanth Kanuri (srkanuri)
 *          Raghavendra Nataraj (natarajr)
 * Date Created: 09/15/2016
 * Last Modified by: Srikanth Kanuri
 * Date Last Modified: 09/15/2016
 * Assignment: Lab3
 * Part of: Lab3
 ***************************************************************/
#include <stm32f30x.h>  // Pull in include files for F30x standard drivers 
#include <f3d_led.h>     // Pull in include file for the local drivers
#include <f3d_user_btn.h>

// Function Name: delay
// Description: Simple looping delay function
// Return Type: void
void delay(void) {
  int i = 2000000;
  while (i-- > 0) {
    asm("nop"); /* This stops it optimising code out */
  }
}

// Function Name: main
// Description: Main function for Lab3
// Return Type: int
int main(void) {
  f3d_led_init();
  f3d_user_btn_init();
  f3d_led_all_off();
  int led_cnt = 0;
  while(1){
    if(led_cnt == 8)
      led_cnt =0;
    f3d_led_on(led_cnt);
    delay();
    while(user_btn_read());
    f3d_led_off(led_cnt);
    led_cnt++;
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
