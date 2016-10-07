/***************************************************************
 * main.c - Main program for Lab6 - LCD
 *
 * Author: Raghavendra Nataraj (natarajr) 
 *         Quintin Lepper (qlepper) 
 *         Srikanth Kanuri (srkanuri)
 * Date Created: 09/30/2016
 * Last Modified by: Raghavendra Nataraj (natarajr)
 * Date Last Modified: 10/04/2016
 * Assignment: Lab6
 * Part of: Lab6
 ***************************************************************/

#include <stm32f30x.h> // Pull in include files for F30x standard drivers
#include <f3d_led.h> // Pull in include file for the local drivers
#include <f3d_uart.h>
#include <f3d_gyro.h>
#include <f3d_lcd_sd.h>
#include <stdio.h>

#define TIMER 20000
#define XAXIS 0
#define YAXIS 41
#define ZAXIS 81

void drawBarPos(int x,int y,int color){
  int i = ST7735_height/2,j;
  for(;i>ST7735_height/2-y/4 ;i--)
    for(j=x;j<x+40;j++)
      f3d_lcd_drawPixel(j, i,color);
}

void drawBarNeg(int x,int y,int color){
  int i = ST7735_height/2,j;
  for(;i< ST7735_height/2+y/4 ;i++)
    for(j=x;j<x+40;j++)
      f3d_lcd_drawPixel(j, i,color);
}

void plotValues(float data[]){
  f3d_lcd_fillScreen2(RED);
  char str[40];
  sprintf(str, "X:%.1f,Y:%.1f,Z:%.1f",data[0],data[1],data[2]);
  if(data[0]<0)
    drawBarNeg(XAXIS,abs((int)data[0]),BLACK);
  else
    drawBarPos(XAXIS,(int)data[0],BLACK);
  
  if(data[1]<0)
    drawBarNeg(YAXIS,abs((int)data[1]),GREEN);
  else
    drawBarPos(YAXIS,(int)data[1],GREEN);
  
  if(data[2]<0)
    drawBarNeg(ZAXIS,abs((int)data[2]),BLUE);
  else
    drawBarPos(ZAXIS,(int)data[2],BLUE);
  
    f3d_lcd_drawString(0,0,str,WHITE,RED);
}

void delay(void) {
  int i = 2000000;
  while (i-- > 0){
    asm("nop");
  }
}

int main(void) {
  // If you have your inits set up, this should turn your LCD screen red
  f3d_lcd_init();
  f3d_gyro_init();
  f3d_uart_init();
  float data[3];
  while(1){
    delay();
     f3d_gyro_getdata(data);
     plotValues(data);
     delay();
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
