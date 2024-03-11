/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include "../RIT/RIT.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

volatile int hour=0;
volatile int sec=0;
volatile int min=0;
volatile int flagAnimation=0;
volatile int flagMeal=2;

volatile int happiness=4;
volatile int satiety=4;
volatile int Death_Flag=0;
volatile int contatore=0;
volatile int contcuddle=0;
volatile int contrun=0;
volatile int runaway=0;

extern int cuddle;
extern double volumeexit;

char orario[20];

uint16_t SinTable[45] =                                       
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};

const int freq[8]={2120,1890,1684,1592,1417,1263,1125,1062};

void TIMER0_IRQHandler (void)
{ 
	
	/* Match register 0 interrupt service routine */
	if (LPC_TIM0->IR & 01)
	{
		if(cuddle==0 && runaway==0){
			if(flagMeal){
			//animazione per meal
				
				switch(contatore){
					
					case 0:
						
						Draw_Meal();
				
						Draw_Skull(113,130,White);	//Cancello base position
				
						Draw_Animation(White, White);	
				
						Draw_Skull(80,130, Black);	//intermedio
					
						contatore++;
					
						reset_timer(3);
						init_timer(3,0,0,3,freq[1]);
						enable_timer(3);
						
						break;
					
					case 1:
						
						Draw_Skull(80,130, White);	//cancello intermedio
				
						Cancel_MealSnack(0);
				
						Draw_Skull(35,130, Black);	//final position
						
						contatore++;
					
						reset_timer(3);
						init_timer(3,0,0,3,freq[5]);
						enable_timer(3);
						
						break;
					
					case 2:
						
						Draw_Skull(35,130, White);	//final position
				
						Draw_Skull(80,130, Black);	//intermedio
						
						contatore++;
					
						reset_timer(3);
						init_timer(3,0,0,3,freq[1]);
						enable_timer(3);
						
						break;
					
					case 3:
						
						Draw_Skull(80,130, White);	//cancello intermedio
				
						Draw_Skull(113,130,Black);	//base position
					
						reset_timer(3);
						init_timer(3,0,0,3,freq[5]);
						enable_timer(3);
						
						switch(satiety){
						case 1:
							LCD_DrawRectangle(180,52,6,12,Black);
							satiety++;
						
							break;
						case 2:
							LCD_DrawRectangle(188,52,6,12,Black);
							satiety++;
							break;
						case 3:
							LCD_DrawRectangle(196,52,6,12,Black);
							satiety++;
							break;
						default:
							break;
						}
						
						disable_timer(0);
						contatore=0;
						enable_RIT();
						enable_timer(1);
						enable_timer(2);
						break;
					default:
						break;
				}
				}else{
				//animazione per snack
					
						switch(contatore){
						
						case 0:
							
							Draw_Snack();
				
							Draw_Skull(113,130,White);	//Cancello base position
				
							Draw_Animation(White, White);	
				
							Draw_Skull(140,130, Black);	//intermedio
						
							contatore++;
						
							reset_timer(3);
							init_timer(3,0,0,3,freq[3]);
							enable_timer(3);
							
							break;
						
						case 1:
							
							Draw_Skull(140,130, White);	//cancello intermedio
				
							Cancel_MealSnack(1);
				
							Draw_Skull(180,130, Black);	//final position
				
							
							contatore++;
						
							reset_timer(3);
							init_timer(3,0,0,3,freq[7]);
							enable_timer(3);
							
							break;
						
						case 2:
							
							Draw_Skull(180,130, White);	//final position
				
							Draw_Skull(140,130, Black);	//intermedio
						
							contatore++;
						
							reset_timer(3);
							init_timer(3,0,0,3,freq[3]);
							enable_timer(3);
							
							break;
						
						case 3:
							
							Draw_Skull(140,130, White);	//cancello intermedio
				
							Draw_Skull(113,130,Black);	//base position
						
							reset_timer(3);
							init_timer(3,0,0,3,freq[7]);
							enable_timer(3);
							
							switch(happiness){
								case 1:
									LCD_DrawRectangle(40,52,6,12,Black);
									happiness++;
						
									break;
								case 2:
									LCD_DrawRectangle(48,52,6,12,Black);
									happiness++;
									break;
								case 3:
									LCD_DrawRectangle(56,52,6,12,Black);
									happiness++;
									break;
								default:
									break;
							}
							
							disable_timer(0);
							contatore=0;
							enable_RIT();
							enable_timer(1);
							enable_timer(2);
							break;
						default:
							break;
					}
				}
		}else if(cuddle==1 && runaway==0){
			
			switch(contcuddle){
				case 0:
					Draw_Cuddle(150,110);
					contcuddle++;
				
					reset_timer(3);
					init_timer(3,0,0,3,freq[3]);
					enable_timer(3);
				
					break;
				case 1:
					
					contcuddle++;
					reset_timer(3);
					init_timer(3,0,0,3,freq[4]);
					enable_timer(3);
				
					break;
				case 2:
					LCD_DrawRectangle(145,110,40,40,White); //Cancello cuore
					contcuddle++;
					reset_timer(3);
					init_timer(3,0,0,3,freq[5]);
					enable_timer(3);
					break;
				case 3:
				
					contcuddle++;
					reset_timer(3);
					init_timer(3,0,0,3,freq[6]);
					enable_timer(3);
					break;
				case 4:
					Draw_Cuddle(60,110);
					contcuddle++;
					reset_timer(3);
					init_timer(3,0,0,3,freq[5]);
					enable_timer(3);
					break;
				case 5:
					
					contcuddle++;
					reset_timer(3);
					init_timer(3,0,0,3,freq[4]);
					enable_timer(3);
					break;
				case 6:
					LCD_DrawRectangle(50,110,40,40,White); //Cancello cuore
					contcuddle++;
					reset_timer(3);
					init_timer(3,0,0,3,freq[3]);
					enable_timer(3);
					break;
				case 7:
					reset_timer(3);
					init_timer(3,0,0,3,freq[7]);
					enable_timer(3);
				
				switch(happiness){
								case 1:
									LCD_DrawRectangle(40,52,6,12,Black);
									happiness++;
						
									break;
								case 2:
									LCD_DrawRectangle(48,52,6,12,Black);
									happiness++;
									break;
								case 3:
									LCD_DrawRectangle(56,52,6,12,Black);
									happiness++;
									break;
								default:
									break;
							}
				
					contcuddle=0;
					cuddle=0;
					disable_timer(0);
					break;
			}
			
		}else if(runaway==1){
				switch(contrun){
					case 0:
						LCD_DrawRectangle(145,110,40,40,White); //Cancello cuore
						LCD_DrawRectangle(50,110,40,40,White); //Cancello cuore
						LCD_DrawRectangle(90,110,60,90,White); //Cancello skull base 113,130
						contrun++;
						reset_timer(3);
						init_timer(3,0,0,3,freq[5]);
						enable_timer(3);
						break;
					case 1:
						Draw_Hearth();
	
						Draw_Skull(160,130,Black);//Disegno Skull intermediate
						contrun++;
						reset_timer(3);
						init_timer(3,0,0,3,freq[3]);
						enable_timer(3);
						break;
					case 2:
						Draw_Skull(160,130,White);//Cancello skull intermediate
	
						Draw_Skull(210,130,Black);//Disegno Skull lontano
						contrun++;
						reset_timer(3);
						init_timer(3,0,0,3,freq[1]);
						enable_timer(3);
						break;
					case 3:
						Draw_Skull(210,130,White);//Cancello Skull lontano
	
						LCD_DrawRectangle(0,271,240,49,White);
						LCD_DrawLine(0,270, 240, 270, Red);
						contrun++;
						reset_timer(3);
						init_timer(3,0,0,3,freq[3]);
						enable_timer(3);
						break;
					case 4:
						GUI_Text(90, 290, (uint8_t *) " Reset  ", Red, White);
						reset_timer(3);
						init_timer(3,0,0,3,freq[5]);
						enable_timer(3);
					
						contcuddle=0;
						cuddle=0;
						contrun=0;
						disable_timer(0);
						runaway=0;
						break;
					
				}
		}
		LPC_TIM0->IR = 1;			/* clear interrupt flag */
	}
		/* Match register 1 interrupt service routine */
	else if(LPC_TIM0->IR & 02)
  {

		LPC_TIM0->IR =  2 ;			/* clear interrupt flag */	
	}
	/* Match register 2 interrupt service routine */
	else if(LPC_TIM0->IR & 4)
  {
		
		LPC_TIM0->IR =  4 ;			/* clear interrupt flag */	
	}
		/* Match register 3 interrupt service routine */
	else if(LPC_TIM0->IR & 8)
  {
	 
		LPC_TIM0->IR =  8 ;			/* clear interrupt flag */	
	}
  return;
}



/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	
	if(sec<59){
			sec++;
	}else{
			sec=0;
			if(min<59){
					min++;
			}else{
					min=0;
					hour++;
			}
	}
		
	sprintf(orario,"%02d:%02d:%02d", hour,min,sec);
		
	GUI_Text(105, 10, (uint8_t *) orario, Black, White);
		
  /* Match register 0 interrupt service routine */
	if (LPC_TIM1->IR & 01)
	{
		if(Death_Flag==0){
			
			if(flagAnimation){
				Draw_Animation(Black,White);
				flagAnimation--;
			}else{
				Draw_Animation(Red,Red);
				flagAnimation++;
			}
		}
		LPC_TIM1->IR = 1;			/* clear interrupt flag */
	}
		/* Match register 1 interrupt service routine */
	else if(LPC_TIM1->IR & 02)
  {
		
		LPC_TIM1->IR =  2 ;			/* clear interrupt flag */	
	}
	/* Match register 2 interrupt service routine */
	else if(LPC_TIM1->IR & 4)
  {
		
		LPC_TIM1->IR =  4 ;			/* clear interrupt flag */	
	}
		/* Match register 3 interrupt service routine */
	else if(LPC_TIM1->IR & 8)
  {
	 
		LPC_TIM1->IR =  8 ;			/* clear interrupt flag */	
	}
  return;
}

/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 2 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER2_IRQHandler (void)
{
  //LPC_TIM2->IR = 1;			/* clear interrupt flag */
	
	/* Match register 0 interrupt service routine */
	if (LPC_TIM2->IR & 01)
	{
		
		switch(happiness){
			case 1:
				LCD_DrawRectangle(32,52,6,12,White);
				happiness--;
				break;
			case 2:
				LCD_DrawRectangle(40,52,6,12,White);
				happiness--;
				break;
			case 3:
				LCD_DrawRectangle(48,52,6,12,White);
				happiness--;
				break;
			case 4:
				LCD_DrawRectangle(56,52,6,12,White);
				happiness--;
				break;
			default:
				break;
		}
		
	
		switch(satiety){
			case 1:
				LCD_DrawRectangle(172,52,6,12,White);
				satiety--;
				break;
			case 2:
				LCD_DrawRectangle(180,52,6,12,White);
				satiety--;
				break;
			case 3:
				LCD_DrawRectangle(188,52,6,12,White);
				satiety--;
				break;
			case 4:
				LCD_DrawRectangle(196,52,6,12,White);
				satiety--;
				break;
			default:
				break;
		}
		
		if(happiness==0 || satiety==0){
			disable_timer(1);
			disable_timer(2);
				
			Death_Flag=1;
				
			runaway=1;
			
			enable_timer(0);
			//Reset_Animation();
		}
		
		LPC_TIM2->IR = 1;			/* clear interrupt flag */
	}
		/* Match register 1 interrupt service routine */
	else if(LPC_TIM2->IR & 02)
  {
		LPC_TIM2->IR =  2 ;			/* clear interrupt flag */	
	}
	/* Match register 2 interrupt service routine */
	else if(LPC_TIM2->IR & 4)
  {
		
		LPC_TIM2->IR =  4 ;			/* clear interrupt flag */	
	}
		/* Match register 3 interrupt service routine */
	else if(LPC_TIM2->IR & 8)
  {
	 
		LPC_TIM2->IR =  8 ;			/* clear interrupt flag */	
	}
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER3_IRQHandler (void)
{
		
  /* Match register 0 interrupt service routine */
	if (LPC_TIM3->IR & 01)
	{
		static int ticks=0;
		static int counters=0;
		
		static double test;
		
		
		/* DAC management */
		//1.3 valore massimo per alzare volume
		//LPC_DAC->DACR = SinTable[ticks]<<6;
		
		test = SinTable[ticks]<<6;
		LPC_DAC->DACR = test*volumeexit;
		ticks++;
		if(ticks==45 && counters<50){
			ticks=0;
			counters++;
			
		}else if(ticks==45 && counters==50){
			ticks=0;
			counters=0;
			disable_timer(3);
		}
		
		LPC_TIM3->IR = 1;			/* clear interrupt flag */
	}
		/* Match register 1 interrupt service routine */
	else if(LPC_TIM3->IR & 02)
  {
		
		LPC_TIM3->IR =  2 ;			/* clear interrupt flag */	
	}
	/* Match register 2 interrupt service routine */
	else if(LPC_TIM3->IR & 4)
  {
		
		LPC_TIM3->IR =  4 ;			/* clear interrupt flag */	
	}
		/* Match register 3 interrupt service routine */
	else if(LPC_TIM3->IR & 8)
  {
	 
		LPC_TIM3->IR =  8 ;			/* clear interrupt flag */	
	}
  return;
}
/******************************************************************************
**                            End Of File
******************************************************************************/
