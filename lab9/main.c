/***************************************************************
 * main.c - Main program for Lab9 - SD Card
 *
 * Author: Raghavendra Nataraj (natarajr) 
 *         Srikanth Kanuri (srkanuri)
 * Date Created: 11/02/2016
 * Last Modified by: Srikanth Kanuri(srkanuri)
 * Date Last Modified: 11/02/2016
 * Assignment: Lab9
 * Part of: Lab9
 ***************************************************************/

#include <stm32f30x.h>  // Pull in include files for F30x standard drivers 
#include <f3d_led.h>
#include <f3d_uart.h>
#include <f3d_gyro.h>
#include <f3d_lcd_sd.h>
#include <f3d_i2c.h>
#include <f3d_accel.h>
#include <f3d_mag.h>
#include <f3d_nunchuk.h>
#include <f3d_rtc.h>
#include <math.h>
#include <ff.h>
#include <string.h>
#include <diskio.h>
#include <stdio.h>
#include "bmp.h"

void die (FRESULT rc) {
  printf("Failed with rc=%u.\n", rc);
  while (1);
}
typedef struct file{
  char name[13];
  int type;
}file;
int main_toggle = 1;
FATFS Fatfs;		/* File system object */
FIL Fil;		/* File object */
file files[10];
int numOfFiles = 0;
enum ORIENT { portrait = 0, landscape = 1 };
static enum ORIENT orientation = portrait;
int level = 1;
void readFile(){
  char footer[20];
  int count=0;
  int i=0;

  FRESULT rc;			/* Result code */
  DIR dir;			/* Directory object */
  FILINFO fno;			/* File information object */
  UINT bw, br;
  unsigned int retval;

  f_mount(0, &Fatfs);		/* Register volume work area (never fails) */

  
  /*printf("\nCreate a new file (hello.txt).\n");
  rc = f_open(&Fil, "HELLO.TXT", FA_WRITE | FA_CREATE_ALWAYS);
  if (rc) die(rc);
  
  printf("\nWrite a text data. (Hello world!)\n");
  rc = f_write(&Fil, "Hello world!\r\n", 14, &bw);
  if (rc) die(rc);
  printf("%u bytes written.\n", bw);
  
  printf("\nClose the file.\n");
  rc = f_close(&Fil);
  if (rc) die(rc);
  */
  //printf("\nOpen root directory.\n");
  rc = f_opendir(&dir, "");
  if (rc) die(rc);
  
  //printf("\nDirectory listing...\n");
  f3d_lcd_fillScreen2(RED);
  int scr_cnt = 0;
  i=0;
  f3d_lcd_drawString(0,0,"Root:",WHITE,RED);
  for (scr_cnt=10;;scr_cnt+=10) {
    char str[40];
    rc = f_readdir(&dir, &fno);		/* Read a directory item */
    if (rc || !fno.fname[0]) break;	/* Error or end of dir */
    if (fno.fattrib & AM_DIR){
      printf("   <dir>  %s\n", fno.fname);
      //sprintf(files[i++].name, "%s", fno.fname);
      //files[i++].type = 1;
    }
    else{
      numOfFiles++;
      sprintf(files[i].name, "%s", fno.fname);
      files[i++].type = 0;
    }
  }
  if (rc) die(rc);
  
  //printf("\nTest completed.\n");

  rc = disk_ioctl(0,GET_SECTOR_COUNT,&retval);
  printf("%d %d\n",rc,retval);

}

void readImage(char file[]){
  BYTE Buff[384];		/* File read buffer */
  unsigned short d1[128]; 
  struct bmpfile_header head;
  BITMAPINFOHEADER info;
  struct bmpfile_magic magic;
  FRESULT rc;			/* Result code */
  UINT bw, br;
  int i;
  f_mount(0, &Fatfs);		/* Register volume work area (never fails) */
  rc = f_open(&Fil, file, FA_READ);
  if (rc) die(rc);
  rc = f_read(&Fil, Buff, sizeof head, &br);	/* Read a chunk of file */
  rc = f_read(&Fil, Buff, sizeof info, &br);	/* Read a chunk of file */
  rc = f_read(&Fil, Buff, sizeof magic, &br);	/* Read a chunk of file */
  if(!orientation){
    for (i=0;;i++) {
      rc = f_read(&Fil, Buff, sizeof Buff, &br);	
      if (rc || !br) break;		      
      int j =0;
      if(level)
	f3d_lcd_setAddrWindow (0,ST7735_height-i-1,ST7735_width,ST7735_height-i-1,MADCTLGRAPHICS);
      else
	f3d_lcd_setAddrWindow (0,i,ST7735_width,i,MADCTLGRAPHICS);
      for (j = 0; j < br; j+=3){
	char r = Buff[j]>>3;
	char g = Buff[j+1]>>2;
	char b = Buff[j+2]>>3;
	if(level)
	  d1[j/3] = ((r << 11) | (g << 5) | b);
	  //f3d_lcd_drawPixel((j/3),160-i, d1[j/3]);
	else
	  d1[ST7735_width-(j/3)-1] = ((r << 11) | (g << 5) | b);
	  //f3d_lcd_drawPixel(128-(j/3),i, d1[j/3]);
      }
      f3d_lcd_pushColor(d1, ST7735_width);
    }
  }else{
    int cnt = 0;
    for (i=0;i<160;i++) {
      rc = f_read(&Fil, Buff, sizeof Buff, &br);	/* Read a chunk of file */
      if (rc || !br) break;			/* Error or end of file */
      int j =0;
      if(i%5 != 2){
	if(level)
	  f3d_lcd_setAddrWindow (ST7735_width-(i-cnt)-1,0,ST7735_width-(i-cnt)-1,ST7735_width,MADCTLGRAPHICS);
	else
	  f3d_lcd_setAddrWindow ((i-cnt),0,(i-cnt),ST7735_width,MADCTLGRAPHICS);
	for (j = 0; j < br; j+=3){
	  char r = Buff[j]>>3;
	  char g = Buff[j+1]>>2;
	  char b = Buff[j+2]>>3;
	  if(level)
	    d1[ST7735_width-(j/3)-1] = ((r << 11) | (g << 5) | b);
	    //f3d_lcd_drawPixel(128-(i-cnt), 160-(j/3), d1[j/3]);
	  else
	    d1[j/3] = ((r << 11) | (g << 5) | b);
	    //f3d_lcd_drawPixel(i-cnt, (j/3), d1[j/3]);
	}
	f3d_lcd_pushColor(d1, ST7735_width);/* Type the data */ 
      }else
	cnt++;
    }
  }
  if (rc) die(rc);
  rc = f_close(&Fil);
  if (rc) die(rc);
}

void read(char file[]){
  BYTE Buff[128];		/* File read buffer */
  FRESULT rc;			/* Result code */
  UINT bw, br;
  int i;
   f_mount(0, &Fatfs);		/* Register volume work area (never fails) */

   //printf("\nOpen an existing file (message.txt).\n");
    rc = f_open(&Fil, file, FA_READ);
  if (rc) die(rc);
  for (;;) {
    rc = f_read(&Fil, Buff, sizeof Buff, &br);	/* Read a chunk of file */
    if (rc || !br) break;			/* Error or end of file */
  //for (i = 0; i < br; i++)		        /* Type the data */
      //  putchar(Buff[i]);
  }
  if (rc) die(rc);
  rc = f_close(&Fil);
  f3d_lcd_drawString(0,0,file,WHITE,RED);
  f3d_lcd_drawString(0,10,Buff,WHITE,RED);
  if (rc) die(rc);
}

void draw_menu(int option){
  int i;
  f3d_lcd_drawString(0,0,"Root:",WHITE,RED);
   for(i=0;i<numOfFiles;i++){
    f3d_lcd_drawString(0,(i+1)*10,files[i].name,WHITE,((option==i)? BLACK: RED));
  }
}

void accel(int option){
  //Variables
  int led_no;
  float adata[3],mdata[3];
  char str[40];
  float pitch, roll, p_angle, r_angle, xa, ya, za,xm,ym,zm, xh,yh, heading;
  //Read Accel and Magnetometer values
  f3d_accel_read(adata);
  //f3d_mag_read(mdata);
  xa  = adata[0];
  ya = adata[1];
  za = adata[2];
  //Calculation of pitch. roll and heading
  pitch = atan2(xa, (sqrt(pow(ya,2)+pow(za,2))));
  roll = atan2(ya, sqrt(pow(xa,2)+pow(za,2)));
  if(!round(roll) && round(pitch) && ((orientation != portrait) || 
                                     ((level == 0) && (round(pitch) < 0 )) || 
                                     ((level == 1) && (round(pitch) > 0 )))){
    orientation = portrait;
    if(round(pitch) > 0)
      level = 0;
    else
      level = 1;
    char *ind = strrchr(files[option].name, '.');
    if(stricmp(ind+1,"bmp")==0){
      f3d_lcd_fillScreen2(RED);
      readImage(files[option].name);
    }
  }
  else if(!round(pitch) && round(roll) && ((orientation != landscape) || 
                                          ((level == 0) && (round(roll) < 0 )) || 
                                          ((level == 1) && (round(roll) > 0 )))){
    orientation = landscape;
    if(round(roll) > 0)
      level = 0;
    else
      level = 1;
    char *ind = strrchr(files[option].name, '.');
    if(stricmp(ind+1,"bmp")==0){
      f3d_lcd_fillScreen2(RED);
      readImage(files[option].name);
    }
  }
  
}


int main(void) { 
  int i;
  main_toggle = 1;
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  f3d_uart_init();
  f3d_lcd_init();
  f3d_delay_init();
  f3d_rtc_init();
  f3d_i2c1_init();
  delay(10);
  f3d_accel_init();
  delay(10);
  f3d_mag_init();
  f3d_nunchuk_init();
  delay(10);
  readFile();
  int option = 0;
  draw_menu(option);
  nunchuk_t temp;
  while (1){
    f3d_nunchuk_read(&temp);
    if(!main_toggle){
      accel(option);
    }
    if(temp.c && main_toggle){
      main_toggle = 0;
      f3d_lcd_fillScreen2(RED);
      main_toggle = 0;
      char *ind = strrchr(files[option].name, '.');
      if(stricmp(ind+1,"bmp")==0){
	readImage(files[option].name);
      }
      else if(stricmp(ind+1,"txt")==0){
	read(files[option].name);
      }
      else{
	f3d_lcd_drawString(0,10,"Cannot Read. Invalid File Format!",WHITE,RED);
      }
    }
    if(temp.z && !main_toggle){
      f3d_lcd_fillScreen2(RED);
      main_toggle = 1;
      draw_menu(option);
    }
    if(main_toggle && temp.jy == 0x00){
      ++option;
      if(option == numOfFiles){
	option = 0;
      }
      draw_menu(option);
    }
    if(main_toggle && temp.jy == 0xff){
      --option;
      if(option == -1){
	option = numOfFiles-1;
      }
      draw_menu(option);
    }
    delay(100);
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
