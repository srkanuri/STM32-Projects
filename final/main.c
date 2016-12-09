/***************************************************************
 * main.c - Main program for Lab8 - Nunchuk
 *
 * Author: Raghavendra Nataraj (natarajr)
 *         Srikanth Kanuri (srkanuri)
 * Date Created: 11/21/2016
 * Last Modified by: Srikanth Kanuri, Raghavendra Nataraj
 * Date Last Modified: 12/09/2016
 * Assignment: Final Project
 * Part of: Final Project
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
#include <f3d_delay.h>
#include <f3d_systick.h>
#include "brick_game.h"
#include <ff.h>
#include <diskio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bmp.h"

#define TIMER 20000
#define AUDIOBUFSIZE 128

extern uint8_t Audiobuf[AUDIOBUFSIZE];
extern int audioplayerHalf;
extern int audioplayerWhole;
extern int exit_interrupt;
extern int play_g;

int is_free = 1, pause = 1;
int lane_picker[] = {L1, L2, L3, L4, L5};
int speed = 5, car_width = 15, car_length = 20;
car user;
int playing_audio = 0;
int inc, score = 0;
car ocars[MAX_CARS];
uint16_t blank[300];
int level = 1;
FATFS Fatfs;		/* File system object */
FIL Fil;		/* File object */
BYTE Buff[5];		/* File read buffer */
FRESULT rc;			/* Result code */
DIR dir;			/* Directory object */
FILINFO fno;			/* File information object */
UINT bw, br;
unsigned int retval;
unsigned int hiscore;
enum ORIENT { portrait = 0, landscape = 1 };
static enum ORIENT orientation = portrait;
FIL fid;		/* File object */
int ret;

void die (FRESULT rc) {
  printf("Failed with rc=%u.\n", rc);
  while (1);
}

//Function to read the disk for the file
void readckhd(FIL *fid, struct ckhd *hd, uint32_t ckID) {
  f_read(fid, hd, sizeof(struct ckhd), &ret);
  printf("Size read %d, Actual Size %d",ret,sizeof(struct ckhd));
  if (ret != sizeof(struct ckhd))
    exit(-1);
  if (ckID && (ckID != hd->ckID))
    exit(-1);
}

//Draws the Score Board in Gameplay - One time run
void draw_score_board(){
  int j;
  for(j = 0; j<160; j++)
    f3d_lcd_drawPixel(SCOREBOARD, j, WHITE);
  f3d_lcd_fillScoreBoard(SCOREBOARD+1, RED);
  f3d_lcd_drawString(SCOREBOARD+1,40,"LVL:",WHITE,RED);
  f3d_lcd_drawString(SCOREBOARD+1,50,"1",WHITE,RED);
  f3d_lcd_drawString(SCOREBOARD+1,100,"SCR:",WHITE,RED);
  f3d_lcd_drawString(SCOREBOARD+1,110,"0",WHITE,RED);
}

//Draws the end game menu
void draw_menu(int option, int prev){
  if(option != prev){
    if(option == 0 || prev == 0 || prev == -1)
      f3d_lcd_drawString(0,0,"Start Game",WHITE,((option==0)? BLACK: RED));
    if(option == 1 || prev == 1 || prev == -1)
      f3d_lcd_drawString(0,10,"Instructions",WHITE,((option==1)? BLACK: RED));
    if(option == 2 || prev == 2 || prev == -1){
      f3d_lcd_drawString(0,20,"Hi Score",WHITE,((option==2)? BLACK: RED));
      f3d_lcd_drawString(50,20,"          ",RED,RED);
    }
    if(option == 3 || prev == 3 || prev == -1)
      f3d_lcd_drawString(0,30,"Creators",WHITE,((option==3)? BLACK: RED));
    //delay(100);
  }
}

//Prints the creators info
void print_credits(){
  f3d_lcd_fillScreen2(RED);
  f3d_lcd_drawString(0,0,"Authors:",WHITE,RED);
  f3d_lcd_drawString(0,10,"Raghavendra Nataraj",WHITE,RED);
  f3d_lcd_drawString(0,20,"Srikanth Kanuri",WHITE,RED);
  f3d_lcd_drawString(0,50,"Thanks:",WHITE,RED);
  f3d_lcd_drawString(0,60,"Prof. Vel Melbasa",WHITE,RED);
  f3d_lcd_drawString(0,70,"Prof. Geoffrey Brown",WHITE,RED);
  f3d_lcd_drawString(0,80,"Sean Peters",WHITE,RED);
  f3d_lcd_drawString(0,90,"Wendy Kwok",WHITE,RED);
  f3d_lcd_drawString(0,100,"Gleb Alexeev",WHITE,RED);
  f3d_lcd_drawString(0,110,"Colin Thackston",WHITE,RED);
  f3d_lcd_drawString(0,120,"Mohan Chaturvedula",WHITE,RED);
  f3d_lcd_drawString(0,130,"Shridivya Sharma",WHITE,RED);
  f3d_lcd_drawString(0,140,"Wonyong Ha",WHITE,RED);
  f3d_lcd_drawString(0,150,"Rakshesh Shah",WHITE,RED);
}

//Prints instructions of game
void print_instructions(){
  f3d_lcd_fillScreen2(RED);
  f3d_lcd_drawString(0,0,  "---------------------",WHITE,RED);
  f3d_lcd_drawString(0,10, "    INSTRUCTIONS     ",WHITE,RED);
  f3d_lcd_drawString(0,20, "---------------------",WHITE,RED);
  f3d_lcd_drawString(0,30, ".There are 10 levels.",WHITE,RED);
  f3d_lcd_drawString(0,40, ".Each lvl is 90 secs.",WHITE,RED);
  f3d_lcd_drawString(0,50, ".Avoid hitting cars.",WHITE,RED);
  f3d_lcd_drawString(0,60, ".LEDs - Level Timer.",WHITE,RED);
  f3d_lcd_drawString(0,70, ".Each LED is 10 secs.",WHITE,RED);
  f3d_lcd_drawString(0,80, ".Pause with User Btn.",WHITE,RED);
  f3d_lcd_drawString(0,90, ".Nunchuk to move car.",WHITE,RED);
  f3d_lcd_drawString(0,100,".Z to accelerate car.",WHITE,RED);
  f3d_lcd_drawString(0,110,".Tilt also navigates.",WHITE,RED);
  f3d_lcd_drawString(0,120,"---------------------",WHITE,RED);
  f3d_lcd_drawString(0,130,"--!! GET  RACING !!--",WHITE,RED);
  f3d_lcd_drawString(0,140,"---------------------",WHITE,RED);
}

//Fetches the hi score from the SD card
void get_hi_score(){
  printf("In get hi score\n");
  rc = f_open(&Fil, "_brick.hs", FA_READ);
  if(rc){
    rc = f_open(&Fil, "_brick.hs", FA_READ | FA_CREATE_ALWAYS);
    if (rc) die(rc);
    hiscore = 0;
  }
  else{
    rc = f_read(&Fil, Buff, sizeof Buff, &br);
    if (rc) die(rc);
    printf("Hi Score %s",Buff);
    hiscore = atoi(Buff);
  }
  rc = f_close(&Fil);
  if (rc) die(rc);
}

//Set the hi score from the SD card
void set_hi_score(int value){
  char val[5];
  rc = f_open(&Fil, "_brick.hs", FA_WRITE);
  if (rc) die(rc);
  sprintf(val, "%d", value);
  rc = f_write(&Fil, val, 5, &bw);
  if (rc) die(rc);
  rc = f_close(&Fil);
  if (rc) die(rc);
}

//Reads the image from SD card
void readImage(char file[]){
  BYTE Buff[384];		/* File read buffer */
  unsigned short d1[128];
  struct bmpfile_header head;
  BITMAPINFOHEADER info;
  struct bmpfile_magic magic;
  FRESULT rc;			/* Result code */
  UINT bw, br;
  int i;
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

//Plays the Audio file from the SD card
void playAudio(char* file_name){
  FRESULT rc;
  f_mount(0, &Fatfs); // Register volume work area

  printf("\nOpen %s\n",file_name);
  rc = f_open(&fid, file_name, FA_READ);

  if (!rc) {
    struct ckhd hd;
    uint32_t  waveid;
    struct fmtck fck;

    readckhd(&fid, &hd, 'FFIR');

    f_read(&fid, &waveid, sizeof(waveid), &ret);
    if ((ret != sizeof(waveid)) || (waveid != 'EVAW')){
      return;
    }

    readckhd(&fid, &hd, ' tmf');

    f_read(&fid, &fck, sizeof(fck), &ret);

    // skip over extra info

    if (hd.cksize != 16) {
      printf("extra header info %d\n", hd.cksize - 16);
      f_lseek(&fid, hd.cksize - 16);
    }

    printf("audio format 0x%x\n", fck.wFormatTag);
    printf("channels %d\n", fck.nChannels);
    printf("sample rate %d\n", fck.nSamplesPerSec);
    printf("data rate %d\n", fck.nAvgBytesPerSec);
    printf("block alignment %d\n", fck.nBlockAlign);
    printf("bits per sample %d\n", fck.wBitsPerSample);

    // now skip all non-data chunks !

    while(1){
      readckhd(&fid, &hd, 0);
      printf("Read Check D complete %c\n",hd.ckID);
      if (hd.ckID == 'atad')
	break;
      f_lseek(&fid, hd.cksize);
      printf("%u moved, Lseek complete %p\n",hd.cksize,fid);
    }
    printf("Before Play\n\n");
    printf("Samples %d\n", hd.cksize);
    printf("Play now\n\n");

    // Play it !

    //audioplayerInit(fck.nSamplesPerSec);

    f_read(&fid, Audiobuf, AUDIOBUFSIZE, &ret);
    hd.cksize -= ret;
    audioplayerStart();
    playing_audio = 1;
    while (hd.cksize && !exit_interrupt) {
      int next = hd.cksize > AUDIOBUFSIZE/2 ? AUDIOBUFSIZE/2 : hd.cksize;
      if (audioplayerHalf) {
	if (next < AUDIOBUFSIZE/2)
	  bzero(Audiobuf, AUDIOBUFSIZE/2);
	f_read(&fid, Audiobuf, next, &ret);
	hd.cksize -= ret;
	audioplayerHalf = 0;
      }
      if(!hd.cksize)
	break;
      if (audioplayerWhole) {
	if (next < AUDIOBUFSIZE/2)
	  bzero(&Audiobuf[AUDIOBUFSIZE/2], AUDIOBUFSIZE/2);
	f_read(&fid, &Audiobuf[AUDIOBUFSIZE/2], next, &ret);
	hd.cksize -= ret;
	audioplayerWhole = 0;
      }
    }
    exit_interrupt = 0;
    playing_audio = 0;
    audioplayerStop();
  }

  printf("\nClose the file.\n");
  rc = f_close(&fid);

  if (rc) die(rc);
}

//Erase the car from screen
void erase_car(car val){
  int i = 0, j = 0, x = val.prev_x, y = val.prev_y;
  f3d_lcd_setAddrWindow(x,y,x+14,y+19,MADCTLGRAPHICS);
  f3d_lcd_pushColor(blank,300);
}

//Draw car on screen
void draw_car(car val){
  int i = 0, j = 0, x = val.x, y = val.y, color = val.type;
  uint16_t buff[300];
  for(i = 0; i<15; i++){
    for(j = 0; j<20; j++){
      if(j>=15){
	if(i < 5 || i >= 10)
	  buff[(j*car_width)+i]=color;
	else
	  buff[(j*car_width)+i]=BLACK;
      }
      if(j>=10 && j<15){
	if(i >= 5 && i < 10)
	  buff[(j*car_width)+i]=color;
	else
	  buff[(j*car_width)+i]=BLACK;
      }
      if(j>=5 && j<10)
	buff[(j*car_width)+i]=color;
      if(j<5){
	if(i >= 5 && i < 10)
	  buff[(j*car_width)+i]=color;
	else
	  buff[(j*car_width)+i]=BLACK;
      }
    }
  }
  f3d_lcd_setAddrWindow(x,y,x+14,y+19,MADCTLGRAPHICS);
  f3d_lcd_pushColor(buff,300);
}

//Redraw car on screen (calls draw and erase)
void redraw_car(car *val){
  erase_car(*val);
  val->prev_x = val->x;
  val->prev_y = val->y;
  draw_car(*val);
}

//Move cars on screen (Both User and Opponent cars)
int move_cars(){
  int i=0, collision_x=0, collision_y=0;
  if(user.x != user.prev_x || user.y != user.prev_y){
    redraw_car(&user);
  }
  for(;i<MAX_CARS;i++){
    collision_x = 0;
    collision_y = 0;
    if(ocars[i].used){
      ocars[i].prev_x = ocars[i].x;
      ocars[i].prev_y = ocars[i].y;
      ocars[i].y += speed;
      if(ocars[i].y >= 165){
	ocars[i].used = 0;
	score+=inc;
	char scr[3];
	sprintf(scr,"%d",score);
	f3d_lcd_drawString(SCOREBOARD+1,110,scr,WHITE,RED);
      }
      else
	redraw_car(&ocars[i]);
      //Check Collision condition
      if(user.x > ocars[i].x && user.x < ocars[i].x+car_width)
	collision_x = 1;
      if(user.x+car_width > ocars[i].x && user.x+car_width < ocars[i].x+car_width)
	collision_x = 1;
      if(user.y > ocars[i].y && user.y < ocars[i].y+car_length)
	collision_y = 1;
      if(user.y+car_length > ocars[i].y && user.y+car_length < ocars[i].y+car_length)
	collision_y = 1;
      if(collision_y && user.x==ocars[i].x && user.x+car_width==ocars[i].x+car_width)
	collision_x = 1;
      if((collision_x && collision_y)){
	printf("Crash details %d,%d,%d,%d",ocars[i].x,user.x,ocars[i].y,user.y);
	return 1;
      }
    }
  }
  return 0;
}

//Generate new cars on screen
void gen_cars(){
  int ncars = (rand()%3)+1, i = 0;
  for(;i<ncars;i++){
    int lane = rand()%5, k = 0;
    for(;k<MAX_CARS;k++){
      if(!ocars[k].used)
	break;
    }
    ocars[k].x = ocars[i].prev_x = lane_picker[lane];
    ocars[k].y = ocars[i].prev_y = -20;
    ocars[k].used = 1;
    ocars[k].type = BLUE;
    draw_car(ocars[k]);
  }
}

//Play Brick Racing Game -- Main function for game play
void play_game(){
  int i, duration = 100, option = 0,  main_toggle = 1;
  char str[50];
  pause = 0;
  f3d_lcd_fillScreen2(BLACK);
  draw_score_board();
  //f3d_nunchuk_read(&temp);
  int k = 15;
  user.x = 0;
  user.y = user.prev_y = 140;
  user.type = WHITE;
  draw_car(user);
  for(i=0;i<MAX_CARS;i++){
    ocars[i].used = 0;
    ocars[i].type = BLUE;
    ocars[i].x=0;
    ocars[i].y=0;
  }
  get_hi_score();
  while(1){
    printf("Pause %d:\n", pause);
    if(!pause){
      if(k == 15){
	printf("Gen Cars Called");
	gen_cars();
	k = 0;
      }
      printf("Z value %d\n", temp.z);
      if(temp.z){
	delay(20);
	inc = 2;
      }else{
	delay(duration/level);
	inc = 1;
      }
      printf("%d k",k);
      if(move_cars()){
	//play_g = 0;
	//printf("Crash at %d, %d\n",user.x, user.y);
	f3d_lcd_fillScreen2(BLACK);
	f3d_lcd_drawString(20,80,"GAME OVER !!!",RED,BLACK);
	playAudio("crash.wav");
	readImage("CAR.BMP");
	pause = 1;
	if(score > hiscore)
	  set_hi_score(score);
	f3d_lcd_fillMenuScreen(RED, 0, 40);
	draw_menu(0, -1);
	while(1){
	  f3d_led_all_off();
	  delay(100);
	  f3d_led_all_on();
	  delay(100);
	  printf("%d\n",option);
	  if(main_toggle && temp.jy == 0x00){
	    i = option;
	    ++option;
	    printf("%d\n",option);
	    if(option == 4)
	      option = 0;
	    draw_menu(option, i);
	  }
	  if(main_toggle && temp.jy == 0xff){
	    i = option;
	    --option;
	    if(option == -1)
	      option = 3;
	    draw_menu(option, i);
	  }
	  if(temp.c && main_toggle){
	    main_toggle = 0;
	    if(!option){
	      play_g = 1;
	      exit_interrupt = 1;
	      Reset_Handler();
	    }else{
	      switch(option){
	      case 1: print_instructions();
		break;
	      case 2: main_toggle = 1;
		get_hi_score();
		sprintf(str, ": %d",hiscore);
		f3d_lcd_drawString(50,20,str,WHITE,BLACK);
		break;
	      case 3: print_credits();
	      }
	    }
	  }
	  if(temp.z && !main_toggle){
	    main_toggle = 1;
	    f3d_lcd_fillScreen2(BLACK);
	    readImage("car.BMP");
	    f3d_lcd_fillMenuScreen(RED, 0, 40);
	    draw_menu(option, -1);
	  }
	}
	break;
      }
      k++;
    }
  }
}

//Main program
int main(void) {
  int level = 153;
  // If you have your inits set up, this should turn your LCD screen red
  f3d_timer2_init();
  f3d_dac_init();
  f3d_delay_init();
  f3d_rtc_init();
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
  f3d_nunchuk_init();
  delay(10);
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  f3d_systick_init();
  printf("Before mounting FS\n");
  f_mount(0, &Fatfs);
  f3d_lcd_fillScreen2(BLACK);
  readImage("WELCOME.BMP");
  f3d_lcd_fillMenuScreen(WHITE, level, ST7735_height-1);
  //draw_menu(0, -1);
  f3d_lcd_drawString(0,level,"Press c to Start Game",BLACK, WHITE);
  while(!play_g)
    playAudio("welcome.wav");
  play_game();
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
/* Infinite loop */
/* Use GDB to find out why we're here */

}
#endif

/* main.c ends here */
