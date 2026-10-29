#include "Device.h"
OPERATION_REG OP;

PIDServiceData g_PIDServiceData;
//PWM_Duty g_PWM_Duty;
DeviceFeature g_DeviceFeature;
//------------------------------------------------------------------------------
//--------------------------PID自整定添加---------------------------------------
//------------------------------------------------------------------------------

unsigned int semi_wavex1;
unsigned int semi_wavex2;  
unsigned int semi_wavey1;
unsigned int semi_wavey2;
unsigned int semi_wavex;
unsigned int semi_wavey;
 
unsigned char RelayCounter = 0;
 
//float Itest;
// 0: valve position value decrease
unsigned char PID_ZZDflag;       // 1: PID ZZD end

unsigned int Rdj_times;
unsigned int Times1 = 0;   //在整理高中低速时用
unsigned int Times2 = 0;    //在整定低速时防止不动  计数
unsigned int Times3 = 0;   //整定低速时使用
unsigned char Dir_Flag = 0;  //整定速度方向




unsigned int g_uspdtest = 30; //20201219

unsigned char g_uclitstate = 0;

unsigned int ucTickClock = 50; //20210111  50*20ms =1s
unsigned int g_usTimeAlarm = 10000;  //20210111 自检超时判断
float K_R = 1.0;

unsigned int g_usup_pwm_max = 950;
unsigned int g_usdown_pwm_max = 950;

unsigned int ZDmaxpwmCnt = 0;

unsigned int Testuptimes = 0;
unsigned int Testdowntimes = 0; 

unsigned char dwCnt = 0;
unsigned char  V_p1_3 = 2;
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void UpCalculate(void)
{ 
   g_dangel_up = OP.Up_Limited;
   g_dangel_up = (double)(g_dangel_up - 2048)/4096*3.141592653;         
  
  if(VAL.V_p1_1_RCR == V_LINE )
  {
    g_dsinAngel_up = sin(g_dangel_up);  
  }
  else 
  {
    g_dsinAngel_up = tan(g_dangel_up);  
  }   
}
void DownCalculate(void)
{
   g_dangel_down = OP.Down_Limited;   
   g_dangel_down = (double)(g_dangel_down - 2048)/4096*3.141592653; 
 
  if(VAL.V_p1_1_RCR == V_LINE )
  {
    g_dsinAngel_down = sin(g_dangel_down);  
  }
  else 
  {
    g_dsinAngel_down = tan(g_dangel_down);  
  }   
}

#include "Device.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void P1_2_Rdj_H(void)
{ 
 
  switch( SYS.bit.Step )
  {
  case  STP_0:{
    Rdj_times +=1;
    if((VAL.V_p8_1_TRG < 1200)&&(VAL.V_p8_1_TRG >= 10))
    {
      g_usTimeAlarm = VAL.V_p8_1_TRG*ucTickClock; //20210111
    }
    else
    {
      g_usTimeAlarm = 200*ucTickClock;
    }            
    if(Rdj_times >ucTickClock)
    {
      SYS.bit.Step  =STP_1;
      Rdj_times =0;
//      RelayReadyflag = 0; //20201128
//      RelayEndflag = 0;
      
      semi_wavex2 = 0;
      semi_wavey2 = 0;
      PID_ZZDflag = 0; 
      semi_wavex1 = 0;    //20201128
      
      AD.firsttime = 0;  //重新开始填充AD  20201230   
      AD.pretime =0; 
      AD.maxSpeedUp = 0;
      AD.maxSpeedDown = 0;     
      AD.maxSpeedUpPos = 0;    
      AD.maxSpeedDownPos = 0;               
      g_uclitstate = 0; 
      Times1 = 0;
      Times2 = 0; 
      Times3 = 0; 
      Dir_Flag = 1;
      
      
      VAL.V_p3_1_MIN = 200; //20210223 放开限位
      VAL.V_p3_2_MAX = 3890;
      
      Testuptimes = 0;
      Testdowntimes = 0;
    } 
  }
  case STP_1:{             
    Rdj_times +=1;
    OutPut_Negetive(1000);
    if(Rdj_times>800)
    {
      if(Rdj_times%300 ==0)
      {
        ADC12_Convert(1);
        if( OP.Difference <5 )   //保存排空位置
        {       
          dwCnt++;
          if(dwCnt >= 5)
          {
            dwCnt = 0;                     
            if(OP.Meterage_L <2048)  
            {
              if ( OP.Meterage_L <  VAL.V_p3_1_MIN  )  { OP.Down_Limited =  VAL.V_p3_1_MIN; }// 为了安全起见 保存机械最小值
              else    {  OP.Down_Limited = OP.Meterage_L;     } // 保存限位  保存更小的值
              VAL.V_p3_3_RCT = V_CTCL;  //定位器排气是轴逆时针转
            }
            else 
            {
              if( OP.Meterage_L >  VAL.V_p3_2_MAX )    { OP.Up_Limited =  VAL.V_p3_2_MAX;   }    //为了安全起见 保存机械最小值        
              else    {  OP.Up_Limited = OP.Meterage_L;       }    //保存限位    保存更小的值                                  
              VAL.V_p3_3_RCT = V_CLOK; //定位器排气时顺时针转
            }
            Rdj_times =0;   
            SYS.bit.Step  =STP_2;   //结束排空位置                  
          }
        }
        else
        {
          dwCnt = 0;
        }
      }
    }   
  }break;
  case STP_2:{
    Rdj_times +=1;
    OutPut_Positive( 1000);          
    if(Rdj_times>800)
    {
      if(Rdj_times%300 ==0)
      {
        ADC12_Convert(1);
        
        if( OP.Difference <5 )   //保存全压位置
        {
          dwCnt++;
          if(dwCnt >= 5)
          {
            dwCnt = 0;            
            if(OP.Meterage_L <2048)  
            {
              if(VAL.V_p3_3_RCT == V_CTCL)
              {
                ALA.bit.Calc_ERR = 1; return;
              }
              if ( OP.Meterage_L <  VAL.V_p3_1_MIN  )  { OP.Down_Limited =  VAL.V_p3_1_MIN; }// 为了安全起见 保存机械最小值
              else    {  OP.Down_Limited = OP.Meterage_L;     } // 保存限位  保存更小的值
              VAL.V_p3_3_RCT = V_CLOK; //定位器排气时顺时针转
              //VAL.V_p4_3_VRL = V_OPEN;   //气关阀
            }
            else
            {
              if(VAL.V_p3_3_RCT == V_CLOK)
              {
                ALA.bit.Calc_ERR = 1; return;
              }                       
              if ( OP.Meterage_L >  VAL.V_p3_2_MAX )   { OP.Up_Limited =  VAL.V_p3_2_MAX;   }    //为了安全起见 保存机械最小值        
              else    {  OP.Up_Limited = OP.Meterage_L;       }    //保存限位    保存更小的值                                  
              VAL.V_p3_3_RCT = V_CTCL;  //定位器排气是轴逆时针转
              //VAL.V_p4_3_VRL = V_OFF;   //气开阀  
            }
            Rdj_times =0;   
            OutPut_Positive(0);  //停止充气
            SYS.bit.Step  = STP_3;   //结束充气位置 
          }
        }
        else
        {
          dwCnt = 0;         
        }        
      }
    } 
  }break;
  
  case STP_3:{  //检查中点及行程范围
    Rdj_times +=1;
    OP.Range_M = OP.Up_Limited - OP.Down_Limited;
    OP.Middle_Valve = (OP.Up_Limited + OP.Down_Limited)/2;                     //计算中位点
        
    if((VAL.V_p1_1_RCR&0x01) == V_ROTR)  //整定角行程
    {
      OP.Up_Limited_Test = OP.Middle_Valve + (OP.Up_Limited - OP.Middle_Valve)*0.7;               //计算上边界  为双作用更改 
      OP.Down_Limited_Test = OP.Middle_Valve - (OP.Middle_Valve - OP.Down_Limited)*0.7;           //计算下边界  为双作用更改 
    }
    else
    {
      OP.Up_Limited_Test = OP.Middle_Valve + (OP.Up_Limited - OP.Middle_Valve)*0.1;               //计算上边界  在这里有边界限制所以没有问题
      OP.Down_Limited_Test = OP.Middle_Valve - (OP.Middle_Valve - OP.Down_Limited)*0.1;           //计算下边界  之前是 *0.2    
    }
    //--------------------------------------------------------------中位点检查--------------------第四步
    if( OP.Middle_Valve > 3019 || OP.Middle_Valve < 1434 ) { ALA.bit.Zero_ERR = 1;  return; } //机械0位置超差报警 原为2662  1434   //要求可偏差30度
    
    if(OP.Range_M <50)  {ALA.bit.Calc_ERR = 1; return; }  //排机械卡死  
    else if(OP.Range_M <580) {  ALA.bit.Rng__ERR = 1; return; ; } //报夹角过小 
    else 
    {  
      AD.firsttime = 0;  //重新开始填充AD  20201230               
      AD.pretime =0;  
      K_R = (float)(OP.Range_M/1000.0);  
      UpCalculate();
      DownCalculate();
      SAVE_FLH( P2_10F );
      SAVE_FLH( P3_4F ); 
      SAVE_FLH( P4_4F ); 
      SAVE_FLH( P1_6F );
      OP.Function = SAVEF;        
      SYS.bit.Step  = STP_4;   //参数检查结束
      Rdj_times =0;  //停止计时
    }
    
  }break;
  case STP_4:{   ////////////////////整定最大值
    Rdj_times +=1;   //计时  
    ZDmaxpwmCnt +=1;
    ADC12_Convert(1); 
    if(1 == g_uclitstate) //重新充气 以备下一步
    { 
      if(((VAL.V_p3_3_RCT == V_CTCL)&&( OP.Meterage_L > OP.Up_Limited-20))
         ||((VAL.V_p3_3_RCT == V_CLOK)&&( OP.Meterage_L < OP.Down_Limited+20)))
      {
        OutPut_Positive(0); 
        g_uclitstate = 0; 
        SYS.bit.Step  = STP_5;
        Rdj_times = 0;
      }
      else
      {
        OutPut_Positive( 1000);             
      }
    }
    else if(0 == g_uclitstate)
    {
//      if(VAL.V_p1_1_RCR == V_ROTR)  //整定角行程
      {        
        if(Dir_Flag ==1)//整定上行ROTR
        {
           
          if(VAL.V_p3_3_RCT == V_CTCL)
          {                         
            if( OP.Meterage_L > OP.Up_Limited_Test )
            {
              if((OP.Meterage_L > OP.Up_Limited_Test)
                 &&(OP.Meterage_L < OP.Up_Limited - OP.Range_M/10))//抛开两端
              {

                 Testuptimes = Rdj_times;
              }
              
              Times1 +=1;
              if(Times1>ucTickClock)
              {
                if(Testuptimes < VAL.V_p2_6_TUP*5)  //   3s   30*5 =150 = 3000/20
                {
                  OP.up_pwm -=30;  //时间过小需要重新整定
                }
                else { OP.di_up  =1;  }                 
                Dir_Flag =0;  //整定下行
                Times1=0;
                OutPut_Negetive(0);
                Rdj_times = 0;
              } 
            }
            else {                                 
              OutPut_Positive(OP.up_pwm);   
              if(( OP.Meterage_L < OP.Down_Limited + OP.Range_M/10)&&(OP.Meterage_L > OP.Down_Limited + 10)) //求平均速度
              {
                Rdj_times = 0;
              }
              else
              {;}            
            }
          }
          else //  OP.di_up = OP.di_down = 1;
          {
            if( OP.Meterage_L < OP.Down_Limited_Test )
            {
              if((OP.Meterage_L < OP.Down_Limited_Test)
                 &&(OP.Meterage_L > OP.Down_Limited +OP.Range_M/10))//抛开两端
              {
                 Testuptimes = Rdj_times;              
              }                

              Times1 +=1;
              if(Times1>ucTickClock)
              {
                if(Testuptimes < VAL.V_p2_6_TUP*5)  //   3s   30*5 =150 = 3000/20
                {
                  OP.up_pwm -=30;  //时间过小需要重新整定
                }
                else { OP.di_up  =1;  }                   
                Dir_Flag =0;  //整定下行
                Times1=0;
                OutPut_Negetive(0);
                Rdj_times = 0;
              } 
            }
            else {              
              OutPut_Positive(OP.up_pwm);  
               if(( OP.Meterage_L > OP.Up_Limited - OP.Range_M/10)&&(OP.Meterage_L < OP.Up_Limited - 10)) //求平均速度
              {
                Rdj_times = 0;
              }
              else
              {;}             
            }
          }
                   
        }
        else    //整定下行ROTR
        {
          if(VAL.V_p3_3_RCT == V_CTCL)
          {   
            if( OP.Meterage_L < OP.Down_Limited_Test )
            {
              //计算100%至0%的时间
              if((OP.Meterage_L < OP.Down_Limited_Test)
                 &&(OP.Meterage_L > OP.Down_Limited +OP.Range_M/10))//抛开两端
              {
                Testdowntimes = Rdj_times; 
                
              }
              Times1 +=1;
              if(Times1>ucTickClock)  //等待1s准备反向                
              {
                if(Testdowntimes < VAL.V_p2_7_TDN*5)  //   3s   30*5 =150 = 3000/20
                {
                  OP.down_pwm -=30;  //时间过小需要重新整定
                }
                else { OP.di_down  =1;  }                
                Dir_Flag =1;  //整定上行
                Times1=0;
                OutPut_Negetive(0);
                Rdj_times =0;  //停止计时               
              }                     
            }
            else {
              
              OutPut_Negetive(OP.down_pwm); 
              if(( OP.Meterage_L > OP.Up_Limited - OP.Range_M/10)
                 &&(OP.Meterage_L < OP.Up_Limited - 10)) //求平均速度
              {
                Rdj_times = 0;  //开始计时
              }
              else
              {;}           
            }
          }
          else
          {
            if( OP.Meterage_L > OP.Up_Limited_Test )
            {
              //计算100%至0%的时间
              if((OP.Meterage_L > OP.Up_Limited_Test)
                 &&(OP.Meterage_L < OP.Up_Limited - OP.Range_M/10))//抛开两端
              {
                Testdowntimes = Rdj_times; 
                
              }
              Times1 +=1;
              if(Times1>ucTickClock)  //等待1s准备反向                
              {
                if(Testdowntimes < VAL.V_p2_7_TDN*5)  //   3s   30*5 =150 = 3000/20
                {
                  OP.down_pwm -=30;  //时间过小需要重新整定
                }
                else { OP.di_down  =1;  }                
                Dir_Flag =1;  //整定上行
                Times1=0;
                OutPut_Negetive(0);
                Rdj_times =0;  //停止计时               
              }                      
            }
            else {
              OutPut_Negetive(OP.down_pwm);
              if(( OP.Meterage_L < OP.Down_Limited + OP.Range_M/10)
                 &&(OP.Meterage_L > OP.Down_Limited + 10)) //求平均速度
              {
                Rdj_times = 0;
              }
              else
              {;}               
            }
          }          
        }
      }

      
      if(OP.di_up && OP.di_down )  
      {
        g_usup_pwm_max = OP.up_pwm +20 ;
        g_usdown_pwm_max = OP.down_pwm + 20 ;
        VAL.V_p5_1__K1 = g_usup_pwm_max;
        VAL.V_p5_2__K2 = g_usdown_pwm_max;
        if(0 == g_uclitstate)
        {
          g_uclitstate = 1; //需要重新回到满气  只用1次  
        }
        else
        {;}
        OP.di_up =0;
        OP.di_down =0;
        //设定小值整定范围￡中值不整定
        OP.Up_Limited_Test = OP.Middle_Valve + (OP.Up_Limited - OP.Middle_Valve)*0.04;               //计算上边界  在这里有边界限制所以没有问题
        OP.Down_Limited_Test = OP.Middle_Valve - (OP.Middle_Valve - OP.Down_Limited)*0.04;  
        Times1 =0;             
        Times2 =0;
        if(OP.up_pwm>800)  OP.up_pwm=800;
        if(OP.down_pwm>800)  OP.down_pwm =800;
        ZDmaxpwmCnt = 0;
      } //参数检查结束
      if(ZDmaxpwmCnt > 120*ucTickClock)
      {
        g_usup_pwm_max = 650;
        g_usdown_pwm_max = 650;
        if((VAL.V_p1_1_RCR&0x01) == V_ROTR)  //整定角行程
        {
          g_usup_pwm_max = 450;
          g_usdown_pwm_max = 450;
        }           
          if(OP.di_up  ==1)
          {
            g_usup_pwm_max = OP.up_pwm;
          }
          if(OP.di_down  ==1)
          {
            g_usdown_pwm_max = OP.down_pwm;
          }     

        VAL.V_p5_1__K1 = g_usup_pwm_max;
        VAL.V_p5_2__K2 = g_usdown_pwm_max;             
        Rdj_times = 0;
        if(0 == g_uclitstate)
        {
          g_uclitstate = 1; //需要重新回到满气  只用1次  
        }             
        else
        {;}        
        OP.di_up =0;
        OP.di_down =0;
        //设定小值整定范围￡中值不整定
        OP.Up_Limited_Test = OP.Middle_Valve + (OP.Up_Limited - OP.Middle_Valve)*0.04;               //计算上边界  在这里有边界限制所以没有问题
        OP.Down_Limited_Test = OP.Middle_Valve - (OP.Middle_Valve - OP.Down_Limited)*0.04;  
        Times1 =0;             
        Times2 =0;
        if(OP.up_pwm>800)  OP.up_pwm=800;
        if(OP.down_pwm>800)  OP.down_pwm =800;  
        ZDmaxpwmCnt = 0;
      }
    }
    
    
  }break;             
  case STP_5:{  //计算100%至0%的时间
    Rdj_times +=1;      
    ADC12_Convert(1);
    
    OutPut_Negetive(g_usdown_pwm_max);
    if(VAL.V_p3_3_RCT == V_CTCL)
    {
      ValveRodSpeedNEW(1);   //20201230 求速度             
      if(( OP.Meterage_L > OP.Up_Limited - OP.Range_M/10)
         &&(OP.Meterage_L < OP.Up_Limited - 10)) //求平均速度
      {
        Rdj_times = 0;
      }
      else
      {;}
      if((OP.Meterage_L < OP.Down_Limited +OP.Range_M/8)
         &&(OP.Meterage_L > OP.Down_Limited +OP.Range_M/10))//抛开两端
      {
        VAL.V_p2_7_TDN = Rdj_times/5; 
      }             
      if( OP.Meterage_L < OP.Down_Limited+20 )
      {               
        //VAL.V_p2_7_TDN = Rdj_times;
        OutPut_Positive(0);  
        SYS.bit.Step  = STP_6;   //参数检查结束
        Rdj_times =0;  //停止计时
        AD.firsttime = 0;  //重新开始填充AD  20201230
        AD.pretime =0;                
      }
    }
    else
    {
      ValveRodSpeedNEW(0);   //20201230 求速度              
      if(( OP.Meterage_L < OP.Down_Limited + OP.Range_M/10)
         &&(OP.Meterage_L > OP.Down_Limited + 10)) //求平均速度
      {
        Rdj_times = 0;
      }
      else
      {;} 
      if((OP.Meterage_L > OP.Up_Limited - OP.Range_M/8)
         &&(OP.Meterage_L < OP.Up_Limited - OP.Range_M/10))//抛开两端
      {
        VAL.V_p2_7_TDN = Rdj_times/5;
      }
      if( OP.Meterage_L > OP.Up_Limited -20 )
      {
        
        //VAL.V_p2_7_TDN = Rdj_times;
        OutPut_Positive(0);
        SYS.bit.Step  = STP_6;   //参数检查结束
        Rdj_times =0;  //停止计时 
        AD.firsttime = 0;  //重新开始填充AD  20201230                
        AD.pretime =0;  
      } 
    } 
    if(Rdj_times>g_usTimeAlarm)
    {//超时报警
      ALA.bit.No_SCALE =1; return;   //极限位置无法确定
    }
  }break;
  case STP_6:{
    Rdj_times +=1;
    OutPut_Positive( g_usup_pwm_max);
    ADC12_Convert(1);             
    
    if(VAL.V_p3_3_RCT == V_CTCL)
    {
      ValveRodSpeedNEW(0);   //20201230 求速度             
      if(( OP.Meterage_L < OP.Down_Limited + OP.Range_M/10)&&(OP.Meterage_L > OP.Down_Limited + 10)) //求平均速度
      {
        Rdj_times = 0;
      }
      else
      {;}
      
      if((OP.Meterage_L > OP.Up_Limited - OP.Range_M/8)
         &&(OP.Meterage_L < OP.Up_Limited - OP.Range_M/10))//抛开两端
      {
        VAL.V_p2_6_TUP = Rdj_times/5;
      }             
      if( OP.Meterage_L > OP.Up_Limited -20)
      {                 
        //VAL.V_p2_6_TUP = Rdj_times;
        OutPut_Positive(0);  
        SYS.bit.Step  = STP_7;   //参数检查结束
        Rdj_times =0;  //停止计时
        AD.firsttime = 0;  //重新开始填充AD  20201230
        AD.pretime =0; 
        
      }
    }
    else
    {
      ValveRodSpeedNEW(1);   //20201230 求速度             
      if(( OP.Meterage_L > OP.Up_Limited - OP.Range_M/10)&&(OP.Meterage_L < OP.Up_Limited - 10)) //求平均速度
      {
        Rdj_times = 0;
      }
      else
      {;} 
      if((OP.Meterage_L < OP.Down_Limited +OP.Range_M/8)
         &&(OP.Meterage_L > OP.Down_Limited +OP.Range_M/10))//抛开两端
      {
        VAL.V_p2_6_TUP = Rdj_times/5;               
      }            
      if( OP.Meterage_L < OP.Down_Limited+20  )
      {                  
        //VAL.V_p2_6_TUP = Rdj_times;
        OutPut_Positive(0);
        SYS.bit.Step  = STP_7;   //参数检查结束
        Rdj_times =0;  //停止计时 
        AD.firsttime = 0;  //重新开始填充AD  20201230                
        AD.pretime =0;             
      } 
    } 
    if(Rdj_times>g_usTimeAlarm)
    {//超时报警
      ALA.bit.No_SCALE =1; return;   //极限位置无法确定
    }
    
  }break;           
  
  case STP_7:{   ////////////////////整定位移最小值
    Rdj_times +=1;   //计时   
    ADC12_Convert(1);
    ValveRodSpeedNEW(5);   //20210117 求位移 曲线           
    if(0 == g_uclitstate)
    {
      OutPut_Negetive(g_usdown_pwm_max); 
      
      if(((OP.Meterage_L < AD.maxSpeedDownPos)&&(VAL.V_p3_3_RCT == V_CTCL))
         ||((OP.Meterage_L > AD.maxSpeedUpPos)&&(VAL.V_p3_3_RCT == V_CLOK)))
      {  
        OutPut_Negetive(0);
        Rdj_times = 0;
        g_uclitstate  =1;
      }
      
    }
    else if(1 == g_uclitstate)
    {   
      if(Rdj_times>200)
      {
        if(VAL.V_p3_3_RCT == V_CTCL)             
        {               
          ValveRodSpeedNEW(3);   //20201230 求位移 
          VAL.V_p5_10_K0 = abs(AD.maxDletaDown)/K_R;               
        }
        else
        {
          ValveRodSpeedNEW(2);   //20201230 求位移 
          VAL.V_p5_10_K0 = abs(AD.maxDletaUp)/K_R;
        }             
        Rdj_times = 0;
        OP.di_up =0;
        OP.di_down =0; 
        Times2 =0;
        if(OP.up_pwm>200)  OP.up_pwm=200;
        if(OP.down_pwm>200)  OP.down_pwm =200;  
        if(VAL.V_p5_10_K0 > 250 ) //超过10%
        {
          //VAL.V_p5_10_K0 = 100;
          VAL.V_p5_10_K0 = 250;
        } 	
        if(VAL.V_p5_10_K0 < 50)		
        {
          VAL.V_p5_10_K0 = 50;
        }								
        g_uclitstate = 2;  //进入最小输出气量整定
      }
    }
    else if(2 == g_uclitstate)
    {
      if(AD.g_vlow >= -1) //等待200ms时间 没有动
      {
        if(1 == Times3) //新循环标志位
        {
          Times3 = 0;
        }
        Times1 +=1; 
        if(Times1 >= 5*50)  ////等待5s时间 没有动 则增大输出
        {
          Times1 = 0;
          if(OP.down_pwm < 600)
          {
            OP.down_pwm  = OP.down_pwm +20; 
            OutPut_Negetive(OP.down_pwm);                    
          }              
        }
      }
      else if(AD.g_vlow <= -4)  //确认运动了  
      {
        if(0 == Times3)  //施密特切换
        {
          Times1 = 0;                   
          Times2 ++; //等待停下来  每次加1  测试3次
          if(Times2 >= 3)
          {
             OutPut_Negetive(0);              
             g_uclitstate = 4;  //小量整定完毕 开始整定快速启动量 
             Times2 = 0;
             VAL.V_p6_7_MAX = 2;//快速启动补气时间
          }
          else
          {
            if(OP.down_pwm > 200)
            {                 
              OP.down_pwm  = OP.down_pwm -30;
            }
            OutPut_Negetive(0);                 
          }             
          Times3 = 1; //等待下一次静止
        }
        
        
      }
      else
      {
        ;
      }
      
    }
    else if(4 == g_uclitstate)  //小量整定完毕 开始整定快速启动量   
    {
      if(g_vl >= -2)
      {
        if(AD.g_vlow2 >= -1) //等待200ms时间 没有动
        {
          if(1 == Times3) //新循环标志位
          {
            Times3 = 0;
            OutPut_Negetive(950);           
          } 
          Times1 +=1;        
        }        
      }
      else if(g_vl <= -4)
      {
        VAL.V_p6_7_MAX = Times1;
        if(Times1 > 25)
        {
          VAL.V_p6_7_MAX = 25;   //不超过0.5s       
        }
        if(Times1 < 2)
        {
          VAL.V_p6_7_MAX = 2;        
        }        
          if(0 == Times3)  //施密特切换
          {
            Times1 = 0;                   
            Times2 ++; //等待停下来  每次加1  测试2次
            if(Times2 >= 2)
            {
              OutPut_Negetive(800);              
              g_uclitstate = 3;  //快速启动量整定完毕               
            }
            else
            {
               OutPut_Negetive(0); //已动关停                
            }             
            Times3 = 1; //等待下一次静止
          }        
 
      }
      else
      {
        ;
      }
    }
    else
    {
      ;
    }
    
    if((( OP.Meterage_L < OP.Down_Limited + 20)&&(VAL.V_p3_3_RCT == V_CTCL))
       ||((OP.Meterage_L > OP.Up_Limited -20)&&(VAL.V_p3_3_RCT == V_CLOK)))
    {
      OutPut_Negetive(0);
      Rdj_times =0;  //停止计时 
      AD.firsttime = 0;  //重新开始填充AD  20201230 
      AD.pretime =0;              
      g_uclitstate = 0; 
      Times1 = 0;
      Times2 = 0; 
      Times3 = 0;
      VAL.V_p5_4__K4 = OP.down_pwm; 
      SYS.bit.Step  = STP_8;
    }
    if(Rdj_times > g_usTimeAlarm)
    {//超时报警
      ALA.bit.No_SCALE =1; return;   //极限位置无法确定
    }                        
    
  }break; 
  
  case STP_8:{//
    
    Rdj_times +=1;   //计时   
    ADC12_Convert(1);
    ValveRodSpeedNEW(5);   //20210117 求位移 曲线 
    if(0 == g_uclitstate)
    { 
      OutPut_Positive( g_usup_pwm_max); 
      
      if((( OP.Meterage_L > AD.maxSpeedUpPos)&&(VAL.V_p3_3_RCT == V_CTCL))
         ||((OP.Meterage_L < AD.maxSpeedDownPos)&&(VAL.V_p3_3_RCT == V_CLOK)))              
      { 
        OutPut_Positive(0);
        Rdj_times = 0;
        g_uclitstate  =1;            
      }
    }
    else if(1 == g_uclitstate)
    {           
      if(Rdj_times>200)
      {
        if(VAL.V_p3_3_RCT == V_CTCL)             
        {               
          ValveRodSpeedNEW(2);   //20201230 求位移 
          VAL.V_p5_9__K9 = abs(AD.maxDletaUp)/K_R; 
        }
        else
        {
          ValveRodSpeedNEW(3);   //20201230 求位移 
          VAL.V_p5_9__K9 = abs(AD.maxDletaDown)/K_R;               
        }
        
        OutPut_Positive(0);
        Rdj_times =0;  //停止计时  
        if(VAL.V_p5_9__K9 > 250 ) //超过10%
        {
          //VAL.V_p5_9__K9 = 100;
          VAL.V_p5_9__K9 = 250;
        }
 	
        if(VAL.V_p5_9__K9 < 50)		
        {
          VAL.V_p5_9__K9 = 50;
        }								
        g_uclitstate = 2;  //进入最小输出气量整定
        PID_ZZDflag = 1;    //保存数据标志位                         
      }                  
    } 
    else if(2 == g_uclitstate)
    {
      if(AD.g_vlow <= 1)  
      {
        if(1 == Times3) //新循环标志位
        {
          Times3 = 0;
        }              
        Times1 +=1; 
        if(Times1 >= 5*50)  ////等待5s时间 没有动 则增大输出
        {
          Times1 = 0;
          if(OP.up_pwm < 600)
          {
            OP.up_pwm  = OP.up_pwm +20; 
            OutPut_Positive(OP.up_pwm);                    
          }              
        }
      }
      else if(AD.g_vlow >= 4)  //确认运动了  
      { 
        if(0 == Times3)  //施密特切换
        {
          Times1 = 0;
          Times2 ++; //等待停下来  每次加1  测试3次 
          if(Times2 >= 3)
          {                  
            g_uclitstate = 4;   //小量整定完毕 开始整定快速启动量  
            VAL.V_p5_3__K3 = OP.up_pwm;  
            VAL.V_p6_6_MIN = 2;
          }
          else
          {
            
            if(OP.up_pwm > 100)
            {                 
              OP.up_pwm  = OP.up_pwm -30;
            }
            OutPut_Positive(0);  
            
          }
          Times3 = 1; //等待下一次静止
        }             
      }
      else
      {
        ;
      }          
    }
    else if(4 == g_uclitstate)  //小量整定完毕 开始整定快速启动量   
    {
      if(g_vl <= 2)
      {
        if(AD.g_vlow2 <= 1) //等待200ms时间 没有动
        {
          if(1 == Times3) //新循环标志位
          {
            Times3 = 0;
            OutPut_Positive(950);           
          } 
          Times1 +=1;        
        }        
      }
      else if(g_vl >= 4)
      {
        VAL.V_p6_6_MIN = Times1;
        if(Times1 > 25)
        {
          VAL.V_p6_6_MIN = 25;   //不超过0.5s       
        }
        if(Times1 < 2)
        {
          VAL.V_p6_6_MIN = 2;        
        }        
          if(0 == Times3)  //施密特切换
          {
            Times1 = 0;                   
            Times2 ++; //等待停下来  每次加1  测试2次
            if(Times2 >= 2)
            {
             
              g_uclitstate = 3;  //快速启动量整定完毕               
            }
            else
            {
               OutPut_Positive(0); //已动关停                
            }             
            Times3 = 1; //等待下一次静止
          }        
 
      }
      else
      {
        ;
      }
    }    
    else
    {
      ;
    }
    
    if((( OP.Meterage_L > OP.Up_Limited -20)&&(VAL.V_p3_3_RCT == V_CTCL))
       ||((OP.Meterage_L < OP.Down_Limited +20)&&(VAL.V_p3_3_RCT == V_CLOK))
         ||(g_uclitstate == 3))
    {               
      OutPut_Positive(0);                
      AD.firsttime = 0;  //重新开始填充AD  20201230                
      AD.pretime =0; 
      Times1 = 0;
      Times2 = 0; 
      Times3 = 0; 
      g_uclitstate = 0;
      VAL.V_p5_3__K3 = OP.up_pwm;              
      
      VAL.V_p5_5__K5 = 5;
      VAL.V_p5_6__K6 = 5;
      VAL.V_p5_7__K7 = 5;
      VAL.V_p5_8__K8 = 5;                
      
      if(AD.CurrentBack >OP.Down_Limited)
      {
        g_PIDServiceData.Cur_ValvePoint = (AD.CurrentBack -OP.Down_Limited)*1000.0/OP.Range_M;     //当前位置采样 采样值按0-1000来处理
      }
      else if(AD.CurrentBack > OP.Up_Limited ){ g_PIDServiceData.Cur_ValvePoint = 1000; }
      else 
      {
        g_PIDServiceData.Cur_ValvePoint =0;
      }
      
      if( VAL.V_p3_3_RCT == V_CLOK) 
      {
        g_PIDServiceData.Cur_ValvePoint = 1000-g_PIDServiceData.Cur_ValvePoint; 
        Dir_Flag = 0;
      }
      else
      {
        Dir_Flag = 1;    
      }
      g_PIDServiceData.SetPoint = g_PIDServiceData.Cur_ValvePoint; //给定初值
      if(g_PIDServiceData.Cur_ValvePoint <500)
      {
        g_PIDServiceData.Kp =VAL.V_p5_5__K5; 
        g_PIDServiceData.Ki =VAL.V_p5_7__K7;      
      }
      else
      {
        g_PIDServiceData.Kp =VAL.V_p5_6__K6; 
        g_PIDServiceData.Ki =VAL.V_p5_8__K8;  
      } 
      if(VAL.V_p5_9__K9 < 50)		
      {
        VAL.V_p5_9__K9 = 50;
      }	
      if(VAL.V_p5_10_K0 < 50)		
      {
        VAL.V_p5_10_K0 = 50;
      }	
      if(VAL.V_p5_9__K9 > 250)		
      {
        VAL.V_p5_9__K9 = 250;
      }	
      if(VAL.V_p5_10_K0 > 250)		
      {
        VAL.V_p5_10_K0 = 250;
      }	

        
      SAVE_FLH( P2_10F );
      SAVE_FLH( P3_4F ); 
      SAVE_FLH( P4_4F ); 
      SAVE_FLH( P1_6F );
      OP.Function = SAVEF;                 
      Rdj_times =0; 
      V_p1_3 = VAL.V_p1_3BRND; //临时保存1.3  20220515
   //   VAL.V_p1_3BRND = 2 ;1.3改为0.2% 整定时   20220515     
      SYS.bit.Step  = STP_9;   //参数检查结束               
      
    }
    
    if(Rdj_times>g_usTimeAlarm)
    {//超时报警
      OutPut_Positive(0);             
      return;   //极限位置无法确定
    }
    
  }break;
  
  case STP_9:{   //整定输出K值
    Rdj_times +=1;   //计时  
    if(RelayCounter < 3)
    {
      if((Dir_Flag ==1)&&(0 == OP.di_up))//整定上行
      {
        g_PIDServiceData.SetPoint = g_PIDServiceData.Cur_ValvePoint + 60;
        OP.di_up =1;
        OP.di_down = 0;
        if((g_PIDServiceData.SetPoint > 850) ) 
        {g_PIDServiceData.SetPoint =850;Dir_Flag =0;OP.di_up =0;RelayCounter++;}        
        
      }
      else if((Dir_Flag ==0)&&(0 == OP.di_down))
      {
        g_PIDServiceData.SetPoint = g_PIDServiceData.Cur_ValvePoint - 60;
        OP.di_down = 1;
        OP.di_up =0;
        
        if((g_PIDServiceData.SetPoint < 150))
        {g_PIDServiceData.SetPoint =150;Dir_Flag =1;OP.di_down = 0;RelayCounter++;}       
      }
      else
      {
        ;
      } 
    }
    else if(RelayCounter < 5) //x行程整定 20220604
    {
      if(3 == RelayCounter)
      {
         VAL.V_p1_3BRND = V_p1_3 ;//恢复1.3   20220604 
           RelayCounter = 0;             
           SYS.bit.Step = SP_OK;           

      }
      else // RelayCounter == 4;进入大摩擦力整定
      {

      }      
       
    }
    else
    {
      RelayCounter = 0;
      VAL.V_p1_3BRND = V_p1_3 ;//恢复1.3   20220604
      SYS.bit.Step = SP_OK;   
    }
    
    PIDcontrol_H(); //对应大摩擦力整定
    //求超调量  上行
    if(OP.di_up >=1)
    {            
      if((g_PIDServiceData.Cur_Error >= -2)&&(g_PIDServiceData.Cur_Error <= 2)&&(1 == OP.di_up)) //超调
      {
        semi_wavex1 = Rdj_times;
        OP.di_up = 2;
        Rdj_times = 0;
      }
      if(2 == OP.di_up)
      {
        if(g_PIDServiceData.Cur_Error < -3)
        {              
          if (semi_wavey1>g_PIDServiceData.Cur_ValvePoint)//bug
          {
            if((AD.g_vlow <2)&&(AD.g_vlow >-2))
            {
              OP.di_up = 0;
              semi_wavex = Rdj_times;
              Rdj_times = 0;
              g_PIDServiceData.SetPoint = g_PIDServiceData.Cur_ValvePoint;
              OutPut_Positive(0);
              if(semi_wavey1 > (g_PIDServiceData.SetPoint + 3))
              {
                if(RelayCounter < 3)
                {
                  if(g_PIDServiceData.Cur_ValvePoint < 500)
                  {
                    if(VAL.V_p5_5__K5 > 2)
                    {
                      VAL.V_p5_5__K5 = VAL.V_p5_5__K5 - 1;
                    }
                    g_PIDServiceData.Kp = VAL.V_p5_5__K5; 
                  }
                  else
                  {
                    if(VAL.V_p5_6__K6 > 2)
                    {
                      VAL.V_p5_6__K6 = VAL.V_p5_6__K6 - 1;
                    }
                    g_PIDServiceData.Kp = VAL.V_p5_6__K6;                
                  }
                }

              }               
              semi_wavey1 = 0; 
            }
          }
          else {semi_wavey1=g_PIDServiceData.Cur_ValvePoint;} 
        }
        else if((g_PIDServiceData.Cur_Error >= -3)&&(g_PIDServiceData.Cur_Error <= 3))
        {
          if(Rdj_times > 50)
          {
            Rdj_times = 0;
            OP.di_up =0;
          }
        }
        
        if(Rdj_times > 8*50)
        {
          OP.di_up = 0;
          Rdj_times = 0;
        }
        
      }

      else if(g_PIDServiceData.Cur_Error > 6)//
      {
        if(Rdj_times > 3*50)
        {
          OP.di_up = 0;
          Rdj_times = 0;
          if(RelayCounter < 3)
          {
            if(g_PIDServiceData.Cur_ValvePoint < 500)
            {                
              if(VAL.V_p5_5__K5 < 100)
              {
                VAL.V_p5_5__K5 = VAL.V_p5_5__K5 + 1;
              }
              g_PIDServiceData.Kp = VAL.V_p5_5__K5;
            }
            else
            {                
              if(VAL.V_p5_6__K6 < 100)
              {
                VAL.V_p5_6__K6 = VAL.V_p5_6__K6 + 1;
              }
              g_PIDServiceData.Kp = VAL.V_p5_6__K6;
            } 
          }

        }
      }
      else
      {
        if((Rdj_times > 4*50)&&(AD.g_vlow <2)&&(AD.g_vlow >-2))
        {
          OP.di_up = 0;
          Rdj_times = 0;
        }            
      }
    }
    //求超调量  下行     
    if(OP.di_down >=1)
    {            
      if((g_PIDServiceData.Cur_Error >= -2)&&(g_PIDServiceData.Cur_Error <= 2)&&(1 == OP.di_down)) //超调
      {
        semi_wavex2 = Rdj_times;
        OP.di_down = 2;
        Rdj_times = 0;
      }
      if(2 == OP.di_down)
      {
        if(g_PIDServiceData.Cur_Error > 3)
        {              
          if (semi_wavey2 < g_PIDServiceData.Cur_ValvePoint)
          {
            if((AD.g_vlow <2)&&(AD.g_vlow >-2))
            {              
              OP.di_down = 0;
              semi_wavex = Rdj_times;
              Rdj_times = 0;
              g_PIDServiceData.SetPoint = g_PIDServiceData.Cur_ValvePoint;
              OutPut_Positive(0);
              if(semi_wavey2 < (g_PIDServiceData.SetPoint - 3))
              {
                if(RelayCounter < 3)
                {
                  if(g_PIDServiceData.Cur_ValvePoint < 500)
                  {                  
                    if(VAL.V_p5_7__K7 > 2)
                    {
                      VAL.V_p5_7__K7 = VAL.V_p5_7__K7 - 1;
                    }
                    g_PIDServiceData.Ki = VAL.V_p5_7__K7; 
                  }
                  else
                  {                  
                    if(VAL.V_p5_8__K8 > 2)
                    {
                      VAL.V_p5_8__K8 = VAL.V_p5_8__K8 - 1;
                    }
                    g_PIDServiceData.Ki = VAL.V_p5_8__K8; 
                  }
                }
                
              }               
              semi_wavey2 = 0;
            }               
          }
          else {semi_wavey2=g_PIDServiceData.Cur_ValvePoint;} 
        }
        else if((g_PIDServiceData.Cur_Error >= -3)&&(g_PIDServiceData.Cur_Error <= 3))
        {
          if(Rdj_times > 50)
          {
            Rdj_times = 0;
            OP.di_down =0;
          }
        }
        
        if(Rdj_times > 8*50)
        {
          OP.di_down = 0;
          Rdj_times = 0;
        }
        
      }
      else if(g_PIDServiceData.Cur_Error < -6)//
      {
        if(Rdj_times > 3*50)
        {
          OP.di_down = 0;
          Rdj_times = 0;
          if(RelayCounter < 3) 
          {
            if(g_PIDServiceData.Cur_ValvePoint < 500)              
            {                
              if(VAL.V_p5_7__K7 < 100)
              {
                VAL.V_p5_7__K7 = VAL.V_p5_7__K7 + 1;
              }                
              g_PIDServiceData.Ki = VAL.V_p5_7__K7;
            }
            else
            {
              if(VAL.V_p5_8__K8 < 100)
              {
                VAL.V_p5_8__K8 = VAL.V_p5_8__K8 + 1;
              }                
              g_PIDServiceData.Ki = VAL.V_p5_8__K8;               
            }
          }
        
        }
      }
      else
      {
        if((Rdj_times > 4*50)&&(AD.g_vlow <2)&&(AD.g_vlow >-2))
        {
          OP.di_down = 0;
          Rdj_times = 0;
        }            
      }            
    }
    //求超调量  下行     
    if(Rdj_times > g_usTimeAlarm)
    {//超时报警
      OutPut_Positive(0);
      VAL.V_p1_3BRND = V_p1_3 ;//恢复1.3   20220515
      SYS.bit.Step = SP_OK;
      return;   //极限位置无法确定
    }            
  }break;         
  
  
  

  case SP_OK:{   //保存数据
    VAL.V_p1_3BRND = V_p1_3 ;//恢复1.3   20220515
    if(PID_ZZDflag ==1)
    {
      //SYS.bit.Step = SP_OK;
      SYS.bit.Step = 0;
      KB.bit.CountKB = 0;
      if(CLK.OneKey_Flag ==1)
      {  
        CLK.OneKey_Flag =0;
        CLK.SecINC =0;
        SYS.bit.PageStateF = AUTO;
        SYS.bit.PageStateL = AUTO; 
        DIS.ScreenB = DIGITAL;
        DIS.SignL = DIS.SignR = CLK.SecCONFIRM = 0; 
        SYS.bit.PageS1 = 0; 
        SYS.bit.PageS2 = 0; 
        //OP.Function = SAVEF; 
        SAVE_FLH( P2_10F );
        SAVE_FLH( P3_4F ); 
        SAVE_FLH( P4_4F ); 
        SAVE_FLH( P1_6F );
        OP.Function = SAVEF;                                                             
      }
      SYS.bit.Step = 0;
      PID_ZZDflag =0;
      CLK.OneKey_Flag =0;
      //return; 
    }   
    
    
  }break;           
  default: ;
  }   
ERR:   
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
void P1_2_Rdj(void)
{ 
 
  switch( SYS.bit.Step )
  {
  case  STP_0:{
    Rdj_times +=1;
    if((VAL.V_p8_1_TRG < 1200)&&(VAL.V_p8_1_TRG >= 10))
    {
      g_usTimeAlarm = VAL.V_p8_1_TRG*ucTickClock; //20210111
    }
    else
    {
      g_usTimeAlarm = 200*ucTickClock;
    }            
    if(Rdj_times >ucTickClock)
    {
      SYS.bit.Step  =STP_1;
      Rdj_times =0;
//      RelayReadyflag = 0; //20201128
//      RelayEndflag = 0;
      
      semi_wavex2 = 0;
      semi_wavey2 = 0;
      PID_ZZDflag = 0; 
      semi_wavex1 = 0;    //20201128
      
      AD.firsttime = 0;  //重新开始填充AD  20201230   
      AD.pretime =0; 
      AD.maxSpeedUp = 0;
      AD.maxSpeedDown = 0;     
      AD.maxSpeedUpPos = 0;    
      AD.maxSpeedDownPos = 0;               
      g_uclitstate = 0; 
      Times1 = 0;
      Times2 = 0; 
      Times3 = 0; 
      Dir_Flag = 1;
      
      
      VAL.V_p3_1_MIN = 200; //20210223 放开限位
      VAL.V_p3_2_MAX = 3890;
      
      Testuptimes = 0;
      Testdowntimes = 0;
    } 
  }
  case STP_1:{             
    Rdj_times +=1;
    OutPut_Negetive(1000);
    if(Rdj_times>800)
    {
      if(Rdj_times%300 ==0)
      {
        ADC12_Convert(1);
        if( OP.Difference <5 )   //保存排空位置
        {       
          dwCnt++;
          if(dwCnt >= 5)
          {
            dwCnt = 0;                     
            if(OP.Meterage_L <2048)  
            {
              if ( OP.Meterage_L <  VAL.V_p3_1_MIN  )  { OP.Down_Limited =  VAL.V_p3_1_MIN; }// 为了安全起见 保存机械最小值
              else    {  OP.Down_Limited = OP.Meterage_L;     } // 保存限位  保存更小的值
              VAL.V_p3_3_RCT = V_CTCL;  //定位器排气是轴逆时针转
            }
            else 
            {
              if( OP.Meterage_L >  VAL.V_p3_2_MAX )    { OP.Up_Limited =  VAL.V_p3_2_MAX;   }    //为了安全起见 保存机械最小值        
              else    {  OP.Up_Limited = OP.Meterage_L;       }    //保存限位    保存更小的值                                  
              VAL.V_p3_3_RCT = V_CLOK; //定位器排气时顺时针转
            }
            Rdj_times =0;   
            SYS.bit.Step  =STP_2;   //结束排空位置                  
          }
        }
        else
        {
          dwCnt = 0;
        }
      }
    }   
  }break;
  case STP_2:{
    Rdj_times +=1;
    OutPut_Positive( 1000);          
    if(Rdj_times>800)
    {
      if(Rdj_times%300 ==0)
      {
        ADC12_Convert(1);
        
        if( OP.Difference <5 )   //保存全压位置
        {
          dwCnt++;
          if(dwCnt >= 5)
          {
            dwCnt = 0;            
            if(OP.Meterage_L <2048)  
            {
              if(VAL.V_p3_3_RCT == V_CTCL)
              {
                ALA.bit.Calc_ERR = 1; return;
              }
              if ( OP.Meterage_L <  VAL.V_p3_1_MIN  )  { OP.Down_Limited =  VAL.V_p3_1_MIN; }// 为了安全起见 保存机械最小值
              else    {  OP.Down_Limited = OP.Meterage_L;     } // 保存限位  保存更小的值
              VAL.V_p3_3_RCT = V_CLOK; //定位器排气时顺时针转
              //VAL.V_p4_3_VRL = V_OPEN;   //气关阀
            }
            else
            {
              if(VAL.V_p3_3_RCT == V_CLOK)
              {
                ALA.bit.Calc_ERR = 1; return;
              }                       
              if ( OP.Meterage_L >  VAL.V_p3_2_MAX )   { OP.Up_Limited =  VAL.V_p3_2_MAX;   }    //为了安全起见 保存机械最小值        
              else    {  OP.Up_Limited = OP.Meterage_L;       }    //保存限位    保存更小的值                                  
              VAL.V_p3_3_RCT = V_CTCL;  //定位器排气是轴逆时针转
              //VAL.V_p4_3_VRL = V_OFF;   //气开阀  
            }
            Rdj_times =0;   
            OutPut_Positive(0);  //停止充气
            SYS.bit.Step  = STP_3;   //结束充气位置 
          }
        }
        else
        {
          dwCnt = 0;         
        }        
      }
    } 
  }break;
  
  case STP_3:{  //检查中点及行程范围
    Rdj_times +=1;
    OP.Range_M = OP.Up_Limited - OP.Down_Limited;
    OP.Middle_Valve = (OP.Up_Limited + OP.Down_Limited)/2;                     //计算中位点
        
    if((VAL.V_p1_1_RCR&0x01) == V_ROTR)  //整定角行程
    {
      OP.Up_Limited_Test = OP.Middle_Valve + (OP.Up_Limited - OP.Middle_Valve)*0.7;               //计算上边界  为双作用更改 
      OP.Down_Limited_Test = OP.Middle_Valve - (OP.Middle_Valve - OP.Down_Limited)*0.7;           //计算下边界  为双作用更改 
    }
    else
    {
      OP.Up_Limited_Test = OP.Middle_Valve + (OP.Up_Limited - OP.Middle_Valve)*0.1;               //计算上边界  在这里有边界限制所以没有问题
      OP.Down_Limited_Test = OP.Middle_Valve - (OP.Middle_Valve - OP.Down_Limited)*0.1;           //计算下边界  之前是 *0.2    
    }
    //--------------------------------------------------------------中位点检查--------------------第四步
    if( OP.Middle_Valve > 3019 || OP.Middle_Valve < 1434 ) { ALA.bit.Zero_ERR = 1;  return; } //机械0位置超差报警 原为2662  1434   //要求可偏差30度
    
    if(OP.Range_M <50)  {ALA.bit.Calc_ERR = 1; return; }  //排机械卡死  
    else if(OP.Range_M <580) {  ALA.bit.Rng__ERR = 1; return; ; } //报夹角过小 
    else 
    {  
      AD.firsttime = 0;  //重新开始填充AD  20201230               
      AD.pretime =0;  
      K_R = (float)(OP.Range_M/1000.0);   
      
//      g_dangel_down = OP.Down_Limited;              
//      g_dangel_down = (double)(g_dangel_down - 2048)/4096*3.141592653; //自整定完即采用sin数据20211006
//      g_dsinAngel_down = sin(g_dangel_down); 
//      
//      g_dangel_up = OP.Up_Limited;
//      g_dangel_up = (double)(g_dangel_up - 2048)/4096*3.141592653;         
//      g_dsinAngel_up = sin(g_dangel_up); 
      UpCalculate();
      DownCalculate();
      SAVE_FLH( P2_10F );
      SAVE_FLH( P3_4F ); 
      SAVE_FLH( P4_4F ); 
      SAVE_FLH( P1_6F );
      OP.Function = SAVEF; 
      if((VAL.V_p1_1_RCR&0x01) == V_ROTR)  //整定角行程
      {      
      OP.down_pwm = 700;
      OP.up_pwm = 700;
      }
      
      SYS.bit.Step  = STP_4;   //参数检查结束
      Rdj_times =0;  //停止计时
    }
    
  }break;
  case STP_4:{   ////////////////////整定最大值
    Rdj_times +=1;   //计时  
    ZDmaxpwmCnt +=1;
    ADC12_Convert(1); 
    if(1 == g_uclitstate) //重新充气 以备下一步
    { 
      if(((VAL.V_p3_3_RCT == V_CTCL)&&( OP.Meterage_L > OP.Up_Limited-20))
         ||((VAL.V_p3_3_RCT == V_CLOK)&&( OP.Meterage_L < OP.Down_Limited+20)))
      {
        OutPut_Positive(0); 
        g_uclitstate = 0; 
        SYS.bit.Step  = STP_5;
        Rdj_times = 0;
      }
      else
      {
        OutPut_Positive( 1000);             
      }
    }
    else if(0 == g_uclitstate)
    {
      if((VAL.V_p1_1_RCR&0x01) == V_ROTR)  //整定角行程
      {        
        if(Dir_Flag ==1)//整定上行ROTR
        {
           
          if(VAL.V_p3_3_RCT == V_CTCL)
          {                         
            if( OP.Meterage_L > OP.Up_Limited_Test )
            {
              if((OP.Meterage_L > OP.Up_Limited_Test)
                 &&(OP.Meterage_L < OP.Up_Limited - OP.Range_M/10))//抛开两端
              {

                 Testuptimes = Rdj_times;
              }
              
              Times1 +=1;
              if(Times1>ucTickClock)
              {
                if(Testuptimes < VAL.V_p2_6_TUP*5)  //   3s   30*5 =150 = 3000/20
                {
                  OP.up_pwm -=40;  //时间过小需要重新整定
                }
                else { OP.di_up  =1;  }                 
                Dir_Flag =0;  //整定下行
                Times1=0;
                OutPut_Negetive(0);
                Rdj_times = 0;
              } 
            }
            else {                                 
              OutPut_Positive(OP.up_pwm);   
              if(( OP.Meterage_L < OP.Down_Limited + OP.Range_M/10)&&(OP.Meterage_L > OP.Down_Limited + 10)) //求平均速度
              {
                Rdj_times = 0;
              }
              else
              {;}            
            }
          }
          else //  OP.di_up = OP.di_down = 1;
          {
            if( OP.Meterage_L < OP.Down_Limited_Test )
            {
              if((OP.Meterage_L < OP.Down_Limited_Test)
                 &&(OP.Meterage_L > OP.Down_Limited +OP.Range_M/10))//抛开两端
              {
                 Testuptimes = Rdj_times;              
              }                

              Times1 +=1;
              if(Times1>ucTickClock)
              {
                if(Testuptimes < VAL.V_p2_6_TUP*5)  //   3s   30*5 =150 = 3000/20
                {
                  OP.up_pwm -=40;  //时间过小需要重新整定
                }
                else { OP.di_up  =1;  }                   
                Dir_Flag =0;  //整定下行
                Times1=0;
                OutPut_Negetive(0);
                Rdj_times = 0;
              } 
            }
            else {              
              OutPut_Positive(OP.up_pwm);  
               if(( OP.Meterage_L > OP.Up_Limited - OP.Range_M/10)&&(OP.Meterage_L < OP.Up_Limited - 10)) //求平均速度
              {
                Rdj_times = 0;
              }
              else
              {;}             
            }
          }
                   
        }
        else    //整定下行ROTR
        {
          if(VAL.V_p3_3_RCT == V_CTCL)
          {   
            if( OP.Meterage_L < OP.Down_Limited_Test )
            {
              //计算100%至0%的时间
              if((OP.Meterage_L < OP.Down_Limited_Test)
                 &&(OP.Meterage_L > OP.Down_Limited +OP.Range_M/10))//抛开两端
              {
                Testdowntimes = Rdj_times; 
                
              }
              Times1 +=1;
              if(Times1>ucTickClock)  //等待1s准备反向                
              {
                if(Testdowntimes < VAL.V_p2_7_TDN*5)  //   3s   30*5 =150 = 3000/20
                {
                  OP.down_pwm -=40;  //时间过小需要重新整定
                }
                else { OP.di_down  =1;  }                
                Dir_Flag =1;  //整定上行
                Times1=0;
                OutPut_Negetive(0);
                Rdj_times =0;  //停止计时               
              }                     
            }
            else {
              
              OutPut_Negetive(OP.down_pwm); 
              if(( OP.Meterage_L > OP.Up_Limited - OP.Range_M/10)
                 &&(OP.Meterage_L < OP.Up_Limited - 10)) //求平均速度
              {
                Rdj_times = 0;  //开始计时
              }
              else
              {;}           
            }
          }
          else
          {
            if( OP.Meterage_L > OP.Up_Limited_Test )
            {
              //计算100%至0%的时间
              if((OP.Meterage_L > OP.Up_Limited_Test)
                 &&(OP.Meterage_L < OP.Up_Limited - OP.Range_M/10))//抛开两端
              {
                Testdowntimes = Rdj_times; 
                
              }
              Times1 +=1;
              if(Times1>ucTickClock)  //等待1s准备反向                
              {
                if(Testdowntimes < VAL.V_p2_7_TDN*5)  //   3s   30*5 =150 = 3000/20
                {
                  OP.down_pwm -=40;  //时间过小需要重新整定
                }
                else { OP.di_down  =1;  }                
                Dir_Flag =1;  //整定上行
                Times1=0;
                OutPut_Negetive(0);
                Rdj_times =0;  //停止计时               
              }                      
            }
            else {
              OutPut_Negetive(OP.down_pwm);
              if(( OP.Meterage_L < OP.Down_Limited + OP.Range_M/10)
                 &&(OP.Meterage_L > OP.Down_Limited + 10)) //求平均速度
              {
                Rdj_times = 0;
              }
              else
              {;}               
            }
          }          
        }
      }
      else  //整定线行程
      {
        if(Dir_Flag ==1)//整定上行LINE
        {
          if(VAL.V_p3_3_RCT == V_CTCL)
          {   
            if( OP.Meterage_L > OP.Up_Limited_Test )
            {
              OutPut_Negetive(0);
              Times1 +=1;
              if(Times1>ucTickClock)
              {
                if( OP.Meterage_L > OP.Up_Limited_Test+80 )  { OP.up_pwm -=40;}
                else {  OP.di_up =1;  }
                Dir_Flag =0;  //整定下行
                Times1=0;
              } 
            }
            else {  OutPut_Positive(OP.up_pwm);   }
          }
          else //  OP.di_up = OP.di_down = 1;
          {
            if( OP.Meterage_L < OP.Down_Limited_Test )
            {
              OutPut_Negetive(0);
              Times1 +=1;
              if(Times1>ucTickClock)
              {
                if( OP.Meterage_L +80 < OP.Down_Limited_Test )  { OP.up_pwm -=40;}
                else { OP.di_up =1;  }
                Dir_Flag =0;  //整定下行
                Times1=0;
              } 
            }
            else {  OutPut_Positive(OP.up_pwm);   }
          }
        }
        else    //整定下行LINE
        {
          if(VAL.V_p3_3_RCT == V_CTCL)
          {   
            if( OP.Meterage_L < OP.Down_Limited_Test )
            {
              OutPut_Negetive(0);
              Times1 +=1;
              if(Times1>ucTickClock)
              {
                if( OP.Meterage_L +80 < OP.Down_Limited_Test )  { OP.down_pwm -=40;}
                else { OP.di_down  =1;  }
                Dir_Flag =1;  //整定上行
                Times1=0;
              }                     
            }
            else { OutPut_Negetive(OP.down_pwm); }
          }
          else
          {
            if( OP.Meterage_L > OP.Up_Limited_Test )
            {
              OutPut_Negetive(0);
              Times1 +=1;
              if(Times1>ucTickClock)
              {
                if( OP.Meterage_L > OP.Up_Limited_Test+80 )  { OP.down_pwm -=40;}
                else {  OP.di_down =1;  }
                Dir_Flag =1;  //整定上行
                Times1=0;
              }                     
            }
            else { OutPut_Negetive(OP.down_pwm); }
          }
        }
      }             
      
      if(OP.di_up && OP.di_down )  
      {
        g_usup_pwm_max = OP.up_pwm +20 ;
        g_usdown_pwm_max = OP.down_pwm + 20 ;
        VAL.V_p5_1__K1 = g_usup_pwm_max;
        VAL.V_p5_2__K2 = g_usdown_pwm_max;
        if(0 == g_uclitstate)
        {
          g_uclitstate = 1; //需要重新回到满气  只用1次  
        }
        else
        {;}
        OP.di_up =0;
        OP.di_down =0;
        //设定小值整定范围￡中值不整定
        OP.Up_Limited_Test = OP.Middle_Valve + (OP.Up_Limited - OP.Middle_Valve)*0.04;               //计算上边界  在这里有边界限制所以没有问题
        OP.Down_Limited_Test = OP.Middle_Valve - (OP.Middle_Valve - OP.Down_Limited)*0.04;  
        Times1 =0;             
        Times2 =0;
        if(OP.up_pwm>800)  OP.up_pwm=800;
        if(OP.down_pwm>800)  OP.down_pwm =800;
        ZDmaxpwmCnt = 0;
      } //参数检查结束
      if(ZDmaxpwmCnt > 120*ucTickClock)
      {
        g_usup_pwm_max = 650;
        g_usdown_pwm_max = 650;
        if((VAL.V_p1_1_RCR&0x01) == V_ROTR)  //整定角行程
        {
          g_usup_pwm_max = 450;
          g_usdown_pwm_max = 450;
        }           
          if(OP.di_up  ==1)
          {
            g_usup_pwm_max = OP.up_pwm;
          }
          if(OP.di_down  ==1)
          {
            g_usdown_pwm_max = OP.down_pwm;
          }     

        VAL.V_p5_1__K1 = g_usup_pwm_max;
        VAL.V_p5_2__K2 = g_usdown_pwm_max;             
        Rdj_times = 0;
        if(0 == g_uclitstate)
        {
          g_uclitstate = 1; //需要重新回到满气  只用1次  
        }             
        else
        {;}        
        OP.di_up =0;
        OP.di_down =0;
        //设定小值整定范围￡中值不整定
        OP.Up_Limited_Test = OP.Middle_Valve + (OP.Up_Limited - OP.Middle_Valve)*0.04;               //计算上边界  在这里有边界限制所以没有问题
        OP.Down_Limited_Test = OP.Middle_Valve - (OP.Middle_Valve - OP.Down_Limited)*0.04;  
        Times1 =0;             
        Times2 =0;
        if(OP.up_pwm>800)  OP.up_pwm=800;
        if(OP.down_pwm>800)  OP.down_pwm =800;  
        ZDmaxpwmCnt = 0;
      }
    }
    
    
  }break;             
  case STP_5:{  //计算100%至0%的时间
    Rdj_times +=1;      
    ADC12_Convert(1);
    
    OutPut_Negetive(g_usdown_pwm_max);
    if(VAL.V_p3_3_RCT == V_CTCL)
    {
      ValveRodSpeedNEW(1);   //20201230 求速度             
      if(( OP.Meterage_L > OP.Up_Limited - OP.Range_M/10)
         &&(OP.Meterage_L < OP.Up_Limited - 10)) //求平均速度
      {
        Rdj_times = 0;
      }
      else
      {;}
      if((OP.Meterage_L < OP.Down_Limited +OP.Range_M/8)
         &&(OP.Meterage_L > OP.Down_Limited +OP.Range_M/10))//抛开两端
      {
        VAL.V_p2_7_TDN = Rdj_times/5; 
      }             
      if( OP.Meterage_L < OP.Down_Limited+20 )
      {               
        //VAL.V_p2_7_TDN = Rdj_times;
        OutPut_Positive(0);  
        SYS.bit.Step  = STP_6;   //参数检查结束
        Rdj_times =0;  //停止计时
        AD.firsttime = 0;  //重新开始填充AD  20201230
        AD.pretime =0;                
      }
    }
    else
    {
      ValveRodSpeedNEW(0);   //20201230 求速度              
      if(( OP.Meterage_L < OP.Down_Limited + OP.Range_M/10)
         &&(OP.Meterage_L > OP.Down_Limited + 10)) //求平均速度
      {
        Rdj_times = 0;
      }
      else
      {;} 
      if((OP.Meterage_L > OP.Up_Limited - OP.Range_M/8)
         &&(OP.Meterage_L < OP.Up_Limited - OP.Range_M/10))//抛开两端
      {
        VAL.V_p2_7_TDN = Rdj_times/5;
      }
      if( OP.Meterage_L > OP.Up_Limited -20 )
      {
        
        //VAL.V_p2_7_TDN = Rdj_times;
        OutPut_Positive(0);
        SYS.bit.Step  = STP_6;   //参数检查结束
        Rdj_times =0;  //停止计时 
        AD.firsttime = 0;  //重新开始填充AD  20201230                
        AD.pretime =0;  
      } 
    } 
    if(Rdj_times>g_usTimeAlarm)
    {//超时报警
      ALA.bit.No_SCALE =1; return;   //极限位置无法确定
    }
  }break;
  case STP_6:{
    Rdj_times +=1;
    OutPut_Positive( g_usup_pwm_max);
    ADC12_Convert(1);             
    
    if(VAL.V_p3_3_RCT == V_CTCL)
    {
      ValveRodSpeedNEW(0);   //20201230 求速度             
      if(( OP.Meterage_L < OP.Down_Limited + OP.Range_M/10)&&(OP.Meterage_L > OP.Down_Limited + 10)) //求平均速度
      {
        Rdj_times = 0;
      }
      else
      {;}
      
      if((OP.Meterage_L > OP.Up_Limited - OP.Range_M/8)
         &&(OP.Meterage_L < OP.Up_Limited - OP.Range_M/10))//抛开两端
      {
        VAL.V_p2_6_TUP = Rdj_times/5;

      }             
      if( OP.Meterage_L > OP.Up_Limited -20)
      {                 
        //VAL.V_p2_6_TUP = Rdj_times;
        OutPut_Positive(0);  
        SYS.bit.Step  = STP_7;   //参数检查结束
        Rdj_times =0;  //停止计时
        AD.firsttime = 0;  //重新开始填充AD  20201230
        AD.pretime =0; 
        
      }
    }
    else
    {
      ValveRodSpeedNEW(1);   //20201230 求速度             
      if(( OP.Meterage_L > OP.Up_Limited - OP.Range_M/10)&&(OP.Meterage_L < OP.Up_Limited - 10)) //求平均速度
      {
        Rdj_times = 0;
      }
      else
      {;} 
      if((OP.Meterage_L < OP.Down_Limited +OP.Range_M/8)
         &&(OP.Meterage_L > OP.Down_Limited +OP.Range_M/10))//抛开两端
      {
        VAL.V_p2_6_TUP = Rdj_times/5;               
      }            
      if( OP.Meterage_L < OP.Down_Limited+20  )
      {                  
        //VAL.V_p2_6_TUP = Rdj_times;
        OutPut_Positive(0);
        SYS.bit.Step  = STP_7;   //参数检查结束
        Rdj_times =0;  //停止计时 
        AD.firsttime = 0;  //重新开始填充AD  20201230                
        AD.pretime =0;             
      } 
    } 
    if(Rdj_times>g_usTimeAlarm)
    {//超时报警
      ALA.bit.No_SCALE =1; return;   //极限位置无法确定
    }
    
  }break;           
  
  case STP_7:{   ////////////////////整定位移最小值
    Rdj_times +=1;   //计时   
    ADC12_Convert(1);
    ValveRodSpeedNEW(5);   //20210117 求位移 曲线           
    if(0 == g_uclitstate)
    {
      OutPut_Negetive(g_usdown_pwm_max); 
      
      if(((OP.Meterage_L < AD.maxSpeedDownPos)&&(VAL.V_p3_3_RCT == V_CTCL))
         ||((OP.Meterage_L > AD.maxSpeedUpPos)&&(VAL.V_p3_3_RCT == V_CLOK)))
      {  
        OutPut_Negetive(0);
        Rdj_times = 0;
        g_uclitstate  =1;
      }
      
    }
    else if(1 == g_uclitstate)
    {   
      if(Rdj_times>200)
      {
        if(VAL.V_p3_3_RCT == V_CTCL)             
        {               
          ValveRodSpeedNEW(3);   //20201230 求位移 
          VAL.V_p5_10_K0 = abs(AD.maxDletaDown)/K_R;               
        }
        else
        {
          ValveRodSpeedNEW(2);   //20201230 求位移 
          VAL.V_p5_10_K0 = abs(AD.maxDletaUp)/K_R;
        }             
        Rdj_times = 0;
        OP.di_up =0;
        OP.di_down =0; 
        Times2 =0;
        if(OP.up_pwm>200)  OP.up_pwm=200;
        if(OP.down_pwm>200)  OP.down_pwm =200;  
        if(VAL.V_p5_10_K0 > 150 ) //超过10%
        {
          VAL.V_p5_10_K0 = 80;
//          OP.down_pwm = 250;
        } 	
        if(VAL.V_p5_10_K0 < 50)		
        {
          VAL.V_p5_10_K0 = 80;
        }								
        g_uclitstate = 2;  //进入最小输出气量整定
      }
    }
    else if(2 == g_uclitstate)
    {
      if(AD.g_vlow >= -1) //等待200ms时间 没有动
      {
        if(1 == Times3) //新循环标志位
        {
          Times3 = 0;
        }
        Times1 +=1; 
        if(Times1 >= 5*50)  ////等待5s时间 没有动 则增大输出
        {
          Times1 = 0;
          if(OP.down_pwm < (VAL.V_p5_2__K2 -100))
          {
            OP.down_pwm  = OP.down_pwm +20; 
            OutPut_Negetive(OP.down_pwm);                    
          }              
        }
      }
      else if(AD.g_vlow <= -4)  //确认运动了  
      {
        if(0 == Times3)  //施密特切换
        {
          Times1 = 0;                   
          Times2 ++; //等待停下来  每次加1  测试3次
          if(Times2 >= 3)
          {
            OutPut_Negetive(800);
            g_uclitstate = 3;  //整定完毕
          }
          else
          {
            if(OP.down_pwm > 200)
            {                 
              OP.down_pwm  = OP.down_pwm -30;
            }
            OutPut_Negetive(0);                 
          }             
          Times3 = 1; //等待下一次静止
        }
        
        
      }
      else
      {
        ;
      }
      
    }
    else
    {
      ;
    }
    
    if((( OP.Meterage_L < OP.Down_Limited + 20)&&(VAL.V_p3_3_RCT == V_CTCL))
       ||((OP.Meterage_L > OP.Up_Limited -20)&&(VAL.V_p3_3_RCT == V_CLOK))
       ||(g_uclitstate == 3)||((Rdj_times > 40*50)&&(2 == g_uclitstate)))
    {
      OutPut_Negetive(0);
      Rdj_times =0;  //停止计时 
      AD.firsttime = 0;  //重新开始填充AD  20201230 
      AD.pretime =0;              
      g_uclitstate = 0; 
      Times1 = 0;
      Times2 = 0; 
      Times3 = 0;
      VAL.V_p5_4__K4 = OP.down_pwm; 
      SYS.bit.Step  = STP_8;
    }
    if(Rdj_times > g_usTimeAlarm)
    {//超时报警
      ALA.bit.No_SCALE =1; return;   //极限位置无法确定
    }                        
    
  }break; 
  
  case STP_8:{//
    
    Rdj_times +=1;   //计时   
    ADC12_Convert(1);
    ValveRodSpeedNEW(5);   //20210117 求位移 曲线 
    if(0 == g_uclitstate)
    { 
      OutPut_Positive( g_usup_pwm_max); 
      
      if((( OP.Meterage_L > AD.maxSpeedUpPos)&&(VAL.V_p3_3_RCT == V_CTCL))
         ||((OP.Meterage_L < AD.maxSpeedDownPos)&&(VAL.V_p3_3_RCT == V_CLOK)))              
      { 
        OutPut_Positive(0);
        Rdj_times = 0;
        g_uclitstate  =1;            
      }
    }
    else if(1 == g_uclitstate)
    {           
      if(Rdj_times>200) //4s
      {
        if(VAL.V_p3_3_RCT == V_CTCL)             
        {               
          ValveRodSpeedNEW(2);   //20201230 求位移 
          VAL.V_p5_9__K9 = abs(AD.maxDletaUp)/K_R; 
        }
        else
        {
          ValveRodSpeedNEW(3);   //20201230 求位移 
          VAL.V_p5_9__K9 = abs(AD.maxDletaDown)/K_R;               
        }
        
        OutPut_Positive(0);
        Rdj_times =0;  //停止计时  
        if(VAL.V_p5_9__K9 > 150 ) //超过10%
        {
          VAL.V_p5_9__K9 = 80;
//          OP.up_pwm = 250;
        }
 	
        if(VAL.V_p5_9__K9 < 50)		
        {
          VAL.V_p5_9__K9 = 80;
        }								
        g_uclitstate = 2;  //进入最小输出气量整定
        PID_ZZDflag = 1;    //保存数据标志位                         
      }                  
    } 
    else if(2 == g_uclitstate)
    {
      if(AD.g_vlow <= 1)  
      {
        if(1 == Times3) //新循环标志位
        {
          Times3 = 0;
        }              
        Times1 +=1; 
        if(Times1 >= 5*50)  ////等待5s时间 没有动 则增大输出
        {
          Times1 = 0;
          if(OP.up_pwm < (VAL.V_p5_1__K1 -100))
          {
            OP.up_pwm  = OP.up_pwm +20; 
            OutPut_Positive(OP.up_pwm);                    
          }              
        }
      }
      else if(AD.g_vlow >= 4)  //确认运动了  
      { 
        if(0 == Times3)  //施密特切换
        {
          Times1 = 0;
          Times2 ++; //等待停下来  每次加1  测试2次 
          if(Times2 >= 2)
          {                  
            g_uclitstate = 3;  //整定完毕
            VAL.V_p5_3__K3 = OP.up_pwm;                               
          }
          else
          {
            
            if(OP.up_pwm > 100)
            {                 
              OP.up_pwm  = OP.up_pwm -30;
            }
            OutPut_Positive(0);  
            
          }
          Times3 = 1; //等待下一次静止
        }             
      }
      else
      {
        ;
      }          
    }
    else
    {
      ;
    }
    
    if((( OP.Meterage_L > OP.Up_Limited -20)&&(VAL.V_p3_3_RCT == V_CTCL))
       ||((OP.Meterage_L < OP.Down_Limited +20)&&(VAL.V_p3_3_RCT == V_CLOK))
       ||(g_uclitstate == 3)||((Rdj_times > 40*50)&&(2 == g_uclitstate)))
    {               
      OutPut_Positive(0);                
      AD.firsttime = 0;  //重新开始填充AD  20201230                
      AD.pretime =0; 
      Times1 = 0;
      Times2 = 0; 
      Times3 = 0; 
      g_uclitstate = 0;
      VAL.V_p5_3__K3 = OP.up_pwm;              
      
      VAL.V_p5_5__K5 = 5;
      VAL.V_p5_6__K6 = 5;
      VAL.V_p5_7__K7 = 5;
      VAL.V_p5_8__K8 = 5;                
      
      if(AD.CurrentBack >OP.Down_Limited)
      {
        g_PIDServiceData.Cur_ValvePoint = (AD.CurrentBack -OP.Down_Limited)*1000.0/OP.Range_M;     //当前位置采样 采样值按0-1000来处理
      }
      else if(AD.CurrentBack > OP.Up_Limited ){ g_PIDServiceData.Cur_ValvePoint = 1000; }
      else 
      {
        g_PIDServiceData.Cur_ValvePoint =0;
      }
      
      if( VAL.V_p3_3_RCT == V_CLOK) 
      {
        g_PIDServiceData.Cur_ValvePoint = 1000-g_PIDServiceData.Cur_ValvePoint; 
        Dir_Flag = 0;
      }
      else
      {
        Dir_Flag = 1;    
      }
      g_PIDServiceData.SetPoint = g_PIDServiceData.Cur_ValvePoint; //给定初值
      if(g_PIDServiceData.Cur_ValvePoint <500)
      {
        g_PIDServiceData.Kp =VAL.V_p5_5__K5; 
        g_PIDServiceData.Ki =VAL.V_p5_7__K7;      
      }
      else
      {
        g_PIDServiceData.Kp =VAL.V_p5_6__K6; 
        g_PIDServiceData.Ki =VAL.V_p5_8__K8;  
      } 
      if(VAL.V_p5_9__K9 < 50)		
      {
        VAL.V_p5_9__K9 = 80;
      }	
      if(VAL.V_p5_10_K0 < 50)		
      {
        VAL.V_p5_10_K0 = 80;
      }	
      if(VAL.V_p5_9__K9 > 150)		
      {
        VAL.V_p5_9__K9 = 80;
      }	
      if(VAL.V_p5_10_K0 > 150)		
      {
        VAL.V_p5_10_K0 = 80;
      }							
      SAVE_FLH( P2_10F );
      SAVE_FLH( P3_4F ); 
      SAVE_FLH( P4_4F ); 
      SAVE_FLH( P1_6F );
      OP.Function = SAVEF;                 
      Rdj_times =0; 
      
      SYS.bit.Step  = STP_9;   //参数检查结束               
      
    }
    
    if(Rdj_times>g_usTimeAlarm)
    {//超时报警
      OutPut_Positive(0);             
      return;   //极限位置无法确定
    }
    
  }break;
  
  case STP_9:{   //整定输出K值
    Rdj_times +=1;   //计时                  
    if((Dir_Flag ==1)&&(0 == OP.di_up))//整定上行
    {
      g_PIDServiceData.SetPoint = g_PIDServiceData.Cur_ValvePoint + 60;
      OP.di_up =1;
      OP.di_down = 0;
      if((g_PIDServiceData.SetPoint > 750) ) 
      {g_PIDServiceData.SetPoint =750;Dir_Flag =0;OP.di_up =0;RelayCounter++;}
      
      
    }
    else if((Dir_Flag ==0)&&(0 == OP.di_down))
    {
      g_PIDServiceData.SetPoint = g_PIDServiceData.Cur_ValvePoint - 60;
      OP.di_down = 1;
      OP.di_up =0;
      
      if((g_PIDServiceData.SetPoint < 250))
      {g_PIDServiceData.SetPoint =250;Dir_Flag =1;OP.di_down = 0;RelayCounter++;}  
      
      
    }
    else
    {
      ;
    } 
    if(RelayCounter >= 3)
    {
      RelayCounter = 0;
      SYS.bit.Step = SP_OK;
    }
    
    PIDcontrol(); 
    //求超调量  上行
    if(OP.di_up >=1)
    {            
      if((g_PIDServiceData.Cur_Error >= -2)&&(g_PIDServiceData.Cur_Error <= 2)&&(1 == OP.di_up)) //超调
      {
        semi_wavex1 = Rdj_times;
        OP.di_up = 2;
        Rdj_times = 0;
      }
      if(2 == OP.di_up)
      {
        if(g_PIDServiceData.Cur_Error < -3)
        {              
          if (semi_wavey1>g_PIDServiceData.Cur_ValvePoint)//bug
          {
            if((AD.g_vlow <2)&&(AD.g_vlow >-2))
            {
              OP.di_up = 0;
              semi_wavex = Rdj_times;
              Rdj_times = 0;
              g_PIDServiceData.SetPoint = g_PIDServiceData.Cur_ValvePoint;
              OutPut_Positive(0);
              if(semi_wavey1 > (g_PIDServiceData.SetPoint + 3))
              {
                if(g_PIDServiceData.Cur_ValvePoint < 500)
                {
                  if(VAL.V_p5_5__K5 > 3)
                  {
                    VAL.V_p5_5__K5 = VAL.V_p5_5__K5 - 2;
                  }
                  g_PIDServiceData.Kp = VAL.V_p5_5__K5; 
                }
                else
                {
                  if(VAL.V_p5_6__K6 > 3)
                  {
                    VAL.V_p5_6__K6 = VAL.V_p5_6__K6 - 2;
                  }
                  g_PIDServiceData.Kp = VAL.V_p5_6__K6;                
                }
              }               
              semi_wavey1 = 0; 
            }
          }
          else {semi_wavey1=g_PIDServiceData.Cur_ValvePoint;} 
        }
        else if((g_PIDServiceData.Cur_Error >= -3)&&(g_PIDServiceData.Cur_Error <= 3))
        {
          if(Rdj_times > 50)
          {
            Rdj_times = 0;
            OP.di_up =0;
          }
        }
        
        if(Rdj_times > 8*50)
        {
          OP.di_up = 0;
          Rdj_times = 0;
        }
        
      }
      /*else if((g_PIDServiceData.Cur_Error >= -2)&&(g_PIDServiceData.Cur_Error <= 2)&&(3 == OP.di_up)) //超调
      {
      semi_wavex2 = Rdj_times;
      OP.di_up = 0;
    }*/
      else if(g_PIDServiceData.Cur_Error > 6)//
      {
        if(Rdj_times > 3*50)
        {
          OP.di_up = 0;
          Rdj_times = 0;
          if(g_PIDServiceData.Cur_ValvePoint < 500)
          {                
            if(VAL.V_p5_5__K5 < 100)
            {
              VAL.V_p5_5__K5 = VAL.V_p5_5__K5 + 2;
            }
            g_PIDServiceData.Kp = VAL.V_p5_5__K5;
          }
          else
          {                
            if(VAL.V_p5_6__K6 < 100)
            {
              VAL.V_p5_6__K6 = VAL.V_p5_6__K6 + 2;
            }
            g_PIDServiceData.Kp = VAL.V_p5_6__K6;
          }                 
        }
      }
      else
      {
        if((Rdj_times > 4*50)&&(AD.g_vlow <2)&&(AD.g_vlow >-2))
        {
          OP.di_up = 0;
          Rdj_times = 0;
        }            
      }
    }
    //求超调量  下行     
    if(OP.di_down >=1)
    {            
      if((g_PIDServiceData.Cur_Error >= -2)&&(g_PIDServiceData.Cur_Error <= 2)&&(1 == OP.di_down)) //超调
      {
        semi_wavex2 = Rdj_times;
        OP.di_down = 2;
        Rdj_times = 0;
      }
      if(2 == OP.di_down)
      {
        if(g_PIDServiceData.Cur_Error > 3)
        {              
          if (semi_wavey2 < g_PIDServiceData.Cur_ValvePoint)
          {
            if((AD.g_vlow <2)&&(AD.g_vlow >-2))
            {              
              OP.di_down = 0;
              semi_wavex = Rdj_times;
              Rdj_times = 0;
              g_PIDServiceData.SetPoint = g_PIDServiceData.Cur_ValvePoint;
              OutPut_Positive(0);
              if(semi_wavey2 < (g_PIDServiceData.SetPoint - 3))
              {
                if(g_PIDServiceData.Cur_ValvePoint < 500)
                {                  
                  if(VAL.V_p5_7__K7 > 3)
                  {
                    VAL.V_p5_7__K7 = VAL.V_p5_7__K7 - 2;
                  }
                  g_PIDServiceData.Ki = VAL.V_p5_7__K7; 
                }
                else
                {                  
                  if(VAL.V_p5_8__K8 > 3)
                  {
                    VAL.V_p5_8__K8 = VAL.V_p5_8__K8 - 2;
                  }
                  g_PIDServiceData.Ki = VAL.V_p5_8__K8; 
                }
              }               
              semi_wavey2 = 0;
            }               
          }
          else {semi_wavey2=g_PIDServiceData.Cur_ValvePoint;} 
        }
        else if((g_PIDServiceData.Cur_Error >= -3)&&(g_PIDServiceData.Cur_Error <= 3))
        {
          if(Rdj_times > 50)
          {
            Rdj_times = 0;
            OP.di_down =0;
          }
        }
        
        if(Rdj_times > 8*50)
        {
          OP.di_down = 0;
          Rdj_times = 0;
        }
        
      }
      else if(g_PIDServiceData.Cur_Error < -6)//
      {
        if(Rdj_times > 3*50)
        {
          OP.di_down = 0;
          Rdj_times = 0;
          if(g_PIDServiceData.Cur_ValvePoint < 500)
          {                
            if(VAL.V_p5_7__K7 < 100)
            {
              VAL.V_p5_7__K7 = VAL.V_p5_7__K7 + 2;
            }                
            g_PIDServiceData.Ki = VAL.V_p5_7__K7;
          }
          else
          {
            if(VAL.V_p5_8__K8 < 100)
            {
              VAL.V_p5_8__K8 = VAL.V_p5_8__K8 + 2;
            }                
            g_PIDServiceData.Ki = VAL.V_p5_8__K8;               
          }
        }
      }
      else
      {
        if((Rdj_times > 4*50)&&(AD.g_vlow <2)&&(AD.g_vlow >-2))
        {
          OP.di_down = 0;
          Rdj_times = 0;
        }            
      }            
    }
    //求超调量  下行     
    if(Rdj_times > g_usTimeAlarm)
    {//超时报警
      OutPut_Positive(0);
      SYS.bit.Step = SP_OK;
      return;   //极限位置无法确定
    }            
  }break;         
  
  
  
  /*case STP_9:{   //整定Kp,Ki.Kd
  //add时间限制160604
  Rdj_times +=1;   //可加超时报警
  PIDZZD( );  
  if( PID_ZZDflag ==1 )
  {
  SYS.bit.Step  = SP_OK;   //参数检查结束
  Rdj_times =0;  //停止计时  
  
  }
                        else
  {
  if(Rdj_times>1500)  //之前是9000
  {
  g_uspdtest = 300; //20201219                          
  }
  if(Rdj_times>9000)  //之前是9000
  {
  PID_ZZDflag =1;
  SYS.bit.Step  = SP_OK;   //参数检查结束
  Rdj_times =0;  //停止计时 
  g_uspdtest = 30; //20201219                              
  }
                           else
  {
  ;
  }
  }
  
  }break;  */         
  case SP_OK:{   //保存数据         
    if(PID_ZZDflag ==1)
    {
      //SYS.bit.Step = SP_OK;
      SYS.bit.Step = 0;
      KB.bit.CountKB = 0;
      if(CLK.OneKey_Flag ==1)
      {  
        CLK.OneKey_Flag =0;
        CLK.SecINC =0;
        SYS.bit.PageStateF = AUTO;
        SYS.bit.PageStateL = AUTO; 
        DIS.ScreenB = DIGITAL;
        DIS.SignL = DIS.SignR = CLK.SecCONFIRM = 0; 
        SYS.bit.PageS1 = 0; 
        SYS.bit.PageS2 = 0; 
        //OP.Function = SAVEF; 
        SAVE_FLH( P2_10F );
        SAVE_FLH( P3_4F ); 
        SAVE_FLH( P4_4F ); 
        SAVE_FLH( P1_6F );
        OP.Function = SAVEF;                                                             
      }
      SYS.bit.Step = 0;
      PID_ZZDflag =0;
      CLK.OneKey_Flag =0;
      //return; 
    }   
    
    
  }break;           
  default: ;
  }   
ERR:   
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////PID自整定相关函数/////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


/*************************************************
*		函数名：PIDZZD
*		说  明：PID自整定程序	
*		日  期：2015.12.05
*************************************************/
//void PIDZZD(void)
//{ 
//  // if (PID_ZZDflag==0) return;
//  if (PID_ZZDflag==1) return;
//  else {
//    ADC12_Convert(0);  //增加
//    if(AD.CurrentBack < OP.Down_Limited) { g_PIDServiceData.Cur_ValvePoint = 0; }
//    else { g_PIDServiceData.Cur_ValvePoint = (AD.CurrentBack -OP.Down_Limited)*1000.0/OP.Range_M;  }    //当前位置采样 采样值按0-1000来处理
//    if(VAL.V_p3_3_RCT == V_CLOK)
//    {
//      g_PIDServiceData.Cur_ValvePoint =1000 - g_PIDServiceData.Cur_ValvePoint;
//    }
//    if(RelayReadyflag == 0) {
//      ReadytoRelay();
//      return;
//    }
//    else {     
//      if (SWflag==1) {    // negative half-wave
//        semi_wavex2+=1;
//        if (g_PIDServiceData.Cur_ValvePoint < 500) {
//          g_PIDServiceData.LastValvePoint=g_PIDServiceData.Cur_ValvePoint;
//          if (g_PIDServiceData.Cur_ValvePoint>semi_wavey2 || g_PIDServiceData.Cur_ValvePoint == semi_wavey2) return;
//          else {
//            semi_wavey2=g_PIDServiceData.Cur_ValvePoint;
//            return;
//          }  
//        }
//        else {    // >=500
//          if (semi_wavex2<3) return;
//          else {
//            wave_formPAR();
//            if (RelayEndflag==1) {
//              calculateKpKiKd();
//              SYS.bit.Step = SP_OK;
//              PID_ZZDflag=1;
//              //关闭放气通道;
//              //关闭充气通道;
//              OutPut_Negetive( 0 );
//              //提示自整定完成;
//              return;                   
//            }
//            else {          //enter positive half-wave
//              SWflag=0;
//              g_PIDServiceData.LastValvePoint = g_PIDServiceData.Cur_ValvePoint;
//              semi_wavey1 = g_PIDServiceData.Cur_ValvePoint;
//              if (g_PIDServiceData.Cur_ValvePoint == 500) {
//                semi_wavex1=0;
//              }
//              else {
//                semi_wavex1=1;
//              }
//              if (RelayCounter>=4) {
//                RelayCounter=0;
//                changePWMduty();
//              }
//              else {
//                RelayCounter+=1;
//              }
//              //将0送充气D/A;
//              //将PWMduty_ZDS送放气D/A；
//              OutPut_Negetive( PWMduty_ZDS );
//              return;
//            }  
//          }  
//        }  
//      }  
//      else {             // positive half-wave
//        semi_wavex1+=1;
//        if (g_PIDServiceData.Cur_ValvePoint > 500) {
//          g_PIDServiceData.LastValvePoint=g_PIDServiceData.Cur_ValvePoint;
//          if (semi_wavey1>=g_PIDServiceData.Cur_ValvePoint) return;
//          else {
//            semi_wavey1=g_PIDServiceData.Cur_ValvePoint;
//            return;
//          }  
//        }
//        else {    // =<500
//          if (semi_wavex1<3) return;
//          else {              //enter negative half-wave
//            SWflag=1;
//            g_PIDServiceData.LastValvePoint=g_PIDServiceData.Cur_ValvePoint;
//            semi_wavey2=g_PIDServiceData.Cur_ValvePoint;
//            if (g_PIDServiceData.Cur_ValvePoint == 500) {
//              semi_wavex2=0;
//            }
//            else {
//              semi_wavex2=1;
//            }
//            //将0送放气D/A；
//            //将PWMduty_ZDF送充气D/A； 
//            OutPut_Positive( PWMduty_ZDF);
//            return;
//          }  
//        }  
//      }
//    }
//  }             
//}



/*************************************************
*		函数名：ReadytoRelay
*		说  明：为relay做好准备
*		日  期：2015.12.08 
*************************************************/
//void ReadytoRelay(void)
//{
//  if (ValveMoveDIRflag==1) {  // to 500 form 400
//    if (g_PIDServiceData.Cur_ValvePoint<500) {   
//      //将0送放气D/A;
//      //将500送充气D/A;
//      OutPut_Positive(PWMduty_ZDFL1);  //OutPut_Positive( 600 );
//      return;
//    }
//    else  {    // >=500
//      RelayReadyflag=1;
//      SWflag=0;
//      RelayDataEndflag=0;
//      DataSegmentNumber=0;
//      LastPer=990;   //  99.00E-002;
//      semi_wavey1=g_PIDServiceData.Cur_ValvePoint;
//      g_PIDServiceData.LastValvePoint=g_PIDServiceData.Cur_ValvePoint;
//      if (g_PIDServiceData.Cur_ValvePoint>500) {   // >500
//        semi_wavex1=1;
//      }
//      else {    // =500
//        semi_wavex1=0;
//      }
//      //将0送充气D/A;
//      //将700送放气D/A;
//      //OutPut_Negetive( 700);
//      PWMduty_ZDF = PWMduty_ZDFL1;   //PWMduty_ZDF =600;    //...600;
//      PWMduty_ZDS = PWMduty_ZDSH;  //PWMduty_ZDS =700;   //16.1.5 800;
//      OutPut_Negetive(PWMduty_ZDS);
//      
//      return;    
//    }
//  }
//  else  {
//    if (g_PIDServiceData.Cur_ValvePoint<400) {  // <400
//      ValveMoveDIRflag=1;
//      //将0送放气D/A;
//      //将500送充气D/A;
//      OutPut_Positive(PWMduty_ZDFL1);//OutPut_Positive( 600 );
//      return;    
//    }
//    else {    // >=400
//      ValveMoveDIRflag=0;
//      //将0送充气D/A;
//      //将700送放气D/A;
//      OutPut_Negetive(PWMduty_ZDSH);  //OutPut_Negetive( 700);
//      return;    
//    }
//  }
//}

/*************************************************
*		函数名：changePWMduty
*		说  明：改变PWM占空比
*		日  期：2015.12.08
*************************************************/
//void changePWMduty(void)
//{
//  if (DataSegmentNumber==0) {
//    if (PWMduty_ZDF > PWMduty_ZDFH1)  {  //之前是699 //if (PWMduty_ZDF>799) {  //之前是699
//      DataSegmentNumber=1;
//      PWMduty_ZDS = PWMduty_ZDSL;  //PWMduty_ZDS=600;
//      PWMduty_ZDF = PWMduty_ZDFH2;   //PWMduty_ZDF=700;  
//      return;      
//    }
//    else {
//      PWMduty_ZDS=PWMduty_ZDSH; //PWMduty_ZDS=700;   //700;
//      PWMduty_ZDF+=5;
//      return;    
//    }
//  }
//  else {
//    if (DataSegmentNumber==1) {
//      if (PWMduty_ZDF<PWMduty_ZDFL2) {//if (PWMduty_ZDF<533) {
//        DataSegmentNumber=2;
//        PWMduty_ZDS = PWMduty_ZDSL;
//        PWMduty_ZDF = PWMduty_ZDFH2;  
//        return;
//      }
//      else {
//        PWMduty_ZDS = PWMduty_ZDSL;//PWMduty_ZDS=600;
//        PWMduty_ZDF-=6;
//        return;
//      }
//    }
//    RelayDataEndflag=1;
//    return;
//    
//    
//    //    else {
//    //      if (DataSegmentNumber==2) {
//    //        if (PWMduty_ZDS>598) {
//    //        DataSegmentNumber=3;
//    //        PWMduty_ZDS=600;
//    //        PWMduty_ZDF=450;  
//    //        return;
//    //        }
//    //        else {
//    //          PWMduty_ZDF=500;
//    //          PWMduty_ZDS+=5;
//    //          return;
//    //        }        
//    //      }
//    //      else {   //... 3
//    //        if (PWMduty_ZDF>498) {
//    //          RelayDataEndflag=1;
//    //          return;
//    //        }
//    //        else {
//    //          PWMduty_ZDS=600;
//    //          PWMduty_ZDF+=5;
//    //          return;
//    //        } 
//    //      }
//    //    }
//  }
//}

/*************************************************
*		函数名：wave_formPAR
*		说  明：波形参数
*		日  期：2015.12.10
*************************************************/
//void wave_formPAR(void)
//{
//  unsigned int k;
//  //float i;   //151231
//  unsigned int i;
//  if (RelayDataEndflag==1) {
//    RelayEndflag=1;
//    return;    
//  }
//  else {
//    if (RelayCounter<4) {
//      RelayEndflag=0;
//      return;
//    }
//    else {
//      k=semi_wavex1+semi_wavex2;
//      if (semi_wavex1>semi_wavex2) {
//        i=(float)((semi_wavex1-semi_wavex2)*1000.0/k);
//      }
//      else {
//        i=(float)((semi_wavex2-semi_wavex1)*1000.0/k);        
//      }
//      //Itest =i/1000.0;
//      // if (i< 3.0000E-002) {
//      if (i< g_uspdtest) {   //30 20201219  之前20
//        semi_wavex=k;
//        
//        //semi_wavey=semi_wavey1-semi_wavey2;
//        if(semi_wavey1>semi_wavey2) {semi_wavey=semi_wavey1-semi_wavey2;}
//        else                        {semi_wavey=semi_wavey2-semi_wavey1;}
//        PWMduty_ZDF1=PWMduty_ZDF;     //2016.1.7 a  
//        PWMduty_ZDS1=PWMduty_ZDS;     //2016.1.7 a 
//        RelayEndflag=1;
//        return;
//      }
//      else {
//        RelayEndflag=0;
//        if (i>LastPer || i==LastPer) return;
//        else {
//          LastPer=i;
//          semi_wavex=k;
//          //semi_wavey=semi_wavey1-semi_wavey2;   //semi_y1
//          if(semi_wavey1>semi_wavey2) {semi_wavey=semi_wavey1-semi_wavey2;}
//          else                        {semi_wavey=semi_wavey2-semi_wavey1;}
//          PWMduty_ZDF1=PWMduty_ZDF;     //2016.1.7 a  
//          PWMduty_ZDS1=PWMduty_ZDS;     //2016.1.7 a 
//          return;
//        }
//      }      
//    }
//  }
//}

/*************************************************
*		函数名：calculateKpKiKd
*		说  明：计算PID的系数Kp、Ki、Kd
*		日  期：2015.12.10
*************************************************/
/*
void calculateKpKiKd(void)             
{
float d,a,Ti,Td;
d=(float)(PWMduty_ZDF+PWMduty_ZDS);
a=(float)semi_wavey;
Kp=4*d/3*3.14*a;
Ti=(float)semi_wavex/2;
Td=(float)semi_wavex/8;
Ki=Kp/Ti;
Kd=Kp*Td;
}
*/  
/*
//----以下是20160105 add 参数放大10倍 
void calculateKpKiKd(void)  
{
unsigned int  d,a,Ti,Td;
d=PWMduty_ZDF1+PWMduty_ZDS1;  //PWMduty_ZDF+PWMduty_ZDS
a=semi_wavey;
Kp=40*d/(5*3*a);   //4*d/3*3.14*a
Ti=semi_wavex/2;
Td=semi_wavex/8;
Ki=10*Kp/Ti;
if(Ki<10) Ki =10;
Kd=Kp*Td/10; //Kd=Kp*Td/6;   //Kd=Kp*Td

g_PIDServiceData.Kp = Kp;
g_PIDServiceData.Ki = Ki;
g_PIDServiceData.Kd = Kd;

VAL.V_p5_8__K8 = 0.07*Kp;   //求转换值

}
*/

//----以下是20160105 add 参数放大10倍 
//void calculateKpKiKd(void)  
//{
//  unsigned int  d,a,Ti,Td;
//  d=PWMduty_ZDF1+PWMduty_ZDS1;  //PWMduty_ZDF+PWMduty_ZDS
//  a=semi_wavey;
//  if(a<4) {a =4;}   //  add 2016.3.4
//  g_PIDServiceData.Kp = 40*d/(5*3*a);   //4*d/3*3.14*a
//  if(g_PIDServiceData.Kp >300)  {g_PIDServiceData.Kp = 300;  }      //  add 2016.3.4
//  Ti=semi_wavex/2;
//  Td=semi_wavex/8;
//  g_PIDServiceData.Ki = g_PIDServiceData.Kp/Ti;//  10*g_PIDServiceData.Kp/Ti;
//  if(g_PIDServiceData.Ki<10) g_PIDServiceData.Ki =10;
//  g_PIDServiceData.Kd=g_PIDServiceData.Kp*Td/10; //Kd=Kp*Td/6;   //Kd=Kp*Td
//  VAL.V_p5_6__K6 =(unsigned int)g_PIDServiceData.Kp;
//  VAL.V_p5_7__K7 =(unsigned int)g_PIDServiceData.Ki;
//  VAL.V_p5_8__K8 =  0.02*VAL.V_p5_6__K6;   //求转换值
//  
//}
///////////////////////////////////////////////////////////////////////////////
////////////////////////////////end////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------
void P1_4Step(void)
{
  //---------------------------
  SYS.bit.Step = STP_1;
  CLK.SecRun = 2;                                                             //做2秒等待
  while(CLK.SecRun!=0);  
  SYS.bit.Step = STP_2;
  while( 1 )
  {
    Man_IP();// 执行手动的程序
    ADC12_Convert(1); 
    //----------------------------------------------------------------
    if( OP.Meterage_L > 3890 )       { ALA.bit.Out__RNG = 1; }
    else if( OP.Meterage_L < 200 )   { ALA.bit.Out__RNG = 1; }
    else                                       { ALA.bit.Out__RNG = 0; }
    //----------------------------------------------------------------
    OP.p1_4  = (float)( OP.Meterage_L - 2048.0 )/4095.0*1000.0;   
    //----------------------------------------------------------------
    if( SYS.bit.Step!= STP_2 ) break;
  }
  CLK.SecRun = 2;                                                             //做2秒等待
  while(CLK.SecRun!=0);       
  //--------------------------------------------------------------------
  DIS.SignL = 0;
  //--------------------------------------------------------------------
  SYS.bit.Step = SP_OK;
  CLK.SecRun = 2;                                                             //做2秒等待
  while(CLK.SecRun!=0);  
  SYS.bit.Step = STP_0;
  KB.bit.CountKB = 0;
  
}

//--------------------
//void P1_5_Yes(void)
//{
//  unsigned char i = 0;
//  unsigned int tmp_I = 0;
//  unsigned int tmp_time = 0;     //
//  unsigned int error_flag = 0;   // 用来保存是否有超时错误的标记的
//  //---------------------------
//  SYS.bit.Step = STP_1;
//  //---------------------------   
//  CLK.SecRun = 3;                                                             //做2秒等待
//  while(CLK.SecRun!=0);  
//  //---------------------------                                                从4-20
//  tmp_I = OP.Range_C/4;
//  for( i=0; i<5 ; i++ )
//  {
//    SYS.bit.Step = STP_2 + i;
//    CLK.SecRun = 20;                                                           //做2秒等待
//    while( CLK.SecRun!=0 )
//    {
//      Auto_IP( OP.Down_C + i*tmp_I  );        // 计算的电流标定是换算之后的
//      if( OP.Difference_Ex < OP.BRND_1_3 )    // 这个地方要报警 控制超时
//      {
//        if( tmp_time > CLK.SecRun )  
//        {   
//          if ( VAL.V_p2_6_TUP < 20 - CLK.SecRun ) VAL.V_p2_6_TUP = 20 - CLK.SecRun;
//          SYS.bit.Step = STP_9;
//          CLK.SecRun = 1;                                                //做2秒等待
//          while( CLK.SecRun!=0 );   
//          SYS.bit.Step = STP_8;
//          CLK.SecRun = 1;                                                //做2秒等待
//          
//          while( CLK.SecRun!=0 );   CLK.SecRun = 2;
//          break; 
//        } 
//        else                         {  tmp_time = CLK.SecRun; }
//      }
//      else
//      {
//        tmp_time = 0;
//      }  
//      
//    }
//    if( CLK.SecRun == 0 )    
//    { 
//      error_flag = 1; SYS.bit.Step = SP_10; 
//      CLK.SecRun = 1;                                                //做2秒等待
//      while( CLK.SecRun!=0 ); 
//    }// { ALA.bit.Time_OUT = 1; }
//    ADC12_Convert(1); 
//    
//    if( OP.Meterage_L < OP.Down_4_1 - 100 || OP.Meterage_L > OP.Up_4_2 + 100 )  { ALA.bit.Vrlv_ERR = 1; goto ERR;}
//    else { ALA.bit.Vrlv_ERR = 0; }                                            //这个地方要报警 阀超范围 
//    if( OP.Function == NULLF )  {   OutPut_Negetive( 0 ); goto ERR; }
//  }
//  SYS.bit.Step = STP_3;
//  //---------------------------                                                从20-4
//  for( i=1; i<5 ; i++ )
//  {
//    CLK.SecRun = 20;                                                           //做2秒等待
//    SYS.bit.Step = STP_6 - i;
//    while(CLK.SecRun!=0)
//    {
//      Auto_IP( OP.Up_C - i*tmp_I  );          // 计算的电流标定是换算之后的
//      if( OP.Difference_Ex < OP.BRND_1_3 )    // 这个地方要报警 控制超时
//      {
//        if( tmp_time > CLK.SecRun )  
//        {  
//          if ( VAL.V_p2_7_TDN < 20 - CLK.SecRun ) VAL.V_p2_7_TDN = 20 - CLK.SecRun;
//          SYS.bit.Step = STP_9;
//          CLK.SecRun = 1;                                                //做2秒等待
//          while( CLK.SecRun!=0 ); 
//          SYS.bit.Step = STP_8;
//          CLK.SecRun = 1;                                                //做2秒等待
//          
//          while( CLK.SecRun!=0 );  CLK.SecRun = 2;
//          break; 
//        } 
//        else                         {  tmp_time = CLK.SecRun; }
//      }
//      else
//      {
//        tmp_time = 0;
//      }          
//    } 
//    if( CLK.SecRun == 0 )  
//    { 
//      error_flag = 1; SYS.bit.Step = SP_10; 
//      CLK.SecRun = 1;                                                //做2秒等待
//      while( CLK.SecRun!=0 ); 
//    }//{ ALA.bit.Time_OUT = 1; }
//    ADC12_Convert(1); 
//    if( OP.Meterage_L < OP.Down_4_1 - 100 || OP.Meterage_L > OP.Up_4_2 + 100)  { ALA.bit.Vrlv_ERR = 1; goto ERR; }
//    else { ALA.bit.Vrlv_ERR = 0; }                                            //这个地方要报警 阀超范围 
//    if( OP.Function == NULLF )  {   OutPut_Negetive( 0 ); goto ERR; }
//  }
//  SYS.bit.Step = STP_4;
//  //----------------------------
//  for( i=1 ; i<5 ; i++ )
//  {
//    switch(i)
//    {
//      //case 0 : tmp_I = OP.Down_C;              SYS.bit.Step = STP_2; break;
//    case 1 : tmp_I = OP.Up_C;                  SYS.bit.Step = STP_6; break;
//    case 2 : tmp_I = OP.Down_C + OP.Range_C/4; SYS.bit.Step = STP_3; break;
//    case 3 : tmp_I = OP.Up_C - OP.Range_C/4;   SYS.bit.Step = STP_5; break;
//    case 4 : tmp_I = OP.Down_C + OP.Range_C/2; SYS.bit.Step = STP_4; break;
//    default : break;
//    }  
//    CLK.SecRun = 20;                                                           //做2秒等待
//    while(CLK.SecRun!=0)
//    {
//      Auto_IP( tmp_I );
//      if( OP.Difference_Ex < OP.BRND_1_3 )    //这个地方要报警 控制超时
//      {
//        if( tmp_time > CLK.SecRun )  
//        {  
//          SYS.bit.Step = STP_9;
//          CLK.SecRun = 1;                                                //做2秒等待
//          while( CLK.SecRun!=0 ); 
//          SYS.bit.Step = STP_8;
//          CLK.SecRun = 1;                                                //做2秒等待
//          while( CLK.SecRun!=0 );  CLK.SecRun = 2;
//          break; 
//        } 
//        else                         {  tmp_time = CLK.SecRun; }
//      }
//      else
//      {
//        tmp_time = 0;
//      }  
//    }
//    if( CLK.SecRun == 0 )  
//    { 
//      error_flag = 1; SYS.bit.Step = SP_10; 
//      CLK.SecRun = 1;                                                //做2秒等待
//      while( CLK.SecRun!=0 ); 
//    } //{ ALA.bit.Time_OUT = 1; } 
//    ADC12_Convert(1); 
//    if(  VAL.V_p2_6_TUP > VAL.V_p2_7_TDN )  { VAL.V_p8_1_TRG = 200 + VAL.V_p2_6_TUP; }
//    else                                    { VAL.V_p8_1_TRG = 200 + VAL.V_p2_7_TDN; }
//    if( OP.Meterage_L < OP.Down_4_1 - 100 || OP.Meterage_L > OP.Up_4_2 + 100 )  { ALA.bit.Vrlv_ERR = 1; goto ERR;}
//    else { ALA.bit.Vrlv_ERR = 0; }                                            //这个地方要报警 阀超范围 
//    if( OP.Function == NULLF )  {   OutPut_Negetive( 0 ); goto ERR; }
//  }
//  if( error_flag )                                    {  ALA.bit.Time_OUT = 1; }  
//  if( OP.Meterage_L < 1884 || OP.Meterage_L > 3019 )  {  ALA.bit.Zero_ERR = 1; goto ERR;}   //  1884   2211  2013年11月1日
//  
//  SYS.bit.Step = STP_7;   
//  CLK.SecRun = 3;                                                              //做2秒等待
//  while(CLK.SecRun!=0);
//  SYS.bit.Step = SP_OK;
//  
//ERR:  
//  
//  SYS.bit.Step = STP_0;
//  KB.bit.CountKB = 0;
//}



//void P2_9_Tst( void )
//{
//  unsigned char i = 0;
//  unsigned int tmp_I = 0;
//  unsigned int tmp_time = 0;
//  unsigned int error_flag = 0;
//  unsigned int tmp_chr = VAL.V_p2_3_CHR;
//  VAL.V_p2_3_CHR = V_CUST;
//  //---------------------------
//  SYS.bit.Step = STP_1;
//  //---------------------------
//  //参数检查
//  for( i = 0; i<16 ; i++ )
//  {  
//    if ( VAL.Cust_c[ i ] >  VAL.Cust_c[ i + 1 ] ) 
//    {
//      ALA.bit.Break_AU = 1;  OP.Bre_cnt = E_CUSTOM_CURVE; 
//      return ;
//    }  
//  }     
//  //---------------------------   
//  CLK.SecRun = 3;                                                             //做2秒等待
//  while(CLK.SecRun!=0);  
//  //---------------------------                                                从4-20
//  tmp_I = OP.Range_C/16;
//  for( i=0; i<17 ; i++ )
//  {
//    SYS.bit.Step = STP_1 + i;
//    CLK.SecRun = 20;                                                           //做2秒等待
//    while( CLK.SecRun!=0 )
//    {
//      Auto_IP( OP.Down_C + i*tmp_I  );
//      if( OP.Difference_Ex < OP.BRND_1_3 )    //这个地方要报警 控制超时
//      {
//        if( tmp_time > CLK.SecRun )  
//        {  
//          SYS.bit.Step = SP_20;
//          CLK.SecRun = 1;                                                //做2秒等待
//          while( CLK.SecRun!=0 ); 
//          SYS.bit.Step = SP_19;
//          CLK.SecRun = 1;                                                //做2秒等待
//          while( CLK.SecRun!=0 );  CLK.SecRun = 2;
//          break; 
//        } 
//        else                         {  tmp_time = CLK.SecRun; }
//      }
//      else
//      {
//        tmp_time = 0;
//      }  
//      
//    }
//    if( CLK.SecRun == 0 )  
//    { 
//      error_flag = 1; SYS.bit.Step = SP_21; 
//      CLK.SecRun = 1;                                                //做2秒等待
//      while( CLK.SecRun!=0 ); 
//    } //{ ALA.bit.Time_OUT = 1; } 
//    ADC12_Convert(1); 
//    
//    if( OP.Meterage_L < OP.Down_4_1 - 20 || OP.Meterage_L > OP.Up_4_2 + 20 )  { ALA.bit.Vrlv_ERR = 1; goto ERR;}
//    else { ALA.bit.Vrlv_ERR = 0; }                                            //这个地方要报警 阀超范围 
//    if( OP.Function == NULLF )  {   OutPut_Negetive( 0 ); goto ERR; }
//  }
//  SYS.bit.Step = STP_3;
//  //---------------------------                                                从20-4
//  for( i=1; i<17 ; i++ )
//  {
//    CLK.SecRun = 20;                                                           //做2秒等待
//    SYS.bit.Step = SP_17 - i;
//    while(CLK.SecRun!=0)
//    {
//      Auto_IP( OP.Up_C - i*tmp_I  );
//      if( OP.Difference_Ex < OP.BRND_1_3 )    //这个地方要报警 控制超时
//      {
//        if( tmp_time > CLK.SecRun )  
//        {  
//          SYS.bit.Step = SP_20;
//          CLK.SecRun = 1;                                                //做2秒等待
//          while( CLK.SecRun!=0 ); 
//          SYS.bit.Step = SP_19;
//          CLK.SecRun = 1;                                                //做2秒等待
//          while( CLK.SecRun!=0 );  CLK.SecRun = 2;
//          break; 
//        } 
//        else                         {  tmp_time = CLK.SecRun; }
//      }
//      else
//      {
//        tmp_time = 0;
//      }          
//    } 
//    if( CLK.SecRun == 0 )  
//    { 
//      error_flag = 1; SYS.bit.Step = SP_21; 
//      CLK.SecRun = 1;                                                //做2秒等待
//      while( CLK.SecRun!=0 ); 
//    } //{ ALA.bit.Time_OUT = 1; } 
//    ADC12_Convert(1); 
//    if( OP.Meterage_L < OP.Down_4_1 - 20 || OP.Meterage_L > OP.Up_4_2 + 20 )  { ALA.bit.Vrlv_ERR = 1; goto ERR; }
//    else { ALA.bit.Vrlv_ERR = 0; }                                            //这个地方要报警 阀超范围 
//    if( OP.Function == NULLF )  {   OutPut_Negetive( 0 ); goto ERR; }
//  }
//  if( error_flag )                                    {  ALA.bit.Time_OUT = 1; }
//  // if( OP.Meterage_L < 1884 || OP.Meterage_L > 2211 )  {  ALA.bit.Zero_ERR = 1; goto ERR;}
//  
//  SYS.bit.Step = SP_18;   
//  CLK.SecRun = 3;                                                              //做2秒等待
//  while(CLK.SecRun!=0);
//  SYS.bit.Step = SP_OK;
//  
//ERR:  
//  
//  VAL.V_p2_3_CHR = tmp_chr;
//  SYS.bit.Step = STP_0;
//  KB.bit.CountKB = 0;
//}
//-----------------------------------------------------------------------
// 这是后来增加的 为了测量 机械行程的最小值
//-----------------------------------------------------------------------
void F3_1_MIN( void )
{
  //---------------------------
  SYS.bit.Step = STP_1;
  CLK.SecRun = 2;                                                             //做2秒等待
  while(CLK.SecRun!=0);  
  SYS.bit.Step = STP_2;
  while( 1 )
  {
    Man_IP();// 执行手动的程序
    ADC12_Convert(1); 
    //----------------------------------------------------------------- 
    if( OP.Meterage_L > ( 4095.0*MCH_SEN_PER ) )         { ALA.bit.Out__RNG = 1; }
    else if( OP.Meterage_L <  4095.0*(1-MCH_SEN_PER))    { ALA.bit.Out__RNG = 1; }
    else                                                 { ALA.bit.Out__RNG = 0; }
    //-----------------------------------------------------------------
    OP.p1_4  = (float)( OP.Meterage_L )/4095.0*1000.0;// = 机械行程/感器行程   后面的1000是用来嫁给显示的 
    //-----------------------------------------------------------------
    if( SYS.bit.Step!= STP_2 ) break;
  }
  // SYS.bit.Step = STP_3;
  ADC12_Convert(1); 
  CLK.SecRun = 2;                                                             //做2秒等待
  while(CLK.SecRun!=0);
  
  //--------------------------------------------------------------------
  if((VAL.V_p4_1_MIN == 0)&&(VAL.V_p4_2_MAX == 1000))
  {
     VAL.V_p3_1_MIN = AD.CurrentBack;// 保存机械行程最小值
     OP.Down_Limited =VAL.V_p3_1_MIN;   //将机械行程最小值定义为下限值  
     DownCalculate();      
  }
  else
  {
     VAL.V_p3_1_MIN = AD.CurrentBack;
     OP.Machine_Down_4_1 = VAL.V_p3_1_MIN;     
  }  

  
  //--------------------------------------------------------------------
  if( VAL.V_p3_1_MIN > VAL.V_p3_2_MAX )         {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_3_1_D_3_2; goto ERR; }//机械行程整定边界控制存在严重错误
  //-------------------------------------------------------------------
  SYS.bit.Step = STP_4;  
  CLK.SecRun = 2;                                                             //做2秒等待
  while(CLK.SecRun!=0);
  ERR:     
  SYS.bit.Step = SP_OK;
  KB.bit.CountKB = 0;
  
}

//-----------------------------------------------------------------------
// 这是后来增加的 为了测量 机械行程的最大值
//-----------------------------------------------------------------------
  unsigned int rangeA = 1000;  
  unsigned int rangeB = 2000;
void F3_2_MAX( void )
{
  //---------------------------
  SYS.bit.Step = STP_1;
  CLK.SecRun = 2;                                                             //做2秒等待
  while(CLK.SecRun!=0);  
  SYS.bit.Step = STP_2;
  while( 1 )
  {
    Man_IP();// 执行手动的程序
    ADC12_Convert(1); 
    //----------------------------------------------------------------
    if( OP.Meterage_L > ( 4095.0*MCH_SEN_PER )  )       { ALA.bit.Out__RNG = 1; }
    else if( OP.Meterage_L < 4095.0*(1-MCH_SEN_PER)  )  { ALA.bit.Out__RNG = 1; }
    else                                                { ALA.bit.Out__RNG = 0; }
    //----------------------------------------------------------------
    OP.p1_4  = (float)( OP.Meterage_L )/4095.0*1000.0;// = 机械行程/感器行程的90%*100%
    //----------------------------------------------------------------
    if( SYS.bit.Step!= STP_2 ) break;
  }
  ADC12_Convert(1);
  CLK.SecRun = 2;                                                             //做2秒等待
  while(CLK.SecRun!=0);
  
  //--------------------------------------------------------------------
  if((VAL.V_p4_1_MIN == 0)&&(VAL.V_p4_2_MAX == 1000))
  {  
    VAL.V_p3_2_MAX = AD.CurrentBack;    // 保存机械行程最大值
    OP.Up_Limited = VAL.V_p3_2_MAX;     //将机械行程最大值定义为上限值   
  }
  else
  {
    VAL.V_p3_2_MAX = AD.CurrentBack;
    OP.Machine_Up_4_2 = VAL.V_p3_2_MAX; //需要先设完4.1，4.2，3.1和3.2再计算才有效20221006
    rangeA = VAL.V_p4_2_MAX - VAL.V_p4_1_MIN;
    rangeB = OP.Machine_Up_4_2 - OP.Machine_Down_4_1;   
    OP.Range_M = (double)rangeB*1000/rangeA;
    rangeA = (double)rangeB*VAL.V_p4_1_MIN/rangeA;
    if(OP.Down_Limited > rangeA)  
    {
      OP.Down_Limited = OP.Machine_Down_4_1 - rangeA;
    }
    else
    {
      ;
    }
    OP.Up_Limited = OP.Down_Limited + OP.Range_M;    
    DownCalculate(); //此处才会推算线性数据20221006
  } 
    UpCalculate();  
  //--------------------------------------------------------------------
  if( VAL.V_p3_1_MIN > VAL.V_p3_2_MAX )         {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_3_1_D_3_2; goto ERR; }//机械行程整定边界控制存在严重错误
  //--------------------------------------------------------------------
  SYS.bit.Step = STP_4;
  
  CLK.SecRun = 2;                                                             //做2秒等待
  while(CLK.SecRun!=0);
ERR:      
  SYS.bit.Step = SP_OK;
  KB.bit.CountKB = 0;
}

//-------------------------------------------------------------------------
// 这个是用来配合参数修改的，和显示完全无关
//-------------------------------------------------------------------------
void P5_A_MOD( void )
{
  //---------------------------  
  // SYS.bit.Step = STP_1;
  // CLK.SecRun = 2;                                                             //做2秒等待
  // while(CLK.SecRun!=0);  
  //---------------------------                                                从4-20
  // SYS.bit.Step = STP_2;
  // CLK.SecRun = 2;                                                           //做2秒等待
  while( SYS.bit.PageS2 > P5_12_NULL && SYS.bit.PageS2 < P5_11RST )
  {
    //Auto_IP( 0 );
  }
  OutPut_Negetive( 0 );  delay(1000);
  
  // CLK.SecRun = 2;                                                             //做2秒等待
  // while(CLK.SecRun!=0); 
  // SYS.bit.Step = SP_OK;
  // KB.bit.CountKB = 0;
}

//----------------
//void P7_4_Tst(void)
//{ 
//  unsigned int data_temp;
//  if( VAL.V_p7_3_ACT == V_DIRC )
//  {
//    //--------------------- 第一步给出0%的值
//    SYS.bit.Step = STP_1; 
//    CLK.SecRun = 4;  
//    data_temp = VAL.V_p7_1_MIN;
//    SEND_AD421_Test( data_temp );        
//    while(CLK.SecRun);   //  停顿4秒
//    //--------------------- 第二步渐变到100%
//    SYS.bit.Step = STP_2;
//    while( data_temp < VAL.V_p7_2_MAX )
//    {  
//      data_temp += 10; 
//      SEND_AD421_Test( data_temp );
//      delay(10);
//    }    
//    //--------------------- 第三步给出100%的值
//    SYS.bit.Step = STP_3; 
//    CLK.SecRun = 4;
//    data_temp = VAL.V_p7_2_MAX; 
//    SEND_AD421_Test( data_temp );       
//    while(CLK.SecRun);   //  停顿4秒
//    SYS.bit.Step = SP_OK;
//  }
//  else
//  {
//    //--------------------- 第一步给出0%的值
//    SYS.bit.Step = STP_1; 
//    CLK.SecRun = 4;
//    data_temp = VAL.V_p7_2_MAX; 
//    SEND_AD421_Test( data_temp );        
//    while(CLK.SecRun);   //  停顿4秒
//    //--------------------- 第二步渐变到100%
//    SYS.bit.Step = STP_2;
//    while( data_temp > VAL.V_p7_1_MIN )
//    {  
//      data_temp -= 10; 
//      SEND_AD421_Test( data_temp );
//      delay(10);
//    }    
//    //--------------------- 第三步给出100%的值
//    SYS.bit.Step = STP_3; 
//    CLK.SecRun = 4;
//    data_temp = VAL.V_p7_1_MIN;
//    SEND_AD421_Test( data_temp );       
//    while(CLK.SecRun);   //  停顿4秒
//    SYS.bit.Step = SP_OK;    
//  }
//  KB.bit.CountKB = 0;
//}



void P5_11_Rst(void)
{      
  //------------------
  SYS.bit.Step = STP_1;      //控制显示进入运行阶段
  //------------------
  WDTCTL = WDTPW + WDTHOLD;   //关闭看门狗
  //  int *Flash_ptrA;                         // Segment A pointer
  unsigned int  temp_pointer[64]={0}; 
  unsigned int i;
  
  int *Flash_ptrB;   
  int *Flash_ptrA; 
  unsigned int temp[11] ;
  Flash_ptrB = (int *) SegmentB_address; 
  Flash_ptrA = (int *) SegmentA_address; 
  Flash_ptrA = (int *) 0x1000;              // Initialize Flash segment A pointer
  for (i=0; i<64; i++)
  {
    temp_pointer[i] = *Flash_ptrA++;      // Copy value segment A to segment B
  }
  FCTL1 = FWKEY;                           // Clear WRT bit
  //------------------------------------
  SYS.bit.Step = STP_2;     
  //------------------
  for (i=0; i<64; i++)
  {
    if( (i>v_p5_1__K1-1)&&(i<v_p5_10_K0+1) ) { temp[i-v_p5_1__K1] = *Flash_ptrB++; }
    else  {  temp[10] = *Flash_ptrB++;     }  
  }
  SYS.bit.Step = STP_3; 
  //------------------------------------------------------
  VAL.V_p5_1__K1 = temp[0];  
  VAL.V_p5_2__K2 = temp[1];    
  VAL.V_p5_3__K3 = temp[2]; 
  VAL.V_p5_4__K4 = temp[3]; 
  VAL.V_p5_5__K5 = temp[4];  
  VAL.V_p5_6__K6 = temp[5]; 
  VAL.V_p5_7__K7 = temp[6]; 
  VAL.V_p5_8__K8 = temp[7];  
  VAL.V_p5_9__K9 = temp[8]; 
  VAL.V_p5_10_K0 = temp[9];
  //------------------------------------------------------
  
  temp_pointer[v_p5_1__K1] = VAL.V_p5_1__K1;  
  temp_pointer[v_p5_2__K2] = VAL.V_p5_2__K2;    
  temp_pointer[v_p5_3__K3] = VAL.V_p5_3__K3; 
  temp_pointer[v_p5_4__K4] = VAL.V_p5_4__K4; 
  temp_pointer[v_p5_5__K5] = VAL.V_p5_5__K5;  
  temp_pointer[v_p5_6__K6] = VAL.V_p5_6__K6; 
  temp_pointer[v_p5_7__K7] = VAL.V_p5_7__K7; 
  temp_pointer[v_p5_8__K8] = VAL.V_p5_8__K8;  
  temp_pointer[v_p5_9__K9] = VAL.V_p5_9__K9; 
  temp_pointer[v_p5_10_K0] = VAL.V_p5_10_K0;
  
  //-------------------
  SYS.bit.Step = STP_4;
  while(FCTL3 & BUSY); 
  WDTCTL = WDTPW + WDTHOLD;     // Stop watchdog timer
  FCTL2 = FWKEY + FSSEL0 + FN0; // MCLK/2 for Flash Timing Generator
  FCTL1 = FWKEY + ERASE;        // Set Erase bit
  FCTL3 = FWKEY;                // Clear Lock bit
  *Flash_ptrA = 0;               // Dummy write to erase Flash segment
  FCTL1 = FWKEY + WRT;          // Set WRT bit for write operation
  //-------------------
  SYS.bit.Step = STP_5;
  for (i=0; i<64; i++)
  {
    *(Flash_ptrA++) = temp_pointer[i];   // Write value to flash
    while(!(FCTL3 & WAIT));   
  }
  //-------------------
  SYS.bit.Step = STP_6;
  FCTL1 = FWKEY;                            // Clear WRT bit
  while(FCTL3 & BUSY);
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
  WDTCTL = WDT_MRST_32;    //放狗,并选时钟源为SMCLK,即定时时间为SMCLK/32768
  _EINT();           
  //-------------------------------------------------------
  SYS.bit.Step = SP_OK; //显示运行成功
  KB.bit.CountKB = 0;
}

//-----------------
void P8_3_Frc(void)
{
  _DINT();                    
  WDTCTL = WDTPW + WDTHOLD; 
  //-------------------
  SYS.bit.Step = STP_1;
  unsigned int *Flash_ptr;     // Flash pointer
  unsigned int *temp_pointer2;
  unsigned int temp_pointer[64]={0};
  unsigned int i;
  Flash_ptr = (unsigned int *) SegmentA_address;  // Initialize Flash pointer
  temp_pointer2 = (unsigned int *) SegmentB_address; 
  //-------------------
  SYS.bit.Step = STP_2;
  while(FCTL3 & BUSY); 
  WDTCTL = WDTPW + WDTHOLD;     // Stop watchdog timer
  FCTL2 = FWKEY + FSSEL0 + FN0; // MCLK/2 for Flash Timing Generator
  FCTL1 = FWKEY + ERASE;        // Set Erase bit
  FCTL3 = FWKEY;                // Clear Lock bit
  *Flash_ptr = 0;               // Dummy write to erase Flash segment
  FCTL1 = FWKEY + WRT;          // Set WRT bit for write operation
  //-------------------
  SYS.bit.Step = STP_3;
  for (i=0; i<64; i++)
  {
    if( i==v_p8_4_MIN ) 
    {  
      temp_pointer2++;    *(Flash_ptr++) = temp_pointer[i] = VAL.V_p8_4_MIN;
    }
    else if ( i==v_p8_5_MAX )
    {
      temp_pointer2++;    *(Flash_ptr++) = temp_pointer[i] = VAL.V_p8_5_MAX;
    }
    else if ( i==v_p8_2_POS )
    {
      temp_pointer2++;    *(Flash_ptr++) = temp_pointer[i] = VAL.V_p8_2_POS;
    }    
    else
    {  
      temp_pointer[i] = *(Flash_ptr++) = *(temp_pointer2++);   
      while(FCTL3 & BUSY);     
    }         
  }
  //-------------------
  SYS.bit.Step = STP_4;
  //------------------------------------------------------
  VAL.V_p1_1_RCR = temp_pointer[v_p1_1_RCR];   
  VAL.V_p1_3BRND = temp_pointer[v_p1_3BRND];  
  VAL.V_p2_1_MIN = temp_pointer[v_p2_1_MIN];   
  VAL.V_p2_2_MAX = temp_pointer[v_p2_2_MAX];   
  VAL.V_p2_3_CHR = temp_pointer[v_p2_3_CHR];   
  VAL.V_p2_4_ACT = temp_pointer[v_p2_4_ACT];
  VAL.V_p2_5_QOF = temp_pointer[v_p2_5_QOF];  
  VAL.V_p2_6_TUP = temp_pointer[v_p2_6_TUP];  
  VAL.V_p2_7_TDN = temp_pointer[v_p2_7_TDN]; 
  
  VAL.V_p2_8_ST1 = temp_pointer[v_p2_8_ST1];
  VAL.V_p2_8_ST2 = temp_pointer[v_p2_8_ST2]; 
  VAL.V_p2_8_ST3 = temp_pointer[v_p2_8_ST3]; 
  VAL.V_p2_8_ST4 = temp_pointer[v_p2_8_ST4]; 
  VAL.V_p2_8_ST5 = temp_pointer[v_p2_8_ST5]; 
  VAL.V_p2_8_ST6 = temp_pointer[v_p2_8_ST6]; 
  VAL.V_p2_8_ST7 = temp_pointer[v_p2_8_ST7]; 
  VAL.V_p2_8_ST8 = temp_pointer[v_p2_8_ST8]; 
  VAL.V_p2_8_ST9 = temp_pointer[v_p2_8_ST9]; 
  VAL.V_p2_8_ST10 = temp_pointer[v_p2_8_ST10];  
  VAL.V_p2_8_ST11 = temp_pointer[v_p2_8_ST11]; 
  VAL.V_p2_8_ST12 = temp_pointer[v_p2_8_ST12]; 
  VAL.V_p2_8_ST13 = temp_pointer[v_p2_8_ST13]; 
  VAL.V_p2_8_ST14 = temp_pointer[v_p2_8_ST14]; 
  VAL.V_p2_8_ST15 = temp_pointer[v_p2_8_ST15];  
  VAL.V_p2_8_ST16 = temp_pointer[v_p2_8_ST16];  
  VAL.V_p2_8_ST17 = temp_pointer[v_p2_8_ST17];  
  
  VAL.V_p3_1_MIN = temp_pointer[v_p3_1_MIN]; 
  VAL.V_p3_2_MAX = temp_pointer[v_p3_2_MAX]; 
  VAL.V_p3_3_RCT = temp_pointer[v_p3_3_RCT];  
  
  VAL.V_p4_1_MIN = temp_pointer[v_p4_1_MIN];  
  VAL.V_p4_2_MAX = temp_pointer[v_p4_2_MAX];  
  VAL.V_p4_3_VRL = temp_pointer[v_p4_3_VRL];  
  
  VAL.V_p5_1__K1 = temp_pointer[v_p5_1__K1];  
  VAL.V_p5_2__K2 = temp_pointer[v_p5_2__K2];    
  VAL.V_p5_3__K3 = temp_pointer[v_p5_3__K3]; 
  VAL.V_p5_4__K4 = temp_pointer[v_p5_4__K4]; 
  VAL.V_p5_5__K5 = temp_pointer[v_p5_5__K5];  
  VAL.V_p5_6__K6 = temp_pointer[v_p5_6__K6]; 
  VAL.V_p5_7__K7 = temp_pointer[v_p5_7__K7]; 
  VAL.V_p5_8__K8 = temp_pointer[v_p5_8__K8];  
  VAL.V_p5_9__K9 = temp_pointer[v_p5_9__K9]; 
  VAL.V_p5_10_K0 = temp_pointer[v_p5_10_K0];
  
  VAL.V_p6_1__LK = temp_pointer[v_p6_1__LK];   
  VAL.V_p6_2__SV = temp_pointer[v_p6_2__SV];     
  VAL.V_p6_3__ZO = temp_pointer[v_p6_3__ZO];  
  VAL.V_p6_4__VR = temp_pointer[v_p6_4__VR];   
  VAL.V_p6_5__TO = temp_pointer[v_p6_5__TO];
  VAL.V_p6_6_MIN = temp_pointer[v_p6_6_MIN];   
  VAL.V_p6_7_MAX = temp_pointer[v_p6_7_MAX]; 
  
  VAL.V_p7_1_MIN = temp_pointer[v_p7_1_MIN];  
  VAL.V_p7_2_MAX = temp_pointer[v_p7_2_MAX];  
  VAL.V_p7_3_ACT = temp_pointer[v_p7_3_ACT];
  
  VAL.V_p8_1_TRG = temp_pointer[v_p8_1_TRG];  
  VAL.V_p8_2_POS = temp_pointer[v_p8_2_POS]; //FCO不恢复
  
  VAL.V_p8_6_POP = temp_pointer[v_p8_6_POP];       
  VAL.V_p8_7_RDJ = temp_pointer[v_p8_7_RDJ];
  
  OP.Up_Limited = temp_pointer[v_Up_Limited];       
  OP.Down_Limited = temp_pointer[v_Down_Limited]; 
  OP.Middle_Valve = temp_pointer[v_Middle_Valve];   
  
  FCTL1 = FWKEY;
  while(FCTL3 & BUSY);             // Clear WRT bit
  FCTL3 = FWKEY + LOCK;            // Set LOCK bit
  _EINT();  
  WDTCTL = WDT_MRST_32; 
  SYS.bit.Step = SP_OK;
  KB.bit.CountKB = 0;
}

//---------------------
void P8_4_Min(void)
{
  unsigned char i;
  SYS.bit.Step = STP_1;
  delay(3000);
  ADC12_Convert(1);
  VAL.V_p8_4_MIN = AD.CurrentIn;
  //     AD.K=723/VAL.V_p8_4_MIN;
  SYS.bit.Step = STP_2;
  delay(6000);
  //--------------------------------------------------------------------------------------------
  if( VAL.V_p8_4_MIN > 900  )                   {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_8_4_UP; goto ERR;}//8_4设置存在严重偏差
  if( VAL.V_p8_4_MIN < 500  )                   {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_8_4_DN; goto ERR;}//8_4设置存在严重偏差
  //--------------------------------------------------------------------------------------------
ERR:       
  SYS.bit.Step = SP_OK;
  KB.bit.CountKB = 0;
}
//---------------------
void P8_5_Max(void)
{
  unsigned char i;
  SYS.bit.Step = STP_1;
  delay(3000);
  ADC12_Convert(1);
  VAL.V_p8_5_MAX = AD.CurrentIn;
  //VAL.V_p8_5_MAX = AD.CurrentIn+4;
  SYS.bit.Step = STP_2;
  delay(6000);
  //----------------------------------------------------------------------------------------------
  if( VAL.V_p8_5_MAX > 3800 )  { ALA.bit.Break_AU = 1; OP.Bre_cnt = E_8_5_UP; goto ERR; }//8_5设置存在严重偏差
  if( VAL.V_p8_5_MAX < 3400 )  { ALA.bit.Break_AU = 1; OP.Bre_cnt = E_8_5_DN; goto ERR; }//8_5设置存在严重偏差
  //----------------------------------------------------------------------------------------------
ERR: 
  SYS.bit.Step = SP_OK;    
  KB.bit.CountKB = 0;
}
//----------------------
void SAVE_FLH(  unsigned int tmp )
{
  SYS.bit.Step = STP_1;
  
  int *Flash_ptrA;                         // Segment A pointer
  //unsigned int  temp_pointer[64]; 
  unsigned int i;
  unsigned int *Flash_ptr;    // Flash pointer
  unsigned int temp_pointer[64]={0};    
  //unsigned int i; 
  _DINT();                    // disable all interrupts　
  WDTCTL = WDTPW + WDTHOLD;   //关闭看门狗
  Flash_ptrA = (int *) 0x1000;              // Initialize Flash segment A pointer
  for (i=0; i<64; i++)
  {
    temp_pointer[i] = *Flash_ptrA++;      // Copy value segment A to segment B
  }
  FCTL1 = FWKEY;                           // Clear WRT bit
  _EINT();
  SYS.bit.Step = STP_2;
  
  // WDTCTL = WDTPW + WDTHOLD;   //关闭看门狗
  
  Flash_ptr = (unsigned int *) SegmentA_address;    // Initialize Flash pointer       
  while(FCTL3 & BUSY);   
  WDTCTL = WDT_MRST_32;
  //------------------------------------------------------
  if( tmp == P1_6F )
  {
    temp_pointer[v_p1_1_RCR] = VAL.V_p1_1_RCR; 
    temp_pointer[v_p5_1__K1] = VAL.V_p5_1__K1;  
    temp_pointer[v_p5_2__K2] = VAL.V_p5_2__K2;    
    temp_pointer[v_p5_3__K3] = VAL.V_p5_3__K3; 
    temp_pointer[v_p5_4__K4] = VAL.V_p5_4__K4; 
    temp_pointer[v_p5_5__K5] = VAL.V_p5_5__K5;  
    temp_pointer[v_p5_6__K6] = VAL.V_p5_6__K6; 
    temp_pointer[v_p5_7__K7] = VAL.V_p5_7__K7; 
    temp_pointer[v_p5_8__K8] = VAL.V_p5_8__K8;  
    temp_pointer[v_p5_9__K9] = VAL.V_p5_9__K9; 
    temp_pointer[v_p5_10_K0] = VAL.V_p5_10_K0;
    temp_pointer[v_p1_3BRND] = VAL.V_p1_3BRND;
    temp_pointer[v_p2_6_TUP] = VAL.V_p2_6_TUP;  
    temp_pointer[v_p2_7_TDN] = VAL.V_p2_7_TDN;
    temp_pointer[v_p8_1_TRG] = VAL.V_p8_1_TRG;
  }
  else if( tmp == P2_10F )
  {  
    if( VAL.V_p2_1_MIN > VAL.V_p2_2_MAX )         {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_2_1_D_2_2; }//起始与结束电流设置存在严重错误
    {
      temp_pointer[v_p2_1_MIN] = VAL.V_p2_1_MIN;   // 这个值要判断才行
      temp_pointer[v_p2_2_MAX] = VAL.V_p2_2_MAX;   // 这个值也要判断 
      temp_pointer[v_p6_6_MIN] = VAL.V_p6_6_MIN;   
      temp_pointer[v_p6_7_MAX] = VAL.V_p6_7_MAX; 
    }
    
    temp_pointer[v_p2_3_CHR] = VAL.V_p2_3_CHR;   
    temp_pointer[v_p2_4_ACT] = VAL.V_p2_4_ACT;
    temp_pointer[v_p2_5_QOF] = VAL.V_p2_5_QOF;  
    temp_pointer[v_p2_6_TUP] = VAL.V_p2_6_TUP;  
    temp_pointer[v_p2_7_TDN] = VAL.V_p2_7_TDN; 
    
    for( i = 0; i<16 ; i++ )
    {  
      if ( VAL.Cust_c[ i ] >  VAL.Cust_c[ i + 1 ] ) 
      {
        ALA.bit.Break_AU = 1;  OP.Bre_cnt = E_CUSTOM_CURVE;  break;
      }  
    }
    if( i==16 )
    {  
      temp_pointer[v_p2_8_ST1] = VAL.V_p2_8_ST1;
      temp_pointer[v_p2_8_ST2] = VAL.V_p2_8_ST2; 
      temp_pointer[v_p2_8_ST3] = VAL.V_p2_8_ST3; 
      temp_pointer[v_p2_8_ST4] = VAL.V_p2_8_ST4; 
      temp_pointer[v_p2_8_ST5] = VAL.V_p2_8_ST5; 
      temp_pointer[v_p2_8_ST6] = VAL.V_p2_8_ST6; 
      temp_pointer[v_p2_8_ST7] = VAL.V_p2_8_ST7; 
      temp_pointer[v_p2_8_ST8] = VAL.V_p2_8_ST8; 
      temp_pointer[v_p2_8_ST9] = VAL.V_p2_8_ST9; 
      temp_pointer[v_p2_8_ST10] = VAL.V_p2_8_ST10; 
      temp_pointer[v_p2_8_ST11] = VAL.V_p2_8_ST11; 
      temp_pointer[v_p2_8_ST12] = VAL.V_p2_8_ST12; 
      temp_pointer[v_p2_8_ST13] = VAL.V_p2_8_ST13; 
      temp_pointer[v_p2_8_ST14] = VAL.V_p2_8_ST14; 
      temp_pointer[v_p2_8_ST15] = VAL.V_p2_8_ST15;  
      temp_pointer[v_p2_8_ST16] = VAL.V_p2_8_ST16; 
      temp_pointer[v_p2_8_ST17] = VAL.V_p2_8_ST17; 
      
      VAL.Cust_c[0] = VAL.V_p2_8_ST1;
      VAL.Cust_c[1] = VAL.V_p2_8_ST2;
      VAL.Cust_c[2] = VAL.V_p2_8_ST3;
      VAL.Cust_c[3] = VAL.V_p2_8_ST4;
      VAL.Cust_c[4] = VAL.V_p2_8_ST5;
      VAL.Cust_c[5] = VAL.V_p2_8_ST6;
      VAL.Cust_c[6] = VAL.V_p2_8_ST7;
      VAL.Cust_c[7] = VAL.V_p2_8_ST8;
      VAL.Cust_c[8] = VAL.V_p2_8_ST9;
      VAL.Cust_c[9] = VAL.V_p2_8_ST10;
      VAL.Cust_c[10] = VAL.V_p2_8_ST11;
      VAL.Cust_c[11] = VAL.V_p2_8_ST12;
      VAL.Cust_c[12] = VAL.V_p2_8_ST13;
      VAL.Cust_c[13] = VAL.V_p2_8_ST14;
      VAL.Cust_c[14] = VAL.V_p2_8_ST15;
      VAL.Cust_c[15] = VAL.V_p2_8_ST16;
      VAL.Cust_c[16] = VAL.V_p2_8_ST17;
    }    
  }
  else if( tmp == P3_4F )
  {
    if( VAL.V_p3_1_MIN > VAL.V_p3_2_MAX )              {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_3_1_D_3_2; }//机械行程整定边界控制存在严重错误
    else if( VAL.V_p3_2_MAX - VAL.V_p3_1_MIN < 401 )   {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_MACHINE_10;} //机械行程只用到了传感器范围的10%
    else
    {
      temp_pointer[v_p3_1_MIN] = VAL.V_p3_1_MIN; 
      temp_pointer[v_p3_2_MAX] = VAL.V_p3_2_MAX; 
    }
    temp_pointer[v_p3_3_RCT] = VAL.V_p3_3_RCT;  
  }
  else if( tmp == P4_4F )
  { 
    if( VAL.V_p4_1_MIN > VAL.V_p4_2_MAX )              {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_4_1_D_4_2; }//控制行程设置存在严重错误
    else if( VAL.V_p4_2_MAX - VAL.V_p4_1_MIN < 101 )   {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_CONTROL_10;} //控制行程只用到了机械行程的10% 
    else
    {  
      temp_pointer[v_p4_1_MIN] = VAL.V_p4_1_MIN;  
      temp_pointer[v_p4_2_MAX] = VAL.V_p4_2_MAX; 
    }   
    temp_pointer[v_p4_3_VRL] = VAL.V_p4_3_VRL;  
  }
  else if( tmp == P5_12F )
  {  
    temp_pointer[v_p5_1__K1] = VAL.V_p5_1__K1;  
    temp_pointer[v_p5_2__K2] = VAL.V_p5_2__K2;    
    temp_pointer[v_p5_3__K3] = VAL.V_p5_3__K3; 
    temp_pointer[v_p5_4__K4] = VAL.V_p5_4__K4; 
    temp_pointer[v_p5_5__K5] = VAL.V_p5_5__K5;  
    temp_pointer[v_p5_6__K6] = VAL.V_p5_6__K6; 
    temp_pointer[v_p5_7__K7] = VAL.V_p5_7__K7; 
    temp_pointer[v_p5_8__K8] = VAL.V_p5_8__K8;  
    temp_pointer[v_p5_9__K9] = VAL.V_p5_9__K9; 
    temp_pointer[v_p5_10_K0] = VAL.V_p5_10_K0;
  }
  else if( tmp == P6_8F )
  {  
    temp_pointer[v_p6_1__LK] = VAL.V_p6_1__LK;   
    temp_pointer[v_p6_2__SV] = VAL.V_p6_2__SV;     
    temp_pointer[v_p6_3__ZO] = VAL.V_p6_3__ZO;  
    temp_pointer[v_p6_4__VR] = VAL.V_p6_4__VR;   
    temp_pointer[v_p6_5__TO] = VAL.V_p6_5__TO;
    temp_pointer[v_p6_6_MIN] = VAL.V_p6_6_MIN;   
    temp_pointer[v_p6_7_MAX] = VAL.V_p6_7_MAX; 
  }
  else if( tmp == P7_5F )
  {  
    if( VAL.V_p7_1_MIN > VAL.V_p7_2_MAX )         {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_7_1_D_7_2; }//反馈起始与结束电流设置存在严重错误
    else
    {
      temp_pointer[v_p7_1_MIN] = VAL.V_p7_1_MIN;  
      temp_pointer[v_p7_2_MAX] = VAL.V_p7_2_MAX;
    }    
    temp_pointer[v_p7_3_ACT] = VAL.V_p7_3_ACT;
  }
  else if( tmp == P8_8F )
  {  
    temp_pointer[v_p8_1_TRG] = VAL.V_p8_1_TRG;  
    temp_pointer[v_p8_2_POS] = VAL.V_p8_2_POS;
    
    if( VAL.V_p8_4_MIN > 900  )                   {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_8_4_UP; }//8_4设置存在严重偏差
    else if( VAL.V_p8_4_MIN < 500  )              {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_8_4_DN; }//8_4设置存在严重偏差
    else                                          {  temp_pointer[v_p8_4_MIN] = VAL.V_p8_4_MIN; }
    
    if( VAL.V_p8_5_MAX > 3800 )                   {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_8_5_UP; }//8_5设置存在严重偏差 提示错误
    else if( VAL.V_p8_5_MAX < 3400 )              {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_8_5_DN; }//8_5设置存在严重偏差
    else                                          {  temp_pointer[v_p8_5_MAX] = VAL.V_p8_5_MAX;   }
    
    temp_pointer[v_p8_6_POP] = VAL.V_p8_6_POP;       
    temp_pointer[v_p8_7_RDJ] = VAL.V_p8_7_RDJ;
  }
  
  else if(  tmp == PQ_KF )
  {
    temp_pointer[v_p1_1_RCR] = VAL.V_p1_1_RCR;
    
    temp_pointer[v_p5_1__K1] = VAL.V_p5_1__K1;  
    temp_pointer[v_p5_2__K2] = VAL.V_p5_2__K2;    
    temp_pointer[v_p5_3__K3] = VAL.V_p5_3__K3; 
    temp_pointer[v_p5_4__K4] = VAL.V_p5_4__K4; 
    temp_pointer[v_p5_5__K5] = VAL.V_p5_5__K5;  
    temp_pointer[v_p5_6__K6] = VAL.V_p5_6__K6; 
    temp_pointer[v_p5_7__K7] = VAL.V_p5_7__K7; 
    temp_pointer[v_p5_8__K8] = VAL.V_p5_8__K8;  
    temp_pointer[v_p5_9__K9] = VAL.V_p5_9__K9; 
    temp_pointer[v_p5_10_K0] = VAL.V_p5_10_K0;
    temp_pointer[v_p1_3BRND] = VAL.V_p1_3BRND;
    
    temp_pointer[v_p4_3_VRL] = VAL.V_p4_3_VRL;
  }
  else;
  temp_pointer[v_Up_Limited] = OP.Up_Limited;       
  temp_pointer[v_Down_Limited] = OP.Down_Limited; 
  temp_pointer[v_Middle_Valve] = OP.Middle_Valve;   
  SYS.bit.Step = STP_3;
  _DINT();
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  FCTL2 = FWKEY + FSSEL0 + FN0;             // MCLK/2 for Flash Timing Generator
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  FCTL3 = FWKEY;                            // Clear Lock bit
  *Flash_ptr = 0;                           // Dummy write to erase Flash segment
  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation
  for (i=0; i<64; i++) 
  {
    *(Flash_ptr++) = temp_pointer[i];   // Write value to flash
    while(!(FCTL3 & WAIT));   
  }
  SYS.bit.Step = STP_4;
  FCTL1 = FWKEY;                            // Clear WRT bit
  while(FCTL3 & BUSY);
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
  WDTCTL = WDT_MRST_32;    //放狗,并选时钟源为SMCLK,即定时时间为SMCLK/32768
  SYS.bit.Step = SP_OK;
  _EINT();  
  //-------------------------------------------------------------------电流控制范围
  SYS.bit.Step = STP_5;
  
  OP.Range_C = (VAL.V_p8_5_MAX - VAL.V_p8_4_MIN)/160.0*(VAL.V_p2_2_MAX - VAL.V_p2_1_MIN);  //计算电流的全程标定    这里要做检查
  OP.Up_C = VAL.V_p8_5_MAX - (VAL.V_p8_5_MAX - VAL.V_p8_4_MIN)/160.0*(200.0-VAL.V_p2_2_MAX);    //计算电流的标定最大值
  OP.Down_C = VAL.V_p8_4_MIN + (VAL.V_p8_5_MAX - VAL.V_p8_4_MIN)/160.0*(VAL.V_p2_1_MIN-40.0);  //计算电流标定的最小值
  
  OP.Up_C_alarm = VAL.V_p8_4_MIN + (VAL.V_p8_5_MAX - VAL.V_p8_4_MIN)/160.0*(VAL.V_p6_7_MAX-40.0);    //计算电流的标定最大值
  OP.Down_C_alarm = VAL.V_p8_5_MAX -(VAL.V_p8_5_MAX - VAL.V_p8_4_MIN)/160.0*(200.0-VAL.V_p6_6_MIN);  //计算电流标定的最小值
  
  for( i=0;i<17;i++ )
  {  
    VAL.Fit_c[i] = OP.Down_C + (float)OP.Range_C/(16.0)*i;    //VAL.Fit_c[i] = OP.Down_C + OP.Range_C/16.0*i; 
  }   
  VAL.Fit_c[17] = (float)OP.Range_C/(16.0); 
  //---------------------------------------------------------------------------
  VAL.Cust_c[0] = VAL.V_p2_8_ST1;
  VAL.Cust_c[1] = VAL.V_p2_8_ST2;
  VAL.Cust_c[2] = VAL.V_p2_8_ST3;
  VAL.Cust_c[3] = VAL.V_p2_8_ST4;
  VAL.Cust_c[4] = VAL.V_p2_8_ST5;
  VAL.Cust_c[5] = VAL.V_p2_8_ST6;
  VAL.Cust_c[6] = VAL.V_p2_8_ST7;
  VAL.Cust_c[7] = VAL.V_p2_8_ST8;
  VAL.Cust_c[8] = VAL.V_p2_8_ST9;
  VAL.Cust_c[9] = VAL.V_p2_8_ST10;
  VAL.Cust_c[10] = VAL.V_p2_8_ST11;
  VAL.Cust_c[11] = VAL.V_p2_8_ST12;
  VAL.Cust_c[12] = VAL.V_p2_8_ST13;
  VAL.Cust_c[13] = VAL.V_p2_8_ST14;
  VAL.Cust_c[14] = VAL.V_p2_8_ST15;
  VAL.Cust_c[15] = VAL.V_p2_8_ST16;
  VAL.Cust_c[16] = VAL.V_p2_8_ST17;
  //---------------------------------------------------------------------------  
  
  OP.Range_M = OP.Up_Limited - OP.Down_Limited;                        // 实际机械行程范围
  
  //OP.Machine_Up   = 4095 * ( 1 - MCH_SEN_PER)/2 + 4095 * MCH_SEN_PER * VAL.V_p3_2_MAX / 1000.0 ;  //机械行程的极限位置 这个值是提醒用户要注意安装范围的
  //OP.Machine_Down = 4095 * ( 1 - MCH_SEN_PER)/2 + 4095 * MCH_SEN_PER * VAL.V_p3_1_MIN / 1000.0 ;  //机械行程的极限位置
  
  //OP.Machine_Up   = 4095.0 * ( VAL.V_p3_2_MAX/1000.0); 
  //OP.Machine_Down = 4095.0 * ( VAL.V_p3_1_MIN/1000.0); 
  
  OP.Down_4_1 = OP.Down_Limited;     //20221006
  OP.Up_4_2 = OP.Range_M + OP.Down_Limited;       //20221006  
//  OP.Down_4_1 = OP.Range_M*(VAL.V_p4_1_MIN/1000.0) + OP.Down_Limited;     //控制行程最小 这个百分比是按照机械行程范围计算的
//  OP.Up_4_2 = OP.Range_M*(VAL.V_p4_2_MAX/1000.0) + OP.Down_Limited;       //控制行程最大 这个百分比是按照机械行程范围计算的
  OP.Middle_Valve = ( OP.Down_4_1 + OP.Up_4_2 )/2;
  OP.Range_D = OP.Up_4_2 - OP.Down_4_1;
  //---------------------------------------------------------------------------0位置超差计算
  //  if ( ( 4095 - OP.Up_4_2 ) > OP.Down_4_1 )  { OP.Up_50 = (float)Cv_Fit[ VAL.V_p2_3_CHR ][ 8 ]*4.095;}                            // - (4095 - OP.Up_4_2 - OP.Down_4_1);    }
  //  else                                       { OP.Up_50 = (float)Cv_Fit[ VAL.V_p2_3_CHR ][ 8 ]*4.095*( OP.Up_4_2 - OP.Down_4_1);} // + OP.Down_4_1 - ( 4095 - OP.Up_4_2 );  }
  //---------------------------------------------------------------------------   
  OP.Down_2_5 = OP.Range_D*(VAL.V_p2_5_QOF/1000.0) + OP.Down_4_1;    // 速关最小
  OP.Up_2_5 = OP.Up_4_2 - OP.Range_D*(VAL.V_p2_5_QOF/1000.0);        // 速关最大
  
  OP.BRND_1_3 = OP.Range_D/1000.0*VAL.V_p1_3BRND;                    // 控制精度
  OP.BRND = OP.Range_D/1000.0*2.0;                                   // 关死范围控制
  
  //OP.Range_D = OP.Up_4_2 - OP.Down_4_1;                            // 控制行程范围
  KB.bit.CountKB = 0;
  SYS.bit.Step = SP_OK;
}
//------------------------- 组态运行 -------------------------------------------
void Config_IP(void)
{
  if( OP.Function )
  {  
    switch(OP.Function)
    {
    case P1_2F : 
      {
                 if(1 == VAL.V_p6_4__VR)//高摩擦力
                 {
                  P1_2_Rdj_H();
                 }
                 else
                 {
                  P1_2_Rdj();                 
                 }
      OP.Function = NULLF; 
    }
      break;
    case P1_4F : P1_4Step(); OP.Function = NULLF; break;
//    case P1_5F : P1_5_Yes(); OP.Function = NULLF; break; 
    //--------------------------------------------------
//    case P2_9F : P2_9_Tst(); OP.Function = NULLF; break; 
    //--------------------------------------------------
    case P3_1F : F3_1_MIN(); OP.Function = NULLF; break;
    case P3_2F : F3_2_MAX(); OP.Function = NULLF; break;
    //-------------------------------------------------- 
    case P5_AF :  P5_A_MOD(); OP.Function = NULLF; break;
    case P5_11F : P5_11_Rst(); OP.Function = NULLF; break;
    //--------------------------------------------------
//    case P7_4F : P7_4_Tst(); OP.Function = NULLF; break;
    case P8_3F : P8_3_Frc(); OP.Function = NULLF; break;
    case P8_4F : P8_4_Min(); OP.Function = NULLF; break;
    case P8_5F : P8_5_Max(); OP.Function = NULLF; break;
    //--------------------------------------------------
    case P1_6F  : SAVE_FLH( P1_6F );  OP.Function = NULLF;  break;
    case P2_10F : SAVE_FLH( P2_10F ); OP.Function = NULLF;  break;
    case P3_4F  : SAVE_FLH( P3_4F );  OP.Function = NULLF;  break;
    case P4_4F  : SAVE_FLH( P4_4F );  OP.Function = NULLF;  break;
    case P5_12F : SAVE_FLH( P5_12F ); OP.Function = NULLF;  break; 
    case P6_8F  : SAVE_FLH( P6_8F );  OP.Function = NULLF;  break;
    case P7_5F  : SAVE_FLH( P7_5F );  OP.Function = NULLF;  break;
    case P8_8F  : SAVE_FLH( P8_8F );  OP.Function = NULLF;  break;
    case SAVEF  : Read_SegA ();       OP.Function = NULLF;  break;
    case NULLF : break ;
    default :    break ;
    }
  }  
}


void Quick_IP(void)
{
  switch(OP.Function)
  {
  case P1_2F : 
    {
                 if(1 == VAL.V_p6_4__VR)//高摩擦力
                 {
                  P1_2_Rdj_H();
                 }
                 else
                 {
                  P1_2_Rdj();                 
                 }       
    OP.Function = NULLF; 
    }
    break;             
  case P1_4F : P1_4Step();        OP.Function = NULLF; break; 
  case PQ_KF : SAVE_FLH( PQ_KF ); OP.Function = NULLF; break;
  case NULLF : break ;
  default :    break ;
  }
}


void RunControl(void)
{
  switch( SYS.bit.PageStateL )
  {
    //        case AUTO   : Auto_IP( 0 );  break;
  case AUTO   : 
    {
    if(1 == VAL.V_p6_4__VR)//高摩擦力
    {
      PIDcontrol_H();
    }
    else
    {
      PIDcontrol();      
    }
    }
    break;
  case MAN    : Man_IP();      break; 
  case CONFIG : Config_IP();   break;
  case QUICK  : Quick_IP();   break;  //SYS.bit.PageS2=1为解决快速使用速写完显示温度的问题
  default : break;
  }
}



