/***************************************************************
 * main.c - Lab5 Main Program
 *
 * Authors: Srikanth Kanuri (srkanuri)
 *          Daozhen Lu (daozlv)
 * Date Created: 09/28/2016
 * Last Modified by: Srikanth Kanuri
 * Date Last Modified: 09/29/2016
 * Assignment: Lab5
 * Part of: Lab5
 ***************************************************************/
#include <f3d_uart.h>
#include <f3d_gyro.h>
#include <f3d_led.h>
#include <f3d_user_btn.h>
#include <stdio.h>

void delay(void) {
  int i = 2000000;
  while (i-- > 0) {
    asm("nop"); /* This stops it optimising code out */
  }
}

//Name : main
//Description: Main fucntion to init and run the main logic for the angular velocity visualization application
//Input: None
//Output: int
int main(void){
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  f3d_led_init();
  f3d_user_btn_init();
  f3d_uart_init();
  f3d_gyro_init();
  float gyroData[3];
  int temp,i,j,c;
  temp = 0;
  i = 0;
  while(1){
    f3d_gyro_getdata(gyroData);
    while(user_btn_read() == 1){
       i = temp+1;
    }
    if(i == 3){
      i = 0;
    }
    if (c = getch()){
      if(c == 'x')
	i = 0;
      if(c == 'y')
	i = 1;
      if(c == 'z')
	i = 2;
    }
    temp = i;
    printf("%f\n",gyroData[temp]);
    int k;
    k = abs((int)gyroData[temp])/20;
    for(j = 0; j < 8; j++){
      if(gyroData[temp] > 0){
	if(j<k){ 
	  f3d_led_on(j);
	}
	else{
	  f3d_led_off(j);
	}
      }
      else{
	if(j<k){ 
	  f3d_led_on((8-j)%8);
	}
	else{
	  f3d_led_off((8-j)%8);
	}
      }
    }
    delay();
  }
}

void assert_failed(uint8_t* file, uint32_t line) {
/* Infinite loop */
/* Use GDB to find out why we're here */
  while (1);
}
