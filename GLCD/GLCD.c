/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			GLCD.c
** Descriptions:		Has been tested SSD1289、ILI9320、R61505U、SSD1298、ST7781、SPFD5408B、ILI9325、ILI9328、
**						HX8346A、HX8347A
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2012-3-10
** Version:					1.3
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			Paolo Bernardi
** Modified date:		03/01/2020
** Version:					2.0
** Descriptions:		simple arrangement for screen usage
********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "GLCD.h" 
#include "AsciiLib.h"

/* Private variables ---------------------------------------------------------*/
static uint8_t LCD_Code;

/* Private define ------------------------------------------------------------*/
#define  ILI9320    0  /* 0x9320 */
#define  ILI9325    1  /* 0x9325 */
#define  ILI9328    2  /* 0x9328 */
#define  ILI9331    3  /* 0x9331 */
#define  SSD1298    4  /* 0x8999 */
#define  SSD1289    5  /* 0x8989 */
#define  ST7781     6  /* 0x7783 */
#define  LGDP4531   7  /* 0x4531 */
#define  SPFD5408B  8  /* 0x5408 */
#define  R61505U    9  /* 0x1505 0x0505 */
#define  HX8346A		10 /* 0x0046 */  
#define  HX8347D    11 /* 0x0047 */
#define  HX8347A    12 /* 0x0047 */	
#define  LGDP4535   13 /* 0x4535 */  
#define  SSD2119    14 /* 3.5 LCD 0x9919 */

/*******************************************************************************
* Function Name  : Lcd_Configuration
* Description    : Configures LCD Control lines
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_Configuration(void)
{
	/* Configure the LCD Control pins */
	
	/* EN = P0.19 , LE = P0.20 , DIR = P0.21 , CS = P0.22 , RS = P0.23 , RS = P0.23 */
	/* RS = P0.23 , WR = P0.24 , RD = P0.25 , DB[0.7] = P2.0...P2.7 , DB[8.15]= P2.0...P2.7 */  
	LPC_GPIO0->FIODIR   |= 0x03f80000;
	LPC_GPIO0->FIOSET    = 0x03f80000;
}

/*******************************************************************************
* Function Name  : LCD_Send
* Description    : LCD写数据
* Input          : - byte: byte to be sent
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_Send (uint16_t byte) 
{
	LPC_GPIO2->FIODIR |= 0xFF;          /* P2.0...P2.7 Output */
	LCD_DIR(1)		   				    				/* Interface A->B */
	LCD_EN(0)	                        	/* Enable 2A->2B */
	LPC_GPIO2->FIOPIN =  byte;          /* Write D0..D7 */
	LCD_LE(1)                         
	LCD_LE(0)														/* latch D0..D7	*/
	LPC_GPIO2->FIOPIN =  byte >> 8;     /* Write D8..D15 */
}

/*******************************************************************************
* Function Name  : wait_delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None 
*******************************************************************************/
static void wait_delay(int count)
{
	while(count--);
}

/*******************************************************************************
* Function Name  : LCD_Read
* Description    : LCD读数据
* Input          : - byte: byte to be read
* Output         : None
* Return         : 返回读取到的数据
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) uint16_t LCD_Read (void) 
{
	uint16_t value;
	
	LPC_GPIO2->FIODIR &= ~(0xFF);              /* P2.0...P2.7 Input */
	LCD_DIR(0);		   				           				 /* Interface B->A */
	LCD_EN(0);	                               /* Enable 2B->2A */
	wait_delay(30);							   						 /* delay some times */
	value = LPC_GPIO2->FIOPIN0;                /* Read D8..D15 */
	LCD_EN(1);	                               /* Enable 1B->1A */
	wait_delay(30);							   						 /* delay some times */
	value = (value << 8) | LPC_GPIO2->FIOPIN0; /* Read D0..D7 */
	LCD_DIR(1);
	return  value;
}

/*******************************************************************************
* Function Name  : LCD_WriteIndex
* Description    : LCD写寄存器地址
* Input          : - index: 寄存器地址
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_WriteIndex(uint16_t index)
{
	LCD_CS(0);
	LCD_RS(0);
	LCD_RD(1);
	LCD_Send( index ); 
	wait_delay(22);	
	LCD_WR(0);  
	wait_delay(1);
	LCD_WR(1);
	LCD_CS(1);
}

/*******************************************************************************
* Function Name  : LCD_WriteData
* Description    : LCD写寄存器数据
* Input          : - index: 寄存器数据
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_WriteData(uint16_t data)
{				
	LCD_CS(0);
	LCD_RS(1);   
	LCD_Send( data );
	LCD_WR(0);     
	wait_delay(1);
	LCD_WR(1);
	LCD_CS(1);
}

/*******************************************************************************
* Function Name  : LCD_ReadData
* Description    : 读取控制器数据
* Input          : None
* Output         : None
* Return         : 返回读取到的数据
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) uint16_t LCD_ReadData(void)
{ 
	uint16_t value;
	
	LCD_CS(0);
	LCD_RS(1);
	LCD_WR(1);
	LCD_RD(0);
	value = LCD_Read();
	
	LCD_RD(1);
	LCD_CS(1);
	
	return value;
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
{ 
	/* Write 16-bit Index, then Write Reg */  
	LCD_WriteIndex(LCD_Reg);         
	/* Write 16-bit Reg */
	LCD_WriteData(LCD_RegValue);  
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Reads the selected LCD Register.
* Input          : None
* Output         : None
* Return         : LCD Register Value.
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
	uint16_t LCD_RAM;
	
	/* Write 16-bit Index (then Read Reg) */
	LCD_WriteIndex(LCD_Reg);
	/* Read 16-bit Reg */
	LCD_RAM = LCD_ReadData();      	
	return LCD_RAM;
}

/*******************************************************************************
* Function Name  : LCD_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position. 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_SetCursor(uint16_t Xpos,uint16_t Ypos)
{
    #if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )
	
 	uint16_t temp = Xpos;

			 Xpos = Ypos;
			 Ypos = ( MAX_X - 1 ) - temp;  

	#elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )
		
	#endif

  switch( LCD_Code )
  {
     default:		 /* 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x7783 0x4531 0x4535 */
          LCD_WriteReg(0x0020, Xpos );     
          LCD_WriteReg(0x0021, Ypos );     
	      break; 

     case SSD1298: 	 /* 0x8999 */
     case SSD1289:   /* 0x8989 */
	      LCD_WriteReg(0x004e, Xpos );      
          LCD_WriteReg(0x004f, Ypos );          
	      break;  

     case HX8346A: 	 /* 0x0046 */
     case HX8347A: 	 /* 0x0047 */
     case HX8347D: 	 /* 0x0047 */
	      LCD_WriteReg(0x02, Xpos>>8 );                                                  
	      LCD_WriteReg(0x03, Xpos );  

	      LCD_WriteReg(0x06, Ypos>>8 );                           
	      LCD_WriteReg(0x07, Ypos );    
	
	      break;     
     case SSD2119:	 /* 3.5 LCD 0x9919 */
	      break; 
  }
}

/*******************************************************************************
* Function Name  : LCD_Delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void delay_ms(uint16_t ms)    
{ 
	uint16_t i,j; 
	for( i = 0; i < ms; i++ )
	{ 
		for( j = 0; j < 1141; j++ );
	}
} 


/*******************************************************************************
* Function Name  : LCD_Initializtion
* Description    : Initialize TFT Controller.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Initialization(void)
{
	uint16_t DeviceCode;
	
	LCD_Configuration();
	delay_ms(100);
	DeviceCode = LCD_ReadReg(0x0000);		/* 读取屏ID	*/	
	
	if( DeviceCode == 0x9325 || DeviceCode == 0x9328 )	
	{
		LCD_Code = ILI9325;
		LCD_WriteReg(0x00e7,0x0010);      
		LCD_WriteReg(0x0000,0x0001);  	/* start internal osc */
		LCD_WriteReg(0x0001,0x0100);     
		LCD_WriteReg(0x0002,0x0700); 	/* power on sequence */
		LCD_WriteReg(0x0003,(1<<12)|(1<<5)|(1<<4)|(0<<3) ); 	/* importance */
		LCD_WriteReg(0x0004,0x0000);                                   
		LCD_WriteReg(0x0008,0x0207);	           
		LCD_WriteReg(0x0009,0x0000);         
		LCD_WriteReg(0x000a,0x0000); 	/* display setting */        
		LCD_WriteReg(0x000c,0x0001);	/* display setting */        
		LCD_WriteReg(0x000d,0x0000); 			        
		LCD_WriteReg(0x000f,0x0000);
		/* Power On sequence */
		LCD_WriteReg(0x0010,0x0000);   
		LCD_WriteReg(0x0011,0x0007);
		LCD_WriteReg(0x0012,0x0000);                                                                 
		LCD_WriteReg(0x0013,0x0000);                 
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0010,0x1590);   
		LCD_WriteReg(0x0011,0x0227);
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0012,0x009c);                  
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0013,0x1900);   
		LCD_WriteReg(0x0029,0x0023);
		LCD_WriteReg(0x002b,0x000e);
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0020,0x0000);                                                            
		LCD_WriteReg(0x0021,0x0000);           
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0030,0x0007); 
		LCD_WriteReg(0x0031,0x0707);   
		LCD_WriteReg(0x0032,0x0006);
		LCD_WriteReg(0x0035,0x0704);
		LCD_WriteReg(0x0036,0x1f04); 
		LCD_WriteReg(0x0037,0x0004);
		LCD_WriteReg(0x0038,0x0000);        
		LCD_WriteReg(0x0039,0x0706);     
		LCD_WriteReg(0x003c,0x0701);
		LCD_WriteReg(0x003d,0x000f);
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0050,0x0000);        
		LCD_WriteReg(0x0051,0x00ef);   
		LCD_WriteReg(0x0052,0x0000);     
		LCD_WriteReg(0x0053,0x013f);
		LCD_WriteReg(0x0060,0xa700);        
		LCD_WriteReg(0x0061,0x0001); 
		LCD_WriteReg(0x006a,0x0000);
		LCD_WriteReg(0x0080,0x0000);
		LCD_WriteReg(0x0081,0x0000);
		LCD_WriteReg(0x0082,0x0000);
		LCD_WriteReg(0x0083,0x0000);
		LCD_WriteReg(0x0084,0x0000);
		LCD_WriteReg(0x0085,0x0000);
		  
		LCD_WriteReg(0x0090,0x0010);     
		LCD_WriteReg(0x0092,0x0000);  
		LCD_WriteReg(0x0093,0x0003);
		LCD_WriteReg(0x0095,0x0110);
		LCD_WriteReg(0x0097,0x0000);        
		LCD_WriteReg(0x0098,0x0000);  
		/* display on sequence */    
		LCD_WriteReg(0x0007,0x0133);
		
		LCD_WriteReg(0x0020,0x0000);  /* 行首址0 */                                                          
		LCD_WriteReg(0x0021,0x0000);  /* 列首址0 */     
	}

    delay_ms(50);   /* delay 50 ms */	
}

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : 将屏幕填充成指定的颜色，如清屏，则填充 0xffff
* Input          : - Color: Screen Color
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Clear(uint16_t Color)
{
	uint32_t index;
	
	if( LCD_Code == HX8347D || LCD_Code == HX8347A )
	{
		LCD_WriteReg(0x02,0x00);                                                  
		LCD_WriteReg(0x03,0x00);  
		                
		LCD_WriteReg(0x04,0x00);                           
		LCD_WriteReg(0x05,0xEF);  
		                 
		LCD_WriteReg(0x06,0x00);                           
		LCD_WriteReg(0x07,0x00);    
		               
		LCD_WriteReg(0x08,0x01);                           
		LCD_WriteReg(0x09,0x3F);     
	}
	else
	{	
		LCD_SetCursor(0,0); 
	}	

	LCD_WriteIndex(0x0022);
	for( index = 0; index < MAX_X * MAX_Y; index++ )
	{
		LCD_WriteData(Color);
	}
}

/******************************************************************************
* Function Name  : LCD_BGR2RGB
* Description    : RRRRRGGGGGGBBBBB 改为 BBBBBGGGGGGRRRRR 格式
* Input          : - color: BRG 颜色值  
* Output         : None
* Return         : RGB 颜色值
* Attention		 : 内部函数调用
*******************************************************************************/
static uint16_t LCD_BGR2RGB(uint16_t color)
{
	uint16_t  r, g, b, rgb;
	
	b = ( color>>0 )  & 0x1f;
	g = ( color>>5 )  & 0x3f;
	r = ( color>>11 ) & 0x1f;
	
	rgb =  (b<<11) + (g<<5) + (r<<0);
	
	return( rgb );
}

/******************************************************************************
* Function Name  : LCD_GetPoint
* Description    : 获取指定座标的颜色值
* Input          : - Xpos: Row Coordinate
*                  - Xpos: Line Coordinate 
* Output         : None
* Return         : Screen Color
* Attention		 : None
*******************************************************************************/
uint16_t LCD_GetPoint(uint16_t Xpos,uint16_t Ypos)
{
	uint16_t dummy;
	
	LCD_SetCursor(Xpos,Ypos);
	LCD_WriteIndex(0x0022);  
	
	switch( LCD_Code )
	{
		case ST7781:
		case LGDP4531:
		case LGDP4535:
		case SSD1289:
		case SSD1298:
             dummy = LCD_ReadData();   /* Empty read */
             dummy = LCD_ReadData(); 	
 		     return  dummy;	      
	    case HX8347A:
	    case HX8347D:
             {
		        uint8_t red,green,blue;
				
				dummy = LCD_ReadData();   /* Empty read */

		        red = LCD_ReadData() >> 3; 
                green = LCD_ReadData() >> 2; 
                blue = LCD_ReadData() >> 3; 
                dummy = (uint16_t) ( ( red<<11 ) | ( green << 5 ) | blue ); 
		     }	
	         return  dummy;

        default:	/* 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x9919 */
             dummy = LCD_ReadData();   /* Empty read */
             dummy = LCD_ReadData(); 	
 		     return  LCD_BGR2RGB( dummy );
	}
}

/******************************************************************************
* Function Name  : LCD_SetPoint
* Description    : 在指定座标画点
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point)
{
	if( Xpos >= MAX_X || Ypos >= MAX_Y )
	{
		return;
	}
	LCD_SetCursor(Xpos,Ypos);
	LCD_WriteReg(0x0022,point);
}

/******************************************************************************
* Function Name  : LCD_DrawLine
* Description    : Bresenham's line algorithm
* Input          : - x1: A点行座标
*                  - y1: A点列座标 
*				   - x2: B点行座标
*				   - y2: B点列座标 
*				   - color: 线颜色
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/	 
void LCD_DrawLine( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1 , uint16_t color )
{
    short dx,dy;      /* 定义X Y轴上增加的变量值 */
    short temp;       /* 起点 终点大小比较 交换数据时的中间变量 */

    if( x0 > x1 )     /* X轴上起点大于终点 交换数据 */
    {
	    temp = x1;
		x1 = x0;
		x0 = temp;   
    }
    if( y0 > y1 )     /* Y轴上起点大于终点 交换数据 */
    {
		temp = y1;
		y1 = y0;
		y0 = temp;   
    }
  
	dx = x1-x0;       /* X轴方向上的增量 */
	dy = y1-y0;       /* Y轴方向上的增量 */

    if( dx == 0 )     /* X轴上没有增量 画垂直线 */ 
    {
        do
        { 
            LCD_SetPoint(x0, y0, color);   /* 逐点显示 描垂直线 */
            y0++;
        }
        while( y1 >= y0 ); 
		return; 
    }
    if( dy == 0 )     /* Y轴上没有增量 画水平直线 */ 
    {
        do
        {
            LCD_SetPoint(x0, y0, color);   /* 逐点显示 描水平线 */
            x0++;
        }
        while( x1 >= x0 ); 
		return;
    }
	/* 布兰森汉姆(Bresenham)算法画线 */
    if( dx > dy )                         /* 靠近X轴 */
    {
	    temp = 2 * dy - dx;               /* 计算下个点的位置 */         
        while( x0 != x1 )
        {
	        LCD_SetPoint(x0,y0,color);    /* 画起点 */ 
	        x0++;                         /* X轴上加1 */
	        if( temp > 0 )                /* 判断下下个点的位置 */
	        {
	            y0++;                     /* 为右上相邻点，即（x0+1,y0+1） */ 
	            temp += 2 * dy - 2 * dx; 
	 	    }
            else         
            {
			    temp += 2 * dy;           /* 判断下下个点的位置 */  
			}       
        }
        LCD_SetPoint(x0,y0,color);
    }  
    else
    {
	    temp = 2 * dx - dy;                      /* 靠近Y轴 */       
        while( y0 != y1 )
        {
	 	    LCD_SetPoint(x0,y0,color);     
            y0++;                 
            if( temp > 0 )           
            {
                x0++;               
                temp+=2*dy-2*dx; 
            }
            else
			{
                temp += 2 * dy;
			}
        } 
        LCD_SetPoint(x0,y0,color);
	}
} 

/******************************************************************************
* Function Name  : PutChar
* Description    : 将Lcd屏上任意位置显示一个字符
* Input          : - Xpos: 水平坐标 
*                  - Ypos: 垂直坐标  
*				   - ASCI: 显示的字符
*				   - charColor: 字符颜色   
*				   - bkColor: 背景颜色 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void PutChar( uint16_t Xpos, uint16_t Ypos, uint8_t ASCI, uint16_t charColor, uint16_t bkColor )
{
	uint16_t i, j;
    uint8_t buffer[16], tmp_char;
    GetASCIICode(buffer,ASCI);  /* 取字模数据 */
    for( i=0; i<16; i++ )
    {
        tmp_char = buffer[i];
        for( j=0; j<8; j++ )
        {
            if( ((tmp_char >> (7 - j)) & 0x01) == 0x01 )
            {
                LCD_SetPoint( Xpos + j, Ypos + i, charColor );  /* 字符颜色 */
            }
            else
            {
                LCD_SetPoint( Xpos + j, Ypos + i, bkColor );  /* 背景颜色 */
            }
        }
    }
}

/******************************************************************************
* Function Name  : GUI_Text
* Description    : 在指定座标显示字符串
* Input          : - Xpos: 行座标
*                  - Ypos: 列座标 
*				   - str: 字符串
*				   - charColor: 字符颜色   
*				   - bkColor: 背景颜色 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void GUI_Text(uint16_t Xpos, uint16_t Ypos, uint8_t *str,uint16_t Color, uint16_t bkColor)
{
    uint8_t TempChar;
    do
    {
        TempChar = *str++;  
        PutChar( Xpos, Ypos, TempChar, Color, bkColor );    
        if( Xpos < MAX_X - 8 )
        {
            Xpos += 8;
        } 
        else if ( Ypos < MAX_Y - 16 )
        {
            Xpos = 0;
            Ypos += 16;
        }   
        else
        {
            Xpos = 0;
            Ypos = 0;
        }    
    }
    while ( *str != 0 );
}

/*********************************************************************************************************
      My drawing functions
*********************************************************************************************************/

void LCD_DrawRectangle(uint16_t Xpos, uint16_t Ypos, uint16_t width, uint16_t height, uint16_t Color){

	uint16_t i,j;
	
	for(i=0; i<width; i++){
		for(j=0; j<height; j++){
			LCD_SetPoint(Xpos+i,Ypos+j,Color);
		}
	}
	
}

void Draw_Skull(uint16_t x, uint16_t y, uint16_t Color){
	
	//int y=130;
	//int x=113;

	LCD_DrawRectangle(x,y,14,2,Color);		//1

	LCD_DrawRectangle(x-4,y+2,4,2,Color);		//2
	LCD_DrawRectangle(x+14,y+2,4,2,Color);		//2

	LCD_DrawRectangle(x-6,y+4,2,2,Color);		//3
	LCD_DrawRectangle(x+18,y+4,2,2,Color);		//3

	LCD_DrawRectangle(x-8,y+6,2,2,Color);		//4
	LCD_DrawRectangle(x+20,y+6,2,2,Color);		//4

	LCD_DrawRectangle(x-10,y+8,2,2,Color);		//5
	LCD_DrawRectangle(x+22,y+8,2,2,Color);		//5

	LCD_DrawRectangle(x-12,y+10,2,2,Color);		//6
	LCD_DrawRectangle(x+24,y+10,2,2,Color);		//6

	LCD_DrawRectangle(x-14,y+12,2,16,Color);		//7
	LCD_DrawRectangle(x+26,y+12,2,16,Color);		//7

	LCD_DrawRectangle(x-12,y+28,2,2,Color);		//8
	LCD_DrawRectangle(x+24,y+28,2,2,Color);		//8

	LCD_DrawRectangle(x-10,y+30,2,4,Color);		//9
	LCD_DrawRectangle(x+22,y+30,2,4,Color);		//9

	LCD_DrawRectangle(x-12,y+34,2,4,Color);		//10
	LCD_DrawRectangle(x+24,y+34,2,4,Color);		//10

	LCD_DrawRectangle(x-10,y+38,4,2,Color);		//11
	LCD_DrawRectangle(x+20,y+38,4,2,Color);		//11

	LCD_DrawRectangle(x-6,y+36,6,2,Color);		//12
	LCD_DrawRectangle(x+14,y+36,6,2,Color);		//12

	LCD_DrawRectangle(x-2,y+34,2,18,Color);		//13
	LCD_DrawRectangle(x+14,y+34,2,18,Color);		//13

	LCD_DrawRectangle(x+2,y+38,2,16,Color);		//14
	LCD_DrawRectangle(x+10,y+38,2,16,Color);		//14

	LCD_DrawRectangle(x,y+52,2,2,Color);		//15
	LCD_DrawRectangle(x+12,y+52,2,2,Color);		//15

	//eyes

	LCD_DrawRectangle(x-10,y+20,6,6,Color);		//16
	LCD_DrawRectangle(x+18,y+20,6,6,Color);		//16

	LCD_DrawRectangle(x-8,y+22,6,6,Color);		//17
	LCD_DrawRectangle(x+16,y+22,6,6,Color);		//17

	LCD_DrawRectangle(x-6,y+24,8,6,Color);		//18
	LCD_DrawRectangle(x+12,y+24,8,6,Color);		//18

	LCD_DrawRectangle(x+2,y+26,2,4,Color);		//19
	LCD_DrawRectangle(x+10,y+26,2,4,Color);		//19

	LCD_DrawRectangle(x+4,y+28,2,2,Color);		//20
	LCD_DrawRectangle(x+8,y+28,2,2,Color);		//20

	LCD_DrawRectangle(x+6,y+30,2,2,Color);		//21

	LCD_DrawRectangle(x+4,y+32,2,4,Color);		//22
	LCD_DrawRectangle(x+8,y+32,2,4,Color);		//22

	LCD_DrawRectangle(x+6,y+38,2,16,Color);		//23

	LCD_DrawRectangle(x+4,y+54,2,2,Color);		//24
	LCD_DrawRectangle(x+8,y+54,2,2,Color);		//24

}

/*********************************************************************************************************
      My draw writing functions
*********************************************************************************************************/

void Draw_Meal(void){
	
	int x=30;
	int y=130;
	
	
	//TEST HAMBURGER
	
	LCD_DrawRectangle(x+4,y+30,14,2,Black);

	LCD_DrawRectangle(x+4,y+32,14,2,Orange);

	LCD_DrawRectangle(x+18,y+32,2,2,Black);

	LCD_DrawRectangle(x+2,y+32,2,2,Black);

	LCD_DrawRectangle(x+2,y+34,18,2,Orange);

	LCD_DrawRectangle(x+20,y+34,2,2,Black);

	LCD_DrawRectangle(x,y+34,2,2,Black);

	LCD_DrawRectangle(x,y+36,22,6,Orange);

	LCD_DrawRectangle(x+22,y+36,2,6,Black);	//astina alta

	LCD_DrawRectangle(x-2,y+36,2,6,Black);	//astina alta

	LCD_DrawRectangle(x-2,y+42,26,2,Black);

	LCD_DrawRectangle(x+24,y+44,2,4,Black);	//astina media

	LCD_DrawRectangle(x-4,y+44,2,4,Black);	//astina media

	LCD_DrawRectangle(x-2,y+44,26,4,Brown);

	LCD_DrawRectangle(x-2,y+48,2,6,Black);	//astina basso

	LCD_DrawRectangle(x+22,y+48,2,6,Black);	//astina basso

	LCD_DrawRectangle(x,y+48,22,2,Black);

	LCD_DrawRectangle(x,y+50,22,4,Orange);

	LCD_DrawRectangle(x,y+54,22,2,Black);
	
}

/*********************************************************************************************************
      My draw writing functions
*********************************************************************************************************/

void Draw_Snack(void){
	
	int x=180;
	int y=130;

	//TEST PATATINE
	
	LCD_DrawRectangle(x+18,y+24,2,16,Black);
	LCD_DrawRectangle(x+14,y+26,4,14,Yellow);
	LCD_DrawRectangle(x+12,y+24,2,16,Black);

	LCD_DrawRectangle(x+6,y+24,14,2,Black);
	LCD_DrawRectangle(x+6,y+24,2,16,Black);
	LCD_DrawRectangle(x+8,y+26,4,14,Yellow);


	LCD_DrawRectangle(x+24,y+26,2,8,Black);
	LCD_DrawRectangle(x+20,y+26,6,2,Black);
	LCD_DrawRectangle(x+20,y+28,4,6,Yellow);

	LCD_DrawRectangle(x,y+26,6,2,Black);
	LCD_DrawRectangle(x,y+26,2,8,Black);
	LCD_DrawRectangle(x+2,y+28,4,6,Yellow);

	LCD_DrawRectangle(x+26,y+34,2,12,Black);
	LCD_DrawRectangle(x+20,y+34,6,2,Black);

	LCD_DrawRectangle(x-2,y+34,8,2,Black);
	LCD_DrawRectangle(x-2,y+34,2,12,Black);

	LCD_DrawRectangle(x+20,y+36,6,4,Red);

	LCD_DrawRectangle(x,y+36,6,4,Red);

	LCD_DrawRectangle(x+18,y+40,8,2,Red);

	LCD_DrawRectangle(x+8,y+40,10,2,Black);

	LCD_DrawRectangle(x,y+40,8,2,Red);

	LCD_DrawRectangle(x,y+42,26,4,Red);

	LCD_DrawRectangle(x+2,y+46,22,8,Red);

	LCD_DrawRectangle(x,y+46,2,8,Black);	//astina bassa

	LCD_DrawRectangle(x+24,y+46,2,8,Black);	//astina bassa

	LCD_DrawRectangle(x,y+54,26,2,Black);
	
}

/*********************************************************************************************************
      My draw writing functions
*********************************************************************************************************/

void Cancel_MealSnack(uint16_t z){
	//0 for meal 1 for snack
	
	if(z){
		LCD_DrawRectangle(177,153,33,33,White);	
	}else{
		LCD_DrawRectangle(25,159,33,33,White);		
	}
}

/*********************************************************************************************************
      My draw writing functions
*********************************************************************************************************/

void Reset_Animation(void){
		
		LCD_DrawRectangle(90,110,60,90,White); //Cancello skull base 113,130
		
		Draw_Hearth();
	
		Draw_Skull(160,130,Black);//Disegno Skull intermediate
	
		Draw_Skull(160,130,White);//Cancello skull intermediate
	
		Draw_Skull(210,130,Black);//Disegno Skull lontano
	
		Draw_Skull(210,130,White);//Cancello Skull lontano
	
		LCD_DrawRectangle(0,271,240,49,White);
		LCD_DrawLine(0,270, 240, 270, Red);
	
		GUI_Text(90, 290, (uint8_t *) " Reset  ", Red, White);
		
}

/*********************************************************************************************************
      My draw writing functions
*********************************************************************************************************/

void Draw_Animation(uint16_t Color, uint16_t Color2){

	//Red eyes
	
	int y=130;

	LCD_DrawRectangle(103,y+20,6,6,Color);		//16
	LCD_DrawRectangle(131,y+20,6,6,Color);		//16

	LCD_DrawRectangle(105,y+22,6,6,Color);		//17
	LCD_DrawRectangle(129,y+22,6,6,Color);		//17

	LCD_DrawRectangle(107,y+24,8,6,Color);		//18
	LCD_DrawRectangle(125,y+24,8,6,Color);		//18

	LCD_DrawRectangle(115,y+26,2,4,Color);		//19
	LCD_DrawRectangle(123,y+26,2,4,Color);		//19

	LCD_DrawRectangle(117,y+28,2,2,Color);		//20
	LCD_DrawRectangle(121,y+28,2,2,Color);		//20

	//Cross

	LCD_DrawRectangle(119,y+6,2,12,Color2);		
	LCD_DrawRectangle(115,y+14,10,2,Color2);	
	
}

/*********************************************************************************************************
      My draw writing functions
*********************************************************************************************************/

void Draw_Hearth(void){

	int x=105;
	int y=150;
	
	LCD_DrawRectangle(x,y,8,2,Black);
	LCD_DrawRectangle(x+16,y,8,2,Black);	//dx

	LCD_DrawRectangle(x-2,y+2,2,2,Black);
	LCD_DrawRectangle(x+2,y+2,6,2,Red);
	LCD_DrawRectangle(x+8,y+2,2,2,Black);

	LCD_DrawRectangle(x+14,y+2,2,2,Black);	//dx
	LCD_DrawRectangle(x+16,y+2,8,2,Red);	//dx
	LCD_DrawRectangle(x+24,y+2,2,2,Black);	//dx

	LCD_DrawRectangle(x-4,y+4,2,6,Black);
	LCD_DrawRectangle(x,y+4,10,2,Red);
	LCD_DrawRectangle(x+10,y+4,2,2,Black);

	LCD_DrawRectangle(x+14,y+4,12,2,Red);	//dx
	LCD_DrawRectangle(x+26,y+4,2,6,Black);	//dx

	LCD_DrawRectangle(x-2,y+6,14,2,Red);
	LCD_DrawRectangle(x-2,y+8,12,2,Red);

	LCD_DrawRectangle(x+16,y+6,10,2,Red);	//dx
	LCD_DrawRectangle(x+14,y+8,12,2,Red);	//dx

	LCD_DrawRectangle(x-2,y+10,2,4,Black);
	LCD_DrawRectangle(x,y+10,8,2,Red);

	LCD_DrawRectangle(x+12,y+10,12,2,Red);	//dx
	LCD_DrawRectangle(x+24,y+10,2,4,Black);	//dx

	LCD_DrawRectangle(x,y+12,10,2,Red);

	LCD_DrawRectangle(x+14,y+12,10,2,Red);	//dx

	LCD_DrawRectangle(x,y+14,2,2,Black);
	LCD_DrawRectangle(x+2,y+14,6,2,Red);

	LCD_DrawRectangle(x+12,y+14,10,2,Red);	//dx
	LCD_DrawRectangle(x+22,y+14,2,2,Black);	//dx

	LCD_DrawRectangle(x+2,y+16,2,2,Black);
	LCD_DrawRectangle(x+4,y+16,6,2,Red);

	LCD_DrawRectangle(x+14,y+16,6,2,Red);	//dx
	LCD_DrawRectangle(x+20,y+16,2,2,Black);	//dx

	LCD_DrawRectangle(x+4,y+18,2,2,Black);
	LCD_DrawRectangle(x+6,y+18,6,2,Red);

	LCD_DrawRectangle(x+16,y+18,2,2,Red);	//dx
	LCD_DrawRectangle(x+18,y+18,2,2,Black);	//dx

	LCD_DrawRectangle(x+6,y+20,2,2,Black);
	LCD_DrawRectangle(x+8,y+20,6,2,Red);

	LCD_DrawRectangle(x+16,y+20,2,2,Black);	//dx

	LCD_DrawRectangle(x+8,y+22,2,2,Black);
	LCD_DrawRectangle(x+10,y+22,2,2,Red);
	LCD_DrawRectangle(x+12,y+22,2,2,Black);

	LCD_DrawRectangle(x+10,y+24,2,2,Black);

}

/*********************************************************************************************************
      My draw writing functions
*********************************************************************************************************/

void Draw_Writings(void){
	
	
	int x=5;
	int y=10;
	
	GUI_Text(60, 10, (uint8_t *) " Age:  ", Black, White);
	
	GUI_Text(5, 30, (uint8_t *) " Happiness  ", Black, White);
	
	GUI_Text(150, 30, (uint8_t *) " Satiety  ", Black, White);
	
	//Batteries
	
	//Sx
	LCD_DrawLine(30, 50, 64, 50, Black);
	LCD_DrawLine(30, 66, 64, 66, Black);
	LCD_DrawLine(30, 50, 30, 66, Black);
	LCD_DrawLine(64, 50, 64, 66, Black);
	
	LCD_DrawLine(64, 57, 66, 57, Black);
	LCD_DrawLine(66, 57, 66, 61, Black);
	LCD_DrawLine(64, 61, 66, 61, Black);
	
	//x0,y0,larghezza,altezza,colore
	
	LCD_DrawRectangle(32,52,6,12,Black);
	LCD_DrawRectangle(40,52,6,12,Black);
	LCD_DrawRectangle(48,52,6,12,Black);
	LCD_DrawRectangle(56,52,6,12,Black);
	
	
	//Dx
	
	LCD_DrawLine(170, 50, 204, 50, Black);
	LCD_DrawLine(170, 66, 204, 66, Black);
	LCD_DrawLine(170, 50, 170, 66, Black);
	LCD_DrawLine(204, 50, 204, 66, Black);
	
	LCD_DrawLine(204, 57, 206, 57, Black);
	LCD_DrawLine(206, 57, 206, 61, Black);
	LCD_DrawLine(204, 61, 206, 61, Black);
	
	//x0,y0,larghezza,altezza,colore
	
	LCD_DrawRectangle(172,52,6,12,Black);
	LCD_DrawRectangle(180,52,6,12,Black);
	LCD_DrawRectangle(188,52,6,12,Black);
	LCD_DrawRectangle(196,52,6,12,Black);
	
	//Down
	
	LCD_DrawLine(0,270, 240, 270, Black);
	LCD_DrawLine(120, 270, 120, 320, Black);
	
	GUI_Text(30, 290, (uint8_t *) " Meal  ", Black, White);
	
	GUI_Text(150, 290, (uint8_t *) " Snack  ", Black, White);
	
	
	LCD_DrawRectangle(x,y,6,8,Black);

	LCD_DrawRectangle(x+8,y-2,4,12,Black);

	LCD_DrawRectangle(x+12,y-4,4,16,Black);

	LCD_DrawRectangle(x+16,y-6,4,20,Black);

	LCD_DrawRectangle(x+20,y-8,2,24,Black);
	
}

void Draw_Cuddle(uint16_t x, uint16_t y){
	
	LCD_DrawRectangle(x,y,8,2,Black);
	LCD_DrawRectangle(x+14,y,8,2,Black);	//dx

	LCD_DrawRectangle(x-2,y+2,2,2,Black);
	LCD_DrawRectangle(x+2,y+2,6,2,Red);
	LCD_DrawRectangle(x+8,y+2,2,2,Black);

	LCD_DrawRectangle(x+12,y+2,2,2,Black);	//dx
	LCD_DrawRectangle(x+14,y+2,8,2,Red);	//dx
	LCD_DrawRectangle(x+22,y+2,2,2,Black);	//dx

	LCD_DrawRectangle(x-4,y+4,2,6,Black);
	LCD_DrawRectangle(x,y+4,10,2,Red);
	LCD_DrawRectangle(x+10,y+4,2,2,Black);

	LCD_DrawRectangle(x+12,y+4,12,2,Red);	//dx
	LCD_DrawRectangle(x+24,y+4,2,6,Black);	//dx

	LCD_DrawRectangle(x-2,y+6,26,4,Red);

	LCD_DrawRectangle(x-2,y+10,2,4,Black);
	LCD_DrawRectangle(x,y+10,22,4,Red);

	LCD_DrawRectangle(x+22,y+10,2,4,Black);	//dx

	//checkpoint doppio rettangolo ora iniziano i singoli

	LCD_DrawRectangle(x,y+14,2,2,Black);
	LCD_DrawRectangle(x+2,y+14,18,2,Red);

	LCD_DrawRectangle(x+20,y+14,2,2,Black);	//dx

	LCD_DrawRectangle(x+2,y+16,2,2,Black);
	LCD_DrawRectangle(x+4,y+16,14,2,Red);

	LCD_DrawRectangle(x+18,y+16,2,2,Black);	//dx

	LCD_DrawRectangle(x+4,y+18,2,2,Black);
	LCD_DrawRectangle(x+6,y+18,10,2,Red);

	LCD_DrawRectangle(x+16,y+18,2,2,Black);	//dx

	LCD_DrawRectangle(x+6,y+20,2,2,Black);
	LCD_DrawRectangle(x+8,y+20,6,2,Red);

	LCD_DrawRectangle(x+14,y+20,2,2,Black);	//dx

	LCD_DrawRectangle(x+8,y+22,2,2,Black);
	LCD_DrawRectangle(x+10,y+22,2,2,Red);
	LCD_DrawRectangle(x+12,y+22,2,2,Black);	//dx

	LCD_DrawRectangle(x+10,y+24,2,2,Black);
	
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
