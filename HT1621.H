#ifndef __HT1621_H__
#define __HT1621_H__
//------------------------------------------------------------------------------
# include "delay.h"
//------------------------------------------------------------------------------
# define  COMMAND     0X8000       // 命令模式代码    100    0B 1000 0000
# define  WRITE       0XA0         // 写模式命令代码  101    0B 1010 0000

# define  WR          0x02         // 时钟线 P3.1 0X20 BIT1
# define  DATA        0X08         // 数据线 P3.3 BIT3
# define  CS1         0X04         // 片选线 P3.2  BIT2
# define  CS0         0X01         // 片选线 P3.0 BIT0
# define  LCD_PORT    P3OUT        // HT1621 引脚连接
//------------------------------------------------------------------------------                        
//------------------------------------------------------------------------------
typedef struct                     //---- 这个结构体有点太大了
{
    unsigned char ScreenB;
    unsigned char ScreenS;
    unsigned char Point;
    unsigned int SignL;
    unsigned int SignR;
    unsigned int DisTemp;
    unsigned char Digital_Dis[4];
    unsigned char Alarm_Dis[4];
    unsigned char Small_Dis[8];
    unsigned int CurrentBackDis;
    unsigned int CurrentBackDisL;
    unsigned int FIFORDIS[8];
    unsigned char Digital_Dis_W[4];
    unsigned char Dis_Time[8];
}DIS_REG;

//------------------------------------------------------------------------------
extern void HT1621_Initial(void);
extern void Display(void);
extern void Display_Debug(void);
extern void Dis_LOGO (void);
#endif
