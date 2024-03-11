/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include "adc/adc.h"

#define SIMULATOR 1

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif


int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	
  LCD_Initialization();
	
  
	TP_Init();
	TouchPanel_Calibrate();
	ADC_init();
	
	LCD_Clear(White);
	Draw_Skull(113,130,Black);
	Draw_Writings();
	
	init_timer(0, 0, 0, 3, 0x005F5E10);   //TIMER0 MR0 un quarto di secondo
	init_timer(1, 0, 0, 3, 0x017D7840);   //TIMER1 MR0 un secondo
	init_timer(2, 0, 0, 3, 0x07735940);		//TIMER2 MR0 5 secondi
	
	enable_timer(1);
	enable_timer(2);

	
	joystick_init();											/* Joystick Initialization            */
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
	enable_RIT();													/* RIT enabled												*/
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);		

	LPC_GPIO0->FIODIR |= (1<<26);					//aggiunti per speaker 
	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
