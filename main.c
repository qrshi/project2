#include <iostm8S207c8.h>
#include <string.h>
#include <stdlib.h>
#include "LCD12232.h"
#include "Beep.h"
#include "UART3.H"
#include "Timer4.H"
#include "Timer3.H"
#include "slrc632.h"
#include "XEROX7760API.h"
#include "XEROX286API.h"
#include "OKI3400API.h"
#include "CommandAPI.h"
#include "Uboot.h"
#include "OKI841or731API.h"

unsigned char Type = 1; //?????????,   0:XEROX286  1:XEROX7760 2:OKI 3400

#define VER_ADDR    0x17Fd0   //版本号存的地址  

void CLK_Init(void);
void DisplayUpDate(unsigned char flag,unsigned char Ty);
//*******************************************
//主函数
//******************************************
void main(void)
{
   unsigned char Error_Flag,i;
   unsigned char Ver[6];
   CLK_Init();
   CLK_CKDIVR = 0;
   
   LcdIni();
   
   DisplayArray(0,1,"Run Application",15); 
   for(i=0;i<6;i++)
   {
      Ver[i] = *(__far unsigned char*)(VER_ADDR+i);
   }
   DisplayArray(24,0,"VER:",4);//上电显示版本号
   DisplayArray(56,0,Ver+1,5);
   
   Beep_Init();
   TIM4_Init();
   Timer3_Init();
   UART3_Init(13.56,19200);
   Rc632Ready();
    
   
   PcdAntennaOff();//关闭天线   
   PcdConfigISOType('B'); //ISO14443B
   PcdAntennaOff();
   
   BeepOn(1);
   BeepOn(1);
   BeepOn(1);
   BeepOn(1);
   
   
   asm("rim");//开总中断

   while(1)
   {
      BootLoad();
      
      PcdAntennaOn();
      delay100us(300);
      
       switch(Type)
       {
           case 0: //XEROX 286
                 Error_Flag = XEROX286_TEST();
                 DisplayUpDate(Error_Flag,1);
                  break;

           case 1: //XEROX 7760
                  Error_Flag = XEROX7760_TEST();
                  DisplayUpDate(Error_Flag,2);
                  
                  break;

           case 2: //OKI 3400
                  Error_Flag = XEROX3400_TEST();
                  DisplayUpDate(Error_Flag,3);
                                                  
                  break;
                  
           case 3: //OKI C841  OKI731  
                 Error_Flag = XEROX841or731_TEST();
                 DisplayUpDate(Error_Flag,0);
                 break;
                 
           default:
                   DisplayUpDate(Error,0);
                    break;
       }
      
      delay100us(500);//50MS
      PcdAntennaOff();
      delay100us(1000);  
   }
}
//********************************************
//时钟设置
//********************************************
void CLK_Init(void)
{  
    CLK_ECKR=0x01;//外部时钟寄存器 外部时钟准备就绪，外部时钟开
    CLK_SWCR=0x02;//切换控制寄存器 使能自动切换机制
    CLK_SWR=0xB4;//主时钟切换寄存器 选择HSE为主时钟源
    while (!(CLK_SWCR & 0x08)); 
    CLK_CSSR=0x01;//时钟安全系统寄存器
}
//******************************
//????
//******************************
void DisplayUpDate(unsigned char flag,unsigned char Ty)
{
    unsigned int i;
	
    switch(flag)
    {
        case Right:
             BeepOn(1);
             break;
        
        case Error:
             Type = Ty;
             clrscr();
             DisplayStr(5,1,"PleasePutChip!"); 				  
             DisplayArray(8,0,"Search Chip...",14);
             for(i=0;i<8;i++) LastSerialNumber[i] = 0xff;				           
             break;
        
        case CONFIG_ERROR:
                      //   send_Array("?????!\r\n");
             clrscr();
             DisplayStr(8,1,"FAULT ERROR 01"); 	
             BeepOn(3);
            // delay100us(20000);//2S
             for(i=0;i<8;i++) LastSerialNumber[i] = 0xff;	
             break;
        
        case TEST_ERROR:
             clrscr();
             DisplayStr(8,1,"FAULT ERROR 02"); 	
             BeepOn(3);
            // delay100us(20000);//2S
             for(i=0;i<8;i++) LastSerialNumber[i] = 0xff;	
             break;
        
        default:break;
    }
}
