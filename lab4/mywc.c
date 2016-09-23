/***************************************************************
 * mywc.c - Word Count Program
 *
 * Authors: Srikanth Kanuri (srkanuri)
 *          Brent Hall (hallba)
 * Date Created: 09/23/2016
 * Last Modified by: Srikanth Kanuri
 * Date Last Modified: 09/23/2016
 * Assignment: Lab4
 * Part of: Lab4
 ***************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

void mywc(void) {
  int c;
  int numChar = 0;
  int numWords = 0;
  int numLines = 0;
  while((c = getchar()) != 0x1b) {
    numChar++;
    if(c == ' ' || c == '\n' || c == '\t') numWords++;
    if(c == '\n') numLines++;
  }
  printf("%d %d %d\n", numChar, numWords,  numLines);
}
