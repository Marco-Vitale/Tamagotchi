/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../GLCD/GLCD.h" 
#include "../timer/timer.h"
#include "../TouchPanel/TouchPanel.h"
#include "../adc/adc.h"
//#include "../led/led.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

volatile int down=0;
//extern char led_value;

extern int happiness;
extern int satiety;
extern int hour;
extern int min;
extern int sec;
extern int Death_Flag;
extern int flagMeal;

volatile int cuddle=0;

const int frequences[8]={2120,1890,1684,1592,1417,1263,1125,1062};

void RIT_IRQHandler (void)
{					
	//static int select=0;
	//static int position=0;	
	
	static int J_left = 0;
	static int J_right = 0;
	static int J_select = 0;
	
	ADC_start_conversion();		//Conversione per potenziometro
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		/* Joytick Left pressed p1.27 --> using J_left		due to emulator issues*/
		J_left++;
		switch(J_left){
			case 1:
				
				if(!Death_Flag){							//evitare le animazioni in caso di morte 
					
					//Update selection
					
					reset_timer(3);
					init_timer(3,0,0,3,frequences[1]);
					enable_timer(3);
					
					LCD_DrawLine(0, 270, 120, 270, Red);
					LCD_DrawLine(121, 270, 240, 270, Black);
					
					LCD_DrawLine(120, 270, 120, 320, Red);
					
					GUI_Text(30, 290, (uint8_t *) " Meal  ", Red, White);
					
					GUI_Text(150, 290, (uint8_t *) " Snack  ", Black, White);
					
					flagMeal = 1;
					
				}
				break;
			default:
				break;
		}
	}
	else{
			J_left=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		/* Joytick Right pressed p1.28 --> using J_right		due to emulator issues*/
		J_right++;
		switch(J_right){
			case 1:
				
				if(!Death_Flag){							//evitare le animazioni in caso di morte 
					
					reset_timer(3);
					init_timer(3,0,0,3,frequences[1]);
					enable_timer(3);
					//Update selection
	
					LCD_DrawLine(0, 270, 119, 270, Black);
					LCD_DrawLine(120, 270, 240, 270, Red);
					
					LCD_DrawLine(120, 270, 120, 320, Red);
					
					GUI_Text(30, 290, (uint8_t *) " Meal  ", Black, White);
					
					GUI_Text(150, 290, (uint8_t *) " Snack  ", Red, White);
				
					flagMeal=0;
					
				}
				break;
			default:
				break;
		}
	}
	else{
			J_right=0;
	}

	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
		/* Joytick Select pressed p1.25 --> using J_select		due to emulator issues*/
		J_select++;
		switch(J_select){
			case 1:
				
				//in case of death by tamagochi restart the full session
			
				if(Death_Flag){
					
					//Resetto le variabili
					
					hour=0;
					min=0;
					sec=0;
				
					happiness=4;
					satiety=4;
					Death_Flag=0;
					flagMeal=2;
				
					LCD_Clear(White);
					Draw_Skull(113,130,Black);
					Draw_Writings();
					
					enable_timer(1);
					enable_timer(2);
				}else if(flagMeal==1 || flagMeal==0){
					
					disable_timer(1);
					disable_timer(2);
					
					disable_RIT();							//disabilito l'utilizzo del joystick durante l'animazione
					
					enable_timer(0);						//utilizzo Timer0 per effettuare un animazione di almeno 1 secondo
				}
				
				break;
			default:
				break;
		}
	}
	else{
			J_select=0;
	}
	
	if(Death_Flag==0 && getDisplayPoint(&display, Read_Ads7846(), &matrix)){
		if(display.y<200 && display.y>110){
			if(display.x<150 && display.x>90){
				cuddle=1;
				enable_timer(0);
			}
		}
	}
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
