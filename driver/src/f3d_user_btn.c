/***************************************************************
 * f3d_user_btn.c - contains the init and read functions for the User Button
 *
 * Authors: Srikanth Kanuri (srkanuri)
 *          Raghavendra Nataraj (natarajr)
 * Date Created: 09/15/2016
 * Last Modified by: Srikanth Kanuri
 * Date Last Modified: 09/15/2016
 * Assignment: Lab3
 * Part of: Lab3
 ***************************************************************/
#include <stm32f30x.h>
#include <stm32f30x_gpio.h>
#include <stm32f30x_rcc.h>


// Function Name: f3d_user_btn_init
// Description: Initialization of the UserButton
// Return Type: void
void f3d_user_btn_init(void){
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

// Function Name: f3d_btn_read
// Description: reads the User Button
// Return Type: int
int user_btn_read(void){
  return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
}
