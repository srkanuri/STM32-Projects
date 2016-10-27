/***************************************************************
 * main.c - Main program for Lab8 - Nunchuk
 *
 * Author: Raghavendra Nataraj (natarajr) 
 *         Srikanth Kanuri (srkanuri)
 * Date Created: 10/21/2016
 * Last Modified by: Raghavendra Nataraj (natarajr)
 * Date Last Modified: 10/27/2016
 * Assignment: Lab8
 * Part of: Lab8
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
#include <f3d_nunchuk.h>
#ifndef max
    #define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
    #define min(a,b) ((a) < (b) ? (a) : (b))
#endif
#define TIMER 20000
#define XAXIS 0
#define YAXIS 41
#define ZAXIS 81

void drawBarNeg(int x,int y,int color, int scale);
void drawBarPos(int x,int y,int color, int scale);

int main_toggle = 1, gyro_enable = 0, acc_enable = 0, mag_enable = 0, nun_enable =0, pin_enable = 0;
int xpin = 55, ypin = 75, h_refresh = 0;
int prev_x=0,prev_y=0,prev_z=0;

void reset_flags(){
  gyro_enable = acc_enable = mag_enable = nun_enable = pin_enable = 0;
}

void set_flags(int option){
  switch(option){
  case 0:gyro_enable = 1; f3d_lcd_drawString(0,0,"Gyroscope App",WHITE,RED); break;
  case 1:acc_enable = 1; f3d_lcd_drawString(0,0,"Accelerometer App",WHITE,RED); break;
  case 2:mag_enable = 1; f3d_lcd_drawString(0,0,"Compass App",WHITE,RED); break;
  case 3:nun_enable = 1; f3d_lcd_drawString(0,0,"Nunchuk App",WHITE,RED); break;
  case 4:pin_enable = 1; f3d_lcd_drawString(0,0,"Pin Motion",WHITE,RED); break;
  default: reset_flags();
  }
}

void set_prev(int a, int b){
  switch(a){
  case 0: prev_x = b; break;
  case 41: prev_y = b; break;
  case 81: prev_z = b; break;
  }
}

int sign(int x){
  if(x >= 0)
    return 1;
  if(x < 0)
    return -1;
}

void f3d_lcd_fillScreen_p(uint16_t color) {
  uint8_t x,y;
  f3d_lcd_setAddrWindow (0,20,ST7735_width-1,ST7735_height-1,MADCTLGRAPHICS);
  for (x=0;x<ST7735_width;x++) {
    for (y=20;y<ST7735_height; y++) {
      f3d_lcd_pushColor(&color,1);
    }
  }
}

void f3d_lcd_fillScreen_h(uint16_t color) {
  uint8_t x,y;
  f3d_lcd_setAddrWindow (0,10,ST7735_width-1,20,MADCTLGRAPHICS);
  for (x=0;x<ST7735_width;x++) {
    for (y=10;y<20; y++) {
      f3d_lcd_pushColor(&color,1);
    }
  }
}

void drawBarPos(int x,int y,int color, int scale){
  int i = ST7735_height/2,j;
  for(;i>ST7735_height/2-y/scale;i--)
    for(j=x;j<x+40;j++)
      f3d_lcd_drawPixel(j, i,color);
}

void drawBarNeg(int x,int y,int color, int scale){
  int i = ST7735_height/2,j;
  for(;i< ST7735_height/2+y/scale ;i++)
    for(j=x;j<x+40;j++)
      f3d_lcd_drawPixel(j, i,color);
}
/*
void drawBarPos(int x,int y,int color){
  int i,mid = ST7735_height/2,j,prev;
  //for(;i>ST7735_height/2-y/2;i--)
  switch(x){case 0:prev=prev_x;break; case 41:prev=prev_y;break;case 81:prev=prev_z;break;}
  if(sign(y) != sign(prev))
    drawBarNeg(x,prev,RED);
  for(i=mid;i>mid-max(y/3,prev/3);i--){
    for(j=x;j<x+40;j++){
      if(i<(mid-prev/3))
	f3d_lcd_drawPixel(j, i,color);
      if(i<(mid-y/3))
	f3d_lcd_drawPixel(j, i,RED);
    }
  }
}

void drawBarNeg(int x,int y,int color){
  int i,mid = ST7735_height/2,j,prev;
  switch(x){case 0:prev=prev_x;break; case 41:prev=prev_y;break;case 81:prev=prev_z;break;}
  //for(;i< ST7735_height/2+y/2 ;i++)
  if(sign(y) != sign(prev))
    drawBarPos(x,prev,RED);
  for(i=mid;i< mid-min(y/3,prev/3);i++){
    for(j=x;j<x+40;j++){
      if(i>(mid-prev/3))
	f3d_lcd_drawPixel(j, i,color);
      if(i>(mid-y/3))
	f3d_lcd_drawPixel(j, i,RED);
    }
  }
  }*/

void plotValues(float data[], int scale){
  if(h_refresh%10 == 0)
    f3d_lcd_fillScreen_h(RED);
  h_refresh++;
  f3d_lcd_fillScreen_p(RED);
  if(data[0]<0)
    drawBarNeg(XAXIS,abs((int)data[0]),BLACK,scale);
  else
    drawBarPos(XAXIS,(int)data[0],BLACK,scale);
  set_prev(XAXIS,(int)data[0]);
  if(data[1]<0)
    drawBarNeg(YAXIS,abs((int)data[1]),GREEN,scale);
  else
    drawBarPos(YAXIS,(int)data[1],GREEN,scale);
  set_prev(YAXIS,(int)data[1]);
  if(data[2]<0)
    drawBarNeg(ZAXIS,abs((int)data[2]),BLUE,scale);
  else
    drawBarPos(ZAXIS,(int)data[2],BLUE,scale);
  set_prev(ZAXIS,(int)data[2]);
}


void delay(int count) {
  while (count-- > 0) {
    int i = 10000; 
    while (i-- > 0) {
      asm("nop");/* This stops it optimising code out */
    }
  }
}

void nunchuk(float adata[]){
  //f3d_lcd_fillScreen2(RED);
  //Variables
  int led_no;
  char str[40];
  float pitch, roll, p_angle, r_angle, xa, ya, za;
  //Read Accel and Magnetometer values
  xa  = adata[0]-512;
  ya = adata[1]-512;
  za = adata[2]-512;
  //Calculation of pitch. roll and heading
  pitch = atan2(xa, (sqrt(pow(ya,2)+pow(za,2))));
  p_angle = pitch*180/M_PI;
  roll = atan2(ya, sqrt(pow(xa,2)+pow(za,2)));
  r_angle = roll*180/M_PI;
  printf("Pitch : %f,\t Roll : %f\n",pitch,roll);
  sprintf(str, "P:%.1f, R:%.1f",p_angle,r_angle);
  f3d_lcd_drawString(0,10,str,WHITE,RED);
  float data[3] = {p_angle, r_angle, 0};
  plotValues(data, 2);
}

void gyro(){
  float data[3];
  char str[40];
  f3d_gyro_getdata(data);
  sprintf(str, "%3.1f,%3.1f,%3.1f",round((data[0] * 10)) / 10,round((data[1] * 10)) / 10,round((data[2] * 10)) / 10);
  plotValues(data,5);
  f3d_lcd_drawString(0,10,str,WHITE,RED);
}

void accel(int option){
float data[3];
  //f3d_lcd_fillScreen2(RED);
  //Variables
  int led_no;
  float adata[3],mdata[3];
  char str[40];
  float pitch, roll, p_angle, r_angle, xa, ya, za,xm,ym,zm, xh,yh, heading;
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
  if(option == 2){
    //f3d_lcd_fillScreen2(RED);
    printf("%d\n", led_no);
    sprintf(str, "Heading (\t's): %.1f", heading);
    f3d_lcd_drawString(0,10,str,WHITE,RED);
    f3d_led_all_off();
    f3d_led_on((8-led_no)%8);
  }
  //Mode 1 - Bar graph on LCD with text
  if(option == 1){
    f3d_led_all_off();
    printf("Pitch : %f,\t Roll : %f\n",pitch,roll);
    sprintf(str, "P:%.1f, R:%.1f",p_angle,r_angle);
    f3d_lcd_drawString(0,10,str,WHITE,RED);
    float data[3] = {p_angle, r_angle, 0};
    plotValues(data, 2);
  }
}

void draw_menu(int option){
  f3d_lcd_drawString(0,0,"Main Menu :",WHITE,RED);
  f3d_lcd_drawString(0,10,"1. Gyroscope App",WHITE,((option==0)? BLACK: RED));
  f3d_lcd_drawString(0,20,"2. Accelerometer App",WHITE,((option==1)? BLACK: RED));
  f3d_lcd_drawString(0,30,"3. Compass App",WHITE,((option==2)? BLACK: RED));
  f3d_lcd_drawString(0,40,"4. Nunchuck App",WHITE,((option==3)? BLACK: RED));
  f3d_lcd_drawString(0,50,"5. Pin Motion",WHITE,((option==4)? BLACK: RED));
}

void pin_motion(short adata[]){
  //f3d_lcd_fillScreen2(RED);
  
  int i = 0, j = 0, eflag = 0;
  int ymin = 5, ymax = 155;
  int xmin = -2,  xmax = 125;
  if(adata[0] == 0x00){
    if(xpin-10 > xmin)
      xpin-= 10;
    else
      eflag = 1;
  }
  if(adata[0] == 0xff){
    if(xpin+10 < xmax)
      xpin+= 10;
    else
      eflag = 1;
  }
  if(adata[1] == 0x00){
    if(ypin+10 < ymax)
      ypin+=10;
    else
      eflag = 1;
  }
  if(adata[1] == 0xff){
    if(ypin-10 > ymin)
      ypin-=10;
    else
      eflag = 1;
  }
  if(eflag)
    return;
  else{
    for(i = 0; i<10; i++){
      for(j = 0; j<10; j++){
	f3d_lcd_drawPixel(prev_x+i, prev_y+j, RED);
	f3d_lcd_drawPixel(xpin+i, ypin+j, WHITE);
      }
    }
    prev_x = xpin;
    prev_y = ypin;
  }
}


int main(void) {
  // If you have your inits set up, this should turn your LCD screen red
  int option = 0;
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
  f3d_nunchuk_init();
  delay(10);
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  nunchuk_t temp;
  f3d_lcd_fillScreen2(RED);
  draw_menu(option);
  while (1){
    f3d_nunchuk_read(&temp);
    printf("N1:jx = %c;jy=%c;ax=%d;ay=%d;az=%d;c=%c,z=%c\n",temp.jx,temp.jy,temp.ax,temp.ay,temp.az,temp.c,temp.z);
    if(temp.c && main_toggle){
      main_toggle = 0;
      reset_flags();
      f3d_lcd_fillScreen2(RED);
      set_flags(option);
      h_refresh = 0;
    }
    if(!pin_enable && !main_toggle && temp.jx == 0x00){
      reset_flags();
      --option;
      if(option == -1)
	option = 4;
      f3d_led_all_off();
      f3d_lcd_fillScreen2(RED);
      set_flags(option);
    }
    if(!pin_enable && !main_toggle && temp.jx == 0xff){
      reset_flags();
      ++option;
      if(option == 5)
	option =0;
      f3d_led_all_off();
      f3d_lcd_fillScreen2(RED);
      set_flags(option);
    }
    if(temp.z && !main_toggle){
      main_toggle = 1;
      reset_flags();
      f3d_lcd_fillScreen2(RED);
      f3d_led_all_off();
      draw_menu(option);
    }
    if(main_toggle && temp.jy == 0x00){
      ++option;
      if(option == 5){
	option = 0;
      }
      draw_menu(option);
    }
    if(main_toggle && temp.jy == 0xff){
      --option;
      if(option == -1){
	option = 4;
      }
      draw_menu(option);
    }
    if(gyro_enable && !main_toggle){
      gyro();
    }
    if((acc_enable || mag_enable) && !main_toggle){
      accel(option);
    }
    if(nun_enable && !main_toggle){
      float adata[3]={temp.ax,temp.ay,temp.az};
      nunchuk(adata);
    }
    if(pin_enable && !main_toggle){
      short adata[2]={temp.jx, temp.jy};
      pin_motion(adata);
    }
    if(main_toggle)
      delay(60);
    //else
    //delay(10);
    //f3d_nunchuk_read2(&temp);
    //printf("N2:jx = %c;jy=%c;ax=%d;ay=%d;az=%d;c=%c,z=%c\n",temp.jx,temp.jy,temp.ax,temp.ay,temp.az,temp.c,temp.z);
  }
 
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
/* Infinite loop */
/* Use GDB to find out why we're here */
  
}
#endif

/* main.c ends here */
