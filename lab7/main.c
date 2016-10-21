/***************************************************************
 * main.c - Main program for Lab7 - Accelerometer and Magnetometer
 *
 * Author: Raghavendra Nataraj (natarajr) 
 *         Srikanth Kanuri (srkanuri)
 * Date Created: 10/14/2016
 * Last Modified by: Raghavendra Nataraj (natarajr)
 * Date Last Modified: 10/20/2016
 * Assignment: Lab7
 * Part of: Lab7
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

#define TIMER 20000
#define XAXIS 0
#define YAXIS 41
#define ZAXIS 81

void drawBarPos(int x,int y,int color){
  int i = ST7735_height/2,j;
  for(;i>ST7735_height/2-y/2;i--)
    for(j=x;j<x+40;j++)
      f3d_lcd_drawPixel(j, i,color);
}

void drawBarNeg(int x,int y,int color){
  int i = ST7735_height/2,j;
  for(;i< ST7735_height/2+y/2 ;i++)
    for(j=x;j<x+40;j++)
      f3d_lcd_drawPixel(j, i,color);
}

void plotValues(float data[]){
  f3d_lcd_fillScreen2(RED);
  char str[40];
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
}


void delay(int count) {
  while (count-- > 0) {
    int i = 10000; 
    while (i-- > 0) {
      asm("nop");/* This stops it optimising code out */
    }
  }
}

int main(void) {
  // If you have your inits set up, this should turn your LCD screen red
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
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  float data[3];
  f3d_lcd_fillScreen2(RED);
  //Variables
  int led_no, mode = 0;
  float adata[3],mdata[3];
  char str[40];
  float pitch, roll, p_angle, r_angle, xa, ya, za,xm,ym,zm, xh,yh, heading;
  while(1){
    //Read Accel and Magnetometer values
    f3d_accel_read(adata);
    f3d_mag_read(mdata);
    xa  = adata[0];
    ya = adata[1];
    za = adata[2];
    xm = mdata[0];
    ym = mdata[1];
    zm = mdata[2];
    //Calculation of pitch. roll and heading
    pitch = atan2(xa, (sqrt(pow(ya,2)+pow(za,2))));
    p_angle = pitch*180/M_PI;
    roll = atan2(ya, sqrt(pow(xa,2)+pow(za,2)));
    r_angle = roll*180/M_PI;
    xh = xm*cos(pitch)+zm*sin(pitch);
    yh = (xm*sin(roll)*sin(pitch))+(ym*cos(roll))-(zm*sin(roll)*cos(pitch));
    heading = (atan2(yh,xh)*180)/M_PI;
    led_no = round(heading/45);
    if(user_btn_read())
      mode=(mode+1)%2;    
    if(mode == 0){
      f3d_lcd_fillScreen2(RED);
      printf("%d\n", led_no);
      sprintf(str, "Heading (\t's): %.1f", heading);
      f3d_lcd_drawString(0,0,str,WHITE,RED);
      f3d_led_all_off();
      f3d_led_on((8-led_no)%8);
    }
    else{
      f3d_led_all_off();
      printf("Pitch : %f,\t Roll : %f\n",p_angle,r_angle);
      sprintf(str, "Pitch: %.1f",p_angle);
      f3d_lcd_drawString(0,0,str,WHITE,RED);
      sprintf(str, "Roll: %.1f",r_angle);
      f3d_lcd_drawString(0,9,str,WHITE,RED);
      float data[3] = {p_angle, r_angle, 0};
      plotValues(data);
    }
    delay(5);
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
