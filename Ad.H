#ifndef __ad_H__
#define __ad_H__
//-------------------------------------------------------------------------------------------------
# include <msp430x16x.h>
# include "delay.h"
# define  REFVOL 1.5

#define SCL_Out P2DIR |=BIT0  //SCL设置为输出模式
#define SCL_H  P2OUT |= BIT0   //P2^3高电平
#define SCL_L  P2OUT &= ~BIT0  //P2^3低电平

#define SDA_H  P2OUT |= BIT1   //P2^2高电平
#define SDA_L  P2OUT &= ~BIT1  //P2^2低电平

#define SDA_In  P2DIR &=~BIT1 //SDA改成输入模式
#define SDA_Out P2DIR |=BIT1  //SDA变回输出模式
#define SDA_val P2IN&0x02
#define TRUE 1
#define FALSE 0

#define MIN_PP 0
#define MAX_PP 1

extern unsigned int Initdata[4];
extern unsigned int Max_AD;
extern unsigned int Min_AD;
extern unsigned int Current_AD;
extern float Time_MeaSample[10];
extern float Average_Time;
extern float Sample_Time;
typedef struct                     //---- 这个结构体有点太大了
{    
    unsigned int Temp_In[6];
    unsigned int Counter;
    unsigned int Temp_Back[6];
    unsigned int CurrentBack;
    unsigned int CurrentIn;
    unsigned int Temperature; 
    unsigned int CurrentDis;     //电流显示值, 等于实际输入值
    unsigned int FIFOI[9];
    unsigned int FIFOBack[9];
//    unsigned int K;
    unsigned int Pos[12];
    int Speed[12]; 
    int Acc[12]; 
    unsigned char firsttime;
    unsigned int CurrentBackRel; 
    int  maxSpeedUp;
    int  maxSpeedDown;     
    unsigned int  maxSpeedUpPos;    
    unsigned int  maxSpeedDownPos;
    int  maxDletaUp; 
    int  maxDletaDown;   
    unsigned char pretime;
    int g_vlow;
    int g_vlow2;  
}AD_REG;

extern void ADC12_Initial(void);
extern void ADC12_Convert( unsigned char mode );
extern void ack ();
extern void Calculate_ADpp(char pp);
extern char Affirm_ADpp(char pp);
extern float Calculate_AverageTime(void);

//-------------------------------------------------------------------------------------------------
#endif
