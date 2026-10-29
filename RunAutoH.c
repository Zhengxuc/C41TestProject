#include "Device.h"
//------------------------------------------------------------------------------



/*************************************************
*		函数名：PIDcotrol_H
*		说  明：PID控制程序 摩擦力								
*		日  期：2023.10.6
*************************************************/
void PIDcontrol_H(void)
{
  int a,b;
  unsigned char enflag = 0;  //2%以内使能补气增加偏移  

  static unsigned char  ucwaite = 0;  
  static unsigned char  ucGasCnt = 0; //启动时间
  static unsigned char  ucPIDstate =0; //模式
  static unsigned char  ucbrk =0; //刹车
  if(0 == VAL.V_p6_5__TO) //防止默认参数为0，不能除
  {
    VAL.V_p6_5__TO = 10;
  }
  if(2 > VAL.V_p6_6_MIN) //防止默认参数为0，不能除
  {
    VAL.V_p6_6_MIN = 2;
  }
  if(2>  VAL.V_p6_7_MAX) //防止默认参数为0，不能除
  {
    VAL.V_p6_7_MAX = 2;
  } 
 
  g_ucOutputStateALL += g_ucOutputState;
  ADC12_Convert(1);
  ValveRodSpeedNEW(5);
  OP.Expectation = Formula( AD.CurrentIn );   //
  
  if(CLK.OneKey_Flag ==1) //处于整定模式 20210201
  {
    ;
  }
  else //处于正常控制模式
  {
    //------------计算指令位置0-1000----------------------------------------------
    if(AD.CurrentIn < OP.Down_C) {g_PIDServiceData.SetPoint =0;}
    else if (AD.CurrentIn > OP.Up_C ){g_PIDServiceData.SetPoint =1000;}
    else {  g_PIDServiceData.SetPoint=  Formula( AD.CurrentIn );  }
    if(g_PIDServiceData.SetPoint > 1000)  {g_PIDServiceData.SetPoint =1000;}
    if( VAL.V_p2_4_ACT == V_REVR) {g_PIDServiceData.SetPoint = 1000- g_PIDServiceData.SetPoint; }
    
  }
  
  //------------计算反馈位置0-1000----------------------------------------------
  
  if((VAL.V_p1_1_RCR == V_LINE)&&(CLK.OneKey_Flag ==0))  //处于正常控制模式
  {     
      
    LineCalculate();     
    g_PIDServiceData.Cur_ValvePoint = (g_dsinAngel_now - g_dsinAngel_down)*1000.0/(g_dsinAngel_up - g_dsinAngel_down);
    
    
  }
  else
  {  
    if(AD.CurrentBack >OP.Down_Limited)
    {
      g_PIDServiceData.Cur_ValvePoint = (AD.CurrentBack -OP.Down_Limited)*1000.0/OP.Range_M;     //当前位置采样 采样值按0-1000来处理
    }
    else 
    {
      g_PIDServiceData.Cur_ValvePoint = (OP.Down_Limited -AD.CurrentBack)*1000.0/OP.Range_M;
      g_PIDServiceData.Cur_ValvePoint = -g_PIDServiceData.Cur_ValvePoint;
    }
  }   
  if( VAL.V_p3_3_RCT == V_CLOK) 
  {
    g_PIDServiceData.Cur_ValvePoint = 1000-g_PIDServiceData.Cur_ValvePoint;    
  }
  
  ///////////////////////////////////////////////////  
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
  g_PIDServiceData.Cur_Error=(g_PIDServiceData.SetPoint-g_PIDServiceData.Cur_ValvePoint);
  a=abs(g_PIDServiceData.Cur_Error); // 求绝对值
  
  
  KVS_Up = OP.Range_M*4/VAL.V_p2_6_TUP/(float)(VAL.V_p5_9__K9)/5; //求斜率
  KVS_Down = OP.Range_M*4/VAL.V_p2_7_TDN/(float)(VAL.V_p5_10_K0)/5;
  FastSectorUp = VAL.V_p5_9__K9;
  FastSectorDown = (-1)*(int)(VAL.V_p5_10_K0);
  
  K_VADJ = 1;//(float)OP.Range_M/800;  //调节系数
  
  
  if( VAL.V_p3_3_RCT == V_CLOK) 
  {
    FastSectorUp =  VAL.V_p5_10_K0;
    FastSectorDown = (-1)*(int)(VAL.V_p5_9__K9); 
  }

  VLX_UP = OP.Range_M*4/VAL.V_p2_6_TUP/5; //求平均速度
  VLX_DOWN = OP.Range_M*4/VAL.V_p2_7_TDN/5; //求平均速度
            
  VLX_DOWN = -1*VLX_DOWN;
  
  if(CLK.OneKey_Flag ==1) //处于整定模式 速关不起作用 20210304
  {
    g_ucFastcloseFlag = 0;
  }
  else //处于正常控制模式
  {
    //速关处理 20210227 百分比默认为10
    if(0 != VAL.V_p2_5_QOF)//---------速关控制-----
    {
      FCBoundary_Deal();       
    }
    else   //为0则不速关处理
    {
      g_ucFastcloseFlag = 0;
    }
  }
  if(0 != g_ucFastcloseFlag) //若为速关状态1 则不处理PID
  {
    ClearFlag(); //清零刹车或补气标志
    // refresh PID_service_data 
  }
  else if( 4 != RelayCounter) //非摩擦力整定模式
  {
    if((AD.g_vlow <= 2 )&&(AD.g_vlow >=-2))    //静止状态计数
    {
      if(((g_PIDServiceData.Cur_Error <= -6)||(g_PIDServiceData.Cur_Error >= 6))
         &&(1 == ucPIDstate))
      {
        ucwaite++;
        if((ucwaite > 50))
        {
          ucPIDstate = 0; //重新开始
          ucwaite = 0;
          ucGasCnt = 0;          
        }
      }      
      if(0 == ucPIDstate)
      {
        ucwaite = 0;
        if(g_PIDServiceData.Cur_Error > 6)
        {
          ucGasCnt++;          
          if(ucGasCnt  <= 2)
          {
            OutPut_Positive(950);//2*          
          }
          else if(ucGasCnt < (VAL.V_p6_3__ZO))///2
          {
            if(g_PIDServiceData.Cur_Error < 30)
            {
              OutPut_Positive(950 -(ucGasCnt*(950 - VAL.V_p5_3__K3)/(VAL.V_p6_3__ZO -1)));//2*
            }
            else
            {
              OutPut_Positive(950);
            }
          }
          else
          {

              ucPIDstate = 1; //可以引入PID控制   
              OutPut_Positive(0);            
              ucGasCnt = 0;
              ucbrk = 0;

          }
        }
        else if(g_PIDServiceData.Cur_Error < -6)
        {
          ucGasCnt++;          
          if(ucGasCnt  <= 2)
          {
            OutPut_Positive(950);//2*          
          }         
          else if(ucGasCnt < (VAL.V_p6_4__VR))
          {
            if(g_PIDServiceData.Cur_Error > -30)
            {
            OutPut_Negetive(950 - (ucGasCnt*(950 - VAL.V_p5_4__K4)/(VAL.V_p6_4__VR -1)));  
            }
            else
            {
              OutPut_Negetive(950);
            }                   
          }
          else 
          { 

              ucPIDstate = 1; //可以引入PID控制 
              OutPut_Positive(0);             
              ucGasCnt = 0;
              ucbrk = 0;

          }
        } 
        else
        {;}
      }      
    }
    else
    {
      ;      
    }
    if((g_vl >= 2 )||(g_vl <= -2 ))
    {
      if((g_vl > 2 )||(g_vl < -2 ))
      {
        ucwaite = 0;
      }
       
      if(0 == ucPIDstate)
      {
       ucPIDstate = 1; //可以引入PID控制 
       ucwaite = 0;
       ucGasCnt = 0;      
       ucbrk = 0;  
          if((g_PIDServiceData.Cur_Error > -5)&&(g_vl >= 2*K_VADJ))
          {
            OutPut_Negetive(950);
            g_ucOutputState =  -950;
          }
          else if((g_PIDServiceData.Cur_Error < 5)&&(g_vl <= -2*K_VADJ))
          {
            OutPut_Positive(950);  
            g_ucOutputState =  950;
          } 
          else
          {
            OutPut_Positive(0);
            g_ucOutputState =  0;
          }
          return;  
      }
    }
    
    if(0 == ucPIDstate)  //还没开始补气，不进行控制 控制相关状态清零
    {
      ClearFlag(); //清零刹车或补气标志      
      return;
    }
    if ((g_PIDServiceData.Cur_Error> FastSectorUp)||
        (g_PIDServiceData.Cur_Error< FastSectorDown))
    {
      flag = 0;
      
      ClearFlag(); //清零刹车或补气标志
      
      if((g_vl >VLX_UP)||(g_vl <VLX_DOWN))
      {
        OutPut_Positive(0);
        g_ucOutputState = 0;
      }
      else 
      {
        if( g_PIDServiceData.Cur_Error > 0) 
        { 
          CLK.Gasfilled ++;
          OutPut_Positive( VAL.V_p5_1__K1);
          g_ucOutputState =  VAL.V_p5_1__K1;
        }
        else 
        { 
          CLK.Outgassing ++;
          OutPut_Negetive( VAL.V_p5_2__K2);  
          g_ucOutputState =  -VAL.V_p5_2__K2;
        }
      }

    }
    
    else if((a> VAL.V_p1_3BRND)) //VAL.V_p1_3BRND
    {
      if(CLK.OneKey_Flag ==1) //处于整定模式 偏差补偿不起作用
      {
        enflag = 0;
      }
      else
      {
        if(a>20)
        {
          enflag = 0;
        }
        else
        {
          enflag = 1;      
        }
      }
      
      if(0 == flag1)
      {                
        //加速后刹车       
        if((0.5 <= flag4)&&((g_vl > 2*K_VADJ)||(g_vl < -2*K_VADJ)))
        {
          //OutPut_Positive(0);
          flag4 = 0;
          if((g_PIDServiceData.Cur_Error > 0)&&(g_vl > 3*K_VADJ))
          {
            OutPut_Negetive(950);
            g_ucOutputState =  -950;
          }
          else if((g_PIDServiceData.Cur_Error < 0)&&(g_vl < -3*K_VADJ))
          {
            OutPut_Positive(950);  
            g_ucOutputState =  950;
          } 
          else
          {
            OutPut_Positive(0);
            g_ucOutputState =  0;
          }
          return;
        }

        
        if(g_PIDServiceData.Cur_Error > VAL.V_p1_3BRND)
        {
             
          VLX = g_PIDServiceData.Cur_Error*(1+flag4)*KVS_Up;//            
          if(g_vl < VLX)
          {
            caloutput = (VLX-g_vl)*(g_PIDServiceData.Kp+enflag*a/VAL.V_p6_1__LK);
            if(caloutput > 600)
            {
              caloutput = 600;
            }
            if(caloutput < 0)
            {
              caloutput = 0;
            }
            OutPut_Positive(VAL.V_p5_3__K3+caloutput);
            g_ucOutputState =  VAL.V_p5_3__K3+caloutput;
            CLK.Gasfilled ++;                                    
          }
          else if(g_vl > VLX)
          {
            caloutput = (g_vl-VLX)*(g_PIDServiceData.Kp+enflag*a/VAL.V_p6_1__LK);
            if(caloutput > 600)
            {
              caloutput = 600;
            }
            if(caloutput < 0)
            {
              caloutput = 0;
            }               
            OutPut_Negetive(VAL.V_p5_4__K4+caloutput); 
            g_ucOutputState =  -(VAL.V_p5_4__K4+caloutput);                
            CLK.Outgassing ++;                
          }
          else
          {
            OutPut_Negetive(0); 
            g_ucOutputState =  0;                
          } 
        }
        else if(g_PIDServiceData.Cur_Error < -1*VAL.V_p1_3BRND)
        {
               
          VLX = g_PIDServiceData.Cur_Error*(1+flag4)*KVS_Down;
          if(g_vl < VLX)
          {
            caloutput = (VLX-g_vl)*(g_PIDServiceData.Ki+enflag*a/VAL.V_p6_1__LK);
            if(caloutput > 600)
            {
              caloutput = 600;
            }
            if(caloutput < 0)
            {
              caloutput = 0;
            }
            OutPut_Positive(VAL.V_p5_3__K3+caloutput);
            g_ucOutputState =  VAL.V_p5_3__K3+caloutput;                  
            CLK.Gasfilled ++;                                    
          }
          else if(g_vl > VLX)
          {
            caloutput = (g_vl-VLX)*(g_PIDServiceData.Ki+enflag*a/VAL.V_p6_1__LK);
            if(caloutput > 600)
            {
              caloutput = 600;
            }
            if(caloutput < 0)
            {
              caloutput = 0;
            }               
            OutPut_Negetive(VAL.V_p5_4__K4+caloutput);  
            g_ucOutputState =  -(VAL.V_p5_4__K4+caloutput);                  
          }
          else
          {
            OutPut_Negetive(0); 
            g_ucOutputState =  0;                
          }              
        }           
        else
        {
          OutPut_Negetive(0); 
          g_ucOutputState =  0;                
        }
      }
      else
      {
        ;
      }             
      if(VAL.V_p1_1_RCR == V_LINE)
      {
        //  Boundary_Deal();//放在这里边界不限位 ，快速到端点
      }
      else
      {
        Boundary_Deal();//放在这里边界不限位 ，快速到端点 ,角阀需要  
      } 
    }
    else  
    {                  // valve position in Dead_Section   

      CLK.CurrentInRef =0;  //调整到位清指令变化标至
      flag1 = 0; //  0为采用斜率控制标志 
      flag4 = 0; //斜率翻倍系数清零
      SecGas =0;   // 静止计数清零
      SecOut =0;  //  静止计数清零     
      flag2 = 0;  //0为不进行补气控制标志 
      flag3 = 0;  //0为不进行补气控制标志  
      g_ucUpCnt = 0;  //补气计数增加清零
      g_ucDownCnt = 0;  //补气计增加数清零 
      //      }      
      g_PIDServiceData.SumDeltaU=0;  //160118add
      if(g_PIDServiceData.SetPoint>995 || g_PIDServiceData.SetPoint<5 ) 
      {
        Boundary_Deal();   //边界处理
      }
      else
      {
        // OutPut_Positive(0); 
        //到区间还有速度，则应刹车处理，仅1次     记录状态            
        if(((AD.g_vlow2 > 4)||(g_vl > 2*K_VADJ))&&(0 == ucBrkflag))//(g_PIDServiceData.Cur_Error > 0)&&
        {
          ucBrkCnt++;
          if(ucBrkCnt >= 1)
          {
            OutPut_Negetive(VAL.V_p5_2__K2);
            g_ucOutputState =  -VAL.V_p5_2__K2;                     
            ucBrkflag = 2; //刹车状态下次进来要清零
            ucBrkCnt = 0;
            return;             
          }
        }
        else if(((AD.g_vlow2 < -4)||(g_vl < -2*K_VADJ))&&(0 == ucBrkflag))//(g_PIDServiceData.Cur_Error < 0)&&
        {                
          ucBrkCnt++;
          if(ucBrkCnt >= 1)
          {
            OutPut_Positive(VAL.V_p5_1__K1);
            g_ucOutputState =  VAL.V_p5_1__K1;                    
            ucBrkflag = 1;   //下次进来要清零                
            ucBrkCnt = 0;
            return;             
          }                               
        }                                  
        else
        { 
          OutPut_Positive(0); 
          g_ucOutputState =  0;                  
          ucBrkflag = 0; 
          ucBrkCnt = 0;
        }         
        
      }
      
    }
    
    if(CLK.OneKey_Flag !=1)  //不处于整定模式 补气才起作用
    {
      if((AD.g_vlow <=1+1*K_VADJ )&&(AD.g_vlow >=-1-1*K_VADJ))   
      {
        if(SecGas < 6000)
        {
          SecGas ++;
        }
        if(SecOut > 0)
        {
          SecOut --;
        }            
        if((SecGas >= 6)&&(0 == flag1))
        { 
          SecGas = 0;
          flag1 = 1;

          {
            flag1 = 0; 
            if(((0 == VAL.V_p6_2__SV)&&(a >  VAL.V_p1_3BRND))
           ||((2 == VAL.V_p6_2__SV)&&(a >  20)))  //从1改成2避免与输入超限报警冲突 20220623
            {
              flag4 += 0.5;
              if(flag4 > 3)
              {
                flag4 = 3; 
              }
              
            }
            
          }
        }
      } 
      else
      {      
        if(SecOut < 6000)
        {
          SecOut ++;
        }
        if(SecGas > 0)
        {
          SecGas --;
        }
      }
    }
    
    

    if(0 !=ucBrkflag) //代表在死区进行过刹车，需要恢复输出为0
    {
      OutPut_Positive(0); 
      g_ucOutputState =  0;                  
      ucBrkflag = 0; 
      ucBrkCnt = 0;    
    }
    else
    {
      ;
    }
    
  }

  
}



