/***************************************************************
  * f3d_uart.c - contains the initialization basic i/o functions for UART
  *
  * Authors: Srikanth Kanuri (srkanuri)
  *          Raghavendra Nataraj (natarajr)
  * Date Created: 09/23/2016
  * Last Modified by: Srikanth Kanuri
  * Date Last Modified: 11/17/2016
  * Assignment: Lab4, Lab5, Lab10, Lab11
  * Part of: Lab4, Lab5, Lab10, Lab11
 ***************************************************************/

#include <stm32f30x.h>
#include <stm32f30x_misc.h>
#include <f3d_uart.h>
#include <queue.h>

static int TxPrimed = 0;
int RxOverflow = 0;
queue_t rxbuf, txbuf;

void flush_uart(void) {
  USART_ITConfig(USART1,USART_IT_TXE,ENABLE);
}

//Function name: f3d_uart_init
//Description: The initialization function for USART
//Parameters: None
//Return Type: void
void f3d_uart_init(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC,&GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOC,4,GPIO_AF_7);
  GPIO_PinAFConfig(GPIOC,5,GPIO_AF_7);

  USART_InitTypeDef USART_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  USART_StructInit(&USART_InitStructure);
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1 ,&USART_InitStructure);
  USART_Cmd(USART1 , ENABLE);

  // Initialize the rx and tx queues
  init_queue(&rxbuf);
  init_queue(&txbuf);

  // Setup the NVIC priority and subpriority
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x08;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x08;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // Enable the RX interrupt
  USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
}


//Function name: putchar
//Description: sends a character for print
//Parameters: int
//Return Type: int
int putchar(int c) {
  //while (USART_GetFlagStatus(USART1,USART_FLAG_TXE) == (uint16_t)RESET);
  //USART_SendData(USART1, c);
  //return 0;
  while(!enqueue(&txbuf , c));
  if (!TxPrimed) {
    TxPrimed = 1;
    flush_uart();
  }
}

//Function name: getchar
//Description: gets a character
//Parameters: None
//Return Type: int
int getchar(void) {
  //while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == (uint16_t)RESET);
  //int c = USART_ReceiveData(USART1);
  //return c;
  uint8_t data;
  while(!dequeue(&rxbuf , &data));
  return data;
}

//Function name: getchar
//Description: gets a character - doesnt wait for input
//Parameters: None
//Return Type: int
int getch(void) {
  if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != (uint16_t)RESET){
    int c = USART_ReceiveData(USART1);
    return c;
  }
  return 0;
}

//Function name: putstring
//Description: sends a string for print
//Parameters: String pointer
//Return Type: void
void putstring(char *s) {
  while(*s != '\0'){
    putchar(*s);
    s++;
  }
}


void USART1_IRQHandler(void){
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
    uint8_t   data;
    // buffer the data (or toss it if there's no room
    // Flow control will prevent this
    data = USART_ReceiveData(USART1) & 0xff;
    if(!enqueue(&rxbuf , data))
      RxOverflow = 1;
  }
  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET){
    uint8_t data;
    /* Write one byte to the transmit data register */
    if(dequeue(&txbuf , &data)){
      USART_SendData(USART1, data);
    }
    else{
      // if we have nothing to send, disable the interrupt
      // and wait for a kick
      USART_ITConfig(USART1, USART_IT_TXE , DISABLE);
      TxPrimed = 0;
    }
  }
}

/* f3d_uart.c ends here */
