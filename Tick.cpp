#include "Device.h"
//------------------------------------------------------------------------------
TICK_REG CLK;
//------------------------------------------------------------------------------
void Cpu_Tick_Initial(void)
{
    //-------------------------------------------------------------------
    // DCOCTL = 0X80;              // 1110 0000  80
    // BCSCTL1= 0X87;              // 1000 0110  87
    // BCSCTL2= 0X06;              // 0000 0000  //辅助时钟八分频
     DCOCTL = 0X80;              // 1110 0000  80
     BCSCTL1= 0X07;              // 1000 0110  87
     BCSCTL2= 0X06; // BCSCTL2= 0Xce;              // 0000 0000  //
  
     delay(50); 
  
    // delay(50);                  // 等待时钟稳定 
    //----------------------------------------   定时器B  预初始化 ---
     TBCTL = MC_1 + ID_3 + TBSSEL_2 + TBCLR; // 定时器A时钟源为SMCLK,并清TAR
     TBCCR0 = 625;                            // 10ms定时器  125k/625/2=100hz
     TBCCTL0 |= CCIE;                        // CCR0中断使能
     //-----------------------------------------------------
 //    TBCCR1 = 20;                           
 //    TBCCTL1 |= CCIE;                        // CCR1中断使能
     //-----------------------------------------------------
  //   TBCCR2 = 20;                           
  //   TB CCTL2 |= CCIE;                        // CCR2中断使能
    //--------------------------------------------------------------------
}

//------------------------------------------------------------------------------
// 定时器b中断        中断服务程序
//------------------------------------------------------------------------------ 
#pragma vector = TIMERB0_VECTOR 
__interrupt void TimerB0_Isr(void)         // ccr0中断服务            
{ 
  if(0 ==WDogflag)
  {
    WDTCTL= WDTPW + WDTCNTCL;              // 喂狗 
  }
    CLK.Ttem++;                            // 32最大
    if(MENU==mode1_menu)
    {
      if(Control_Flag.start && !Control_Flag.empty)   //开始
      {
            switch(Control_Flag.mode1_measure)
            { 
                case adc_measure:
                      if(CLK.ADmea_TimeFlag<3)
                      {
                            CLK.Times_cnt++;
                            if(CLK.Times_cnt>=3500)
                            {
                                CLK.Times_cnt=0;
                                CLK.ADmea_TimeFlag++;                     
                            }
                      } 
                break;
                case time_measure:
                      if(CLK.TImea_TimeFlag==1)
                      {
                          CLK.Times_cnt++;  
                      
                      }
                      else if(CLK.TImea_TimeFlag==2)
                      {
                            Time_MeaSample[Test_times]=CLK.Times_cnt/100.0f ; //单位s
                            Test_times++;
                            CLK.Times_cnt=0;
                            CLK.TImea_TimeFlag=0;
                      }
                break;            
                case auto_measure:
                      if(CLK.TImea_TimeFlag==1)
                      {
                          CLK.Times_cnt++;  
                      
                      }
                      else if(CLK.TImea_TimeFlag==2)
                      {
                            Sample_Time=CLK.Times_cnt/100.0f;  //单位s
                            if(Sample_Time<(0.8f*Average_Time))
                            {
                             
                               Control_Flag.mode1_measure=stop_measure;
                               Control_Flag.flash_refresh==1;
                            }
                            Test_times++;
                            CLK.Times_cnt=0;
                            CLK.TImea_TimeFlag=0;
                      }     
                 break;
                case stop_measure:
                     CLK.Times_cnt++;
                    if(CLK.Times_cnt>50)
                     {    
                          CLK.Times_cnt=0;
                          CLK.Alarm_Toggle=!CLK.Alarm_Toggle;
                      }
                              
                              
                default:break;
              
              }
      }
      else CLK.Times_cnt=0;
    
    } 
    else{         
      if(Control_Flag.start && !Control_Flag.empty)   //模式二  开始标志位和非空标志位
            {
              CLK.Times_cnt++;                         
              if(CLK.Times_cnt>=600)//6s
              {
                CLK.Out_Toggle=!CLK.Out_Toggle;
                CLK.Times_cnt=0;
                if(!CLK.Out_Toggle)//完整开关计数一次
                  Test_times+= 1;
                if(Test_times >= 500){   //大于一万次
                  Control_Flag.flash_refresh=1;
                }
              }
            }
            else 
            {
              CLK.Times_cnt=0;
              
            }
    }
    if(CLK.Ttem%CLK.Message_Load_Rate == 0)    {  Key_Deal();  }
    if(CLK.Ttem%CLK.Display_Load_Rate == 0)    { Display_Debug();/* Alarm_Deal(); */}
//    if(CLK.Ttem%2 == 0)                       
//    { 
//      //CLK.PIDCon_Flag =1;
//      if(CLK.OneKey_Flag==1) 
//      { 
//        CLK.Adj_Flag =1;  
//      }
//      else CLK.PIDCon_Flag =1;
//    }
//    if(CLK.Ttem == CLK.Second_modulate)    
//    {
//       CLK.Ttem=0;
//       SYS.bit.Dida=~SYS.bit.Dida;
//       if(CLK.SecRun) CLK.SecRun--; // 127
//       if(CLK.SecKB)  CLK.SecKB--;  // 16
//       if(CLK.SecAL)  CLK.SecAL--;  // 127
//       if(OP.Rdj_flg) OP.Rdj_cnt++; // 
//    }
    
//    if(CLK.Hart_Flag == 1)
//    {
//      CLK.SecHT++;
//      //if(CLK.SecHT>15)    //延时300ms
//      if((OCDIN&0x01) ==0)   //如果OCD被拉低,
//      {
//        HartMessage();
//        CLK.SecHT =0;
//      }
//    }
   if(0 ==WDogflag)
   {
    WDTCTL= WDTPW + WDTCNTCL; 
   }
   // TBCCR0 = CLK.DIS_SPEED*50;  //20201219 
}

/*
#pragma vector = TIMERB1_VECTOR 
__interrupt void TimerB1_Isr(void)         // ccr0中断服务            
{                     
   unsigned int TE = TBIV;


   if(TE==0x0002)        {       _BIC_SR_IRQ(LPM1_bits);   TBCCR1 = CLK.RUN_SPEED*10;        }
   else if (TE==0x0004)  
   {   
           if (CLK.Timer_Config!=50000)  { CLK.Timer_Config++;  }
           if(TBCCR2 < TBCCR0-50) { TBCCR2 = TBCCR2 + 50;}
           else                   { TBCCR2 = 50;         }
   }
} 
*/
