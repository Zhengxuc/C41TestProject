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
//      else  //整定线行程
//      {
//        if(Dir_Flag ==1)//整定上行LINE
//        {
//          if(VAL.V_p3_3_RCT == V_CTCL)
//          {   
//            if( OP.Meterage_L > OP.Up_Limited_Test )
//            {
//              OutPut_Negetive(0);
//              Times1 +=1;
//              if(Times1>ucTickClock)
//              {
//                if( OP.Meterage_L > OP.Up_Limited_Test+80 )  { OP.up_pwm -=40;}
//                else {  OP.di_up =1;  }
//                Dir_Flag =0;  //整定下行
//                Times1=0;
//              } 
//            }
//            else {  OutPut_Positive(OP.up_pwm);   }
//          }
//          else //  OP.di_up = OP.di_down = 1;
//          {
//            if( OP.Meterage_L < OP.Down_Limited_Test )
//            {
//              OutPut_Negetive(0);
//              Times1 +=1;
//              if(Times1>ucTickClock)
//              {
//                if( OP.Meterage_L +80 < OP.Down_Limited_Test )  { OP.up_pwm -=40;}
//                else { OP.di_up =1;  }
//                Dir_Flag =0;  //整定下行
//                Times1=0;
//              } 
//            }
//            else {  OutPut_Positive(OP.up_pwm);   }
//          }
//        }
//        else    //整定下行LINE
//        {
//          if(VAL.V_p3_3_RCT == V_CTCL)
//          {   
//            if( OP.Meterage_L < OP.Down_Limited_Test )
//            {
//              OutPut_Negetive(0);
//              Times1 +=1;
//              if(Times1>ucTickClock)
//              {
//                if( OP.Meterage_L +80 < OP.Down_Limited_Test )  { OP.down_pwm -=40;}
//                else { OP.di_down  =1;  }
//                Dir_Flag =1;  //整定上行
//                Times1=0;
//              }                     
//            }
//            else { OutPut_Negetive(OP.down_pwm); }
//          }
//          else
//          {
//            if( OP.Meterage_L > OP.Up_Limited_Test )
//            {
//              OutPut_Negetive(0);
//              Times1 +=1;
//              if(Times1>ucTickClock)
//              {
//                if( OP.Meterage_L > OP.Up_Limited_Test+80 )  { OP.down_pwm -=40;}
//                else {  OP.di_down =1;  }
//                Dir_Flag =1;  //整定上行
//                Times1=0;
//              }                     
//            }
//            else { OutPut_Negetive(OP.down_pwm); }
//          }
//        }
//      }             
      
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

//            if(1 == VAL.V_p6_2__SV) // 大摩擦力模式，默认为0
//            {
             OutPut_Negetive(0);              
             g_uclitstate = 4;  //小量整定完毕 开始整定快速启动量 
             Times2 = 0;
             VAL.V_p6_4__VR = 2;//快速启动补气时间
//            }
//            else
//            {
//             OutPut_Negetive(800);              
//             g_uclitstate = 3;  //小量整定完毕 开始整定快速启动量 
             
//            }
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
        VAL.V_p6_4__VR = Times1;
        if(Times1 > 25)
        {
          VAL.V_p6_4__VR = 25;   //不超过0.5s       
        }
        if(Times1 < 2)
        {
          VAL.V_p6_4__VR = 2;        
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
            VAL.V_p6_3__ZO = 2;
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
        VAL.V_p6_3__ZO = Times1;
        if(Times1 > 25)
        {
          VAL.V_p6_3__ZO = 25;   //不超过0.5s       
        }
        if(Times1 < 2)
        {
          VAL.V_p6_3__ZO = 2;        
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
//      if(VAL.V_p6_3__ZO > 2*VAL.V_p6_4__VR)
//      {
//        VAL.V_p6_3__ZO = VAL.V_p6_3__ZO/2;
//      }
//      else if(VAL.V_p6_4__VR > 2*VAL.V_p6_3__ZO)
//      {
//        VAL.V_p6_4__VR = VAL.V_p6_4__VR/2;
//      }
        
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
  
//         if(1 == VAL.V_p6_2__SV) // 大摩擦力模式，默认为0
//         {
//           SAVE_FLH( P2_10F );
//           SAVE_FLH( P3_4F ); 
//           SAVE_FLH( P4_4F ); 
//           SAVE_FLH( P1_6F );
//           OP.Function = SAVEF; 
//           RelayCounter = 4;   // 进入大摩擦力模式，整定
//         }
//         else //非大摩擦力，直接返回
//         {
           RelayCounter = 0;             
           SYS.bit.Step = SP_OK;           
//         }
      }
      else // RelayCounter == 4;进入大摩擦力整定
      {
//        if(Dir_Flag == 1) //往大值走
//        {
//          if( VAL.V_p3_3_RCT == V_CLOK) 
//          {
//            
//          }
//          else
//          {
//            OutPut_Positive(g_usup_pwm_max); 
//            if(OP.Meterage_L > Middle_Valve)
//            {
//             ;
//            }
//          }
//          
//        }

      }      
       
    }
    else
    {
      RelayCounter = 0;
      VAL.V_p1_3BRND = V_p1_3 ;//恢复1.3   20220604
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

