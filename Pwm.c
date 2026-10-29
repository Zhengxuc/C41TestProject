# include "Device.h"
PWM_REG PWM;

//---pwm输出端口初始化 ，定时器a的初始化
void Control_Initial(void)
{
  
    TACTL = TASSEL_2 + TACLR; 
    P1SEL |= BIT2 + BIT3 ; 
    P1DIR |= BIT2 + BIT3 ;
    //CCR0 = 5000/(0.136*PWM.FREQUENCY); //之前是0.012      
    CCR0 = 5000/(0.136*PWM.FREQUENCY); //之前是0.012     //0.136
    CCTL1 = OUTMOD_6;
    CCTL2 = OUTMOD_6;   
}
/*
void OutPut_Negetive(unsigned int m) 
{
      if(m>950){m=950;}
 //   if(VAL.V_p3_3_RCT ==1)
 //   {
       DAC12_1DAT = m*1.25;  
       DAC12_0DAT = 0; 
 //   }
//    else 
//    {
//       DAC12_0DAT = m*4;  
//       DAC12_1DAT = 0; 
//    }

}
//----------反向电压输出可控子程序-------
void OutPut_Positive( unsigned int n) 
{
      if(n>950){n=950;}
  //  if(VAL.V_p3_3_RCT ==1)
  //  {
       DAC12_0DAT = n*1.25;  
       DAC12_1DAT = 0; 
  //  }
  //  else
  //  {
  //     DAC12_1DAT = n*4;  
  //     DAC12_0DAT = 0; 
  //  }
    
}  
*/

void OutPut_Negetive(unsigned int m) 
{
      if(m>950){m=950;}
   
      if(0 == VAL.V_p8_2_POS)//故障安全
      {
      DAC12_0DAT= 1200 - m*1.2;
      DAC12_1DAT = 0; 
      }
      else if(1 == VAL.V_p8_2_POS) //故障闭锁
      {
        DAC12_1DAT = m*1.25;  
        DAC12_0DAT = 0;      
      }
}



//----------反向电压输出可控子程序-------
void OutPut_Positive( unsigned int n) //23   1047
{
      if(n>1047){n=1047;}//  充气限定0.75
      if(0 == VAL.V_p8_2_POS)//故障安全
      {  
       DAC12_0DAT =1200 + n*1.2;
       DAC12_1DAT = 0;
      }
      else if(1 == VAL.V_p8_2_POS) //故障闭锁
      {
       DAC12_0DAT = n*1.25;  
       DAC12_1DAT = 0; 
      }
}  



