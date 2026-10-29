#include "Device.h"
//------------------------------------------------------------------------------
/*       HSYCData HSYC;
float f;
float a11,b1,q;
float CH,DH,EH,FH;
float b[5],c[5],d[5],e[5];
float ah[6]={65,64,63,62,61,60};

unsigned int SpeedV0;   //过短速度
unsigned int SpeedVx;   //启动电流得出的速度

float s_h,s_l;   

int ahui;
//float a[5]={64,64,64,64,64};
//extern float ah[6]={64,64,64,64,64,64};
*/
unsigned char flag = 0,flag1 = 0,  flag2 =0,  flag3 =0; 
float flag4 =0; //K值翻倍
unsigned char ucBrkflag = 0,ucBrkCnt = 0;//,ucBrkflagF = 0,ucBrkCntF = 0,ucBrkflagB = 0,ucBrkCntB = 0;
unsigned int SecGas =0,  SecOut =0;  //充气 排气自增量 自适应相关参数
int CurError_TimeStepL;     //时间间隔上一次的偏差值

int g_vl = 0;
int gastime = 0, Outtime = 0;
unsigned int pwm_up = 0, pwm_down = 0;
unsigned char g_ucStateP = 0, g_ucStateV = 0;
unsigned char e3flag = 0,e4flag = 0;
unsigned char uptime = 0, downtime = 0;
float KVS_Up = 0.5,KVS_Down = 0.5;// 0.25
float VLX =0 ,VLX_UP = 0,VLX_DOWN = 0;
int caloutput = 0;
int FastSectorUp = 50,FastSectorDown = 50;
float K_VADJ = 1.0; //速度补偿

unsigned char g_ucFastcloseFlag = 0; 

unsigned char g_ucUpPOSduty = 3;  //正向补气时间
unsigned char g_ucUpNEGduty = 1;  //正向刹车时间
unsigned char g_ucDownPOSduty = 3;   //反向补气时间
unsigned char g_ucDownNEGduty = 1;   //反向刹车时间

unsigned char g_ucUpCnt =0;      //补气次数
unsigned char g_ucDownCnt =0;      //补气次数

unsigned char g_ucBrkState = 0;  //刹车状态
int  g_ucOutputState = 0;  //气量状态
long  g_ucOutputStateALL = 0;  //总气量状态

extern double g_dangel_up = 0.0,g_dangel_down = 0.0,g_dangel_now = 0.0;
extern double g_dsinAngel_up = 0.0,g_dsinAngel_down = 0.0,g_dsinAngel_now = 0.0;

//超调检测
int g_SetPointLast = 0;
unsigned int  g_usSetPointCnt = 0;
unsigned int    Cur_ErrorPOSCnt = 0;
unsigned int    Cur_ErrorNEGCnt = 0;
unsigned int    Cur_ErrorZEROCnt = 0;
unsigned char    Cur_ErrorZEROFlag = 0;
unsigned char    Cur_ErrorPOSFlag = 0;
unsigned char    Cur_ErrorNEGFlag = 0;
unsigned int     ZERO2POSCnt = 0;
unsigned int     ZERO2NEGCnt = 0;
unsigned int     uMAXCnt = 0;
   
int     MinGasFlag = 0;  
int     MinOutFlag = 0; 
unsigned int    uwerrcnt = 0;
unsigned char   WDogflag = 0;
//清零刹车或补气标志
void ClearFlag(void)
{
  ucBrkflag = 0; //死区刹车标志清零
  ucBrkCnt = 0; //死区刹车清零 
  flag1 = 0; //  0为采用斜率控制标志 
  flag4 = 0; //斜率翻倍系数清零
  SecGas =0;   // 静止计数清零
  SecOut =0;  //  静止计数清零     
  flag2 = 0;  //0为不进行补气控制标志 
  flag3 = 0;  //0为不进行补气控制标志  
  g_ucUpCnt = 0;  //补气计数增加清零
  g_ucDownCnt = 0;  //补气计增加数清零  
}

//---------------------------速关处理----------------------//
//----------------------------------------------------------//
void FCBoundary_Deal(void)
{
  if((VAL.V_p1_1_RCR&0x01) == V_LINE)
  {
    if(((g_PIDServiceData.SetPoint>996-VAL.V_p2_5_QOF)&&(VAL.V_p3_3_RCT == V_CTCL)&&( VAL.V_p3_2_MAX !=OP.Up_Limited))
       ||((g_PIDServiceData.SetPoint>996-VAL.V_p2_5_QOF)&&(VAL.V_p3_3_RCT != V_CTCL)&&(VAL.V_p3_1_MIN !=OP.Down_Limited))
         )
    {       
      OutPut_Positive( 950 ); 
      g_ucOutputState = 950;
      g_ucFastcloseFlag = 1;
    }        
    else if(((g_PIDServiceData.SetPoint < VAL.V_p2_5_QOF+4)&&(VAL.V_p3_3_RCT == V_CTCL)&&( VAL.V_p3_1_MIN !=OP.Down_Limited))
            ||((g_PIDServiceData.SetPoint < VAL.V_p2_5_QOF+4)&&(VAL.V_p3_3_RCT != V_CTCL)&&(VAL.V_p3_2_MAX !=OP.Up_Limited))
              )
    {       
      OutPut_Negetive( 950 );
      g_ucOutputState = -950;
      g_ucFastcloseFlag = 2;
    } 
    else
    {
      g_ucFastcloseFlag = 0;
    }
  }
  else  //球阀  不判断限位
  {
    if(((g_PIDServiceData.SetPoint>996-VAL.V_p2_5_QOF)&&(VAL.V_p3_3_RCT == V_CTCL))
       ||((g_PIDServiceData.SetPoint >996-VAL.V_p2_5_QOF)&&(VAL.V_p3_3_RCT != V_CTCL))
         )
    {       
      OutPut_Positive( 950 );  
      g_ucOutputState = 950;
      g_ucFastcloseFlag = 1;
    }        
    else if(((g_PIDServiceData.SetPoint < VAL.V_p2_5_QOF+4)&&(VAL.V_p3_3_RCT == V_CTCL))
            ||((g_PIDServiceData.SetPoint  < VAL.V_p2_5_QOF+4)&&(VAL.V_p3_3_RCT != V_CTCL))
              )
    {       
      OutPut_Negetive( 950 );
      g_ucOutputState = -950;          
      g_ucFastcloseFlag = 2;
    } 
    else
    {
      g_ucFastcloseFlag = 0;
    }   
  }
  
  if(2 == g_ucFastcloseFlag) //设置到端点
  {
    ;// g_PIDServiceData.SetPoint = 1000;           
    
  }
  else if(1 == g_ucFastcloseFlag)
  {
    ;// g_PIDServiceData.SetPoint = 0;           
  }
  else
  {
    ;
  }
} 
//-----------------------------------------------------------------------------
//恢复默认输出
//-----------------------------------------------------------------------------
void huifumoren(void)
{  
  if(MinGasFlag != 0)
  {
    MinGasFlag = 0;//恢复参数                    
  }
  
  if(MinOutFlag != 0)
  {
    MinOutFlag = 0;//恢复参数                    
  }
}

//------------------------------------------------------------------------------
//修改5.9及5.10 刹车速率
//------------------------------------------------------------------------------
void  changeP(void)
{
  static unsigned int   usfdbPointCnt = 0;
  static int   ssCur_Errorlast = 0;
  if(((g_PIDServiceData.SetPoint-2)<= g_SetPointLast)&&(g_SetPointLast <= (g_PIDServiceData.SetPoint+2)))
  {
    g_usSetPointCnt++;//代表设定值没有变化计数
  }
  else  //设定值发生了改变
  {
    g_usSetPointCnt = 0;
    
    Cur_ErrorPOSCnt = 0;
    Cur_ErrorNEGCnt = 0;
    Cur_ErrorZEROCnt = 0;
    Cur_ErrorZEROFlag = 0;
    Cur_ErrorPOSFlag = 0;
    Cur_ErrorNEGFlag = 0;
    ZERO2POSCnt = 0;
    ZERO2NEGCnt = 0;   
  }
  g_SetPointLast = g_PIDServiceData.SetPoint; //20210505 记录上一次的设定值
  
  
  if(g_usSetPointCnt > 25*1) //设定值0.5s没有变化且反馈首次进入了设定区间
  {
 
    if((g_PIDServiceData.Cur_Error < VAL.V_p1_3BRND)&&(g_PIDServiceData.Cur_Error > -VAL.V_p1_3BRND))
    {
      Cur_ErrorZEROCnt++;   //进入区间标志
      Cur_ErrorZEROFlag = 1;  //进入区间标志
      if(ZERO2POSCnt+ ZERO2NEGCnt > 6)
      {          
          ZERO2POSCnt = 0;
          ZERO2NEGCnt = 0;          
      }

    }

    if((g_PIDServiceData.Cur_Error > VAL.V_p1_3BRND)&&(Cur_ErrorZEROCnt > 0))
    {     
      Cur_ErrorPOSCnt++; //代表处于震荡状态计数  
      if(1 == Cur_ErrorZEROFlag)
      {
        ZERO2POSCnt++; //正超调次数
        if(ZERO2POSCnt >= 2)       
        {
                 if((VAL.V_p5_3__K3 + 20*MinGasFlag)> 280)
                {
                  MinGasFlag--;
                } 
        }
        Cur_ErrorZEROFlag = 0; //本次超调标志
        Cur_ErrorPOSFlag = 1; // 正超调标志       
      }      
    }
    else if((g_PIDServiceData.Cur_Error < -VAL.V_p1_3BRND)&&(Cur_ErrorZEROCnt > 0))
    {
      Cur_ErrorNEGCnt++; //代表处于震荡状态计数 
      if(1 == Cur_ErrorZEROFlag)
      { 
        ZERO2NEGCnt++; //负超调次数 
        if(ZERO2NEGCnt >= 2)
        {
                 if((VAL.V_p5_4__K4 + 20*MinOutFlag) > 280)
                {
                  MinOutFlag--;
                }
        }        
        Cur_ErrorZEROFlag = 0; //本次超调标志
        Cur_ErrorNEGFlag = 1;   // 负超调标志          
      }
         
    }
    else 
    {
      ;
    }
  }
}

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
  
  static unsigned long wPSDcnt = 0;//2155680000;  //计时
  static int  ssSetPoint = 0;
  static int  statePSD = 0;  
  
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

    if(VAL.V_p6_2__SV >0)
    {
      wPSDcnt++;

      if(wPSDcnt >(2160000*(VAL.V_p6_2__SV -1)+ 15000))//处于PSD模式60*60*50=1h*12
      {
        wPSDcnt = 0;
        statePSD = 0;
      }      
    }
    else
    {
      statePSD = 3; //不检测psd
    }
    switch(statePSD)
    {
    case 0:
      {
        ssSetPoint = g_PIDServiceData.SetPoint;               
        statePSD = 1;
//        VAL.V_p6_1__LK = 0;//重置状态
      }break;
    case 1:
      {
        g_PIDServiceData.SetPoint = ssSetPoint + VAL.V_p6_3__ZO*10;
        if(g_PIDServiceData.SetPoint > 1000)
        {
          g_PIDServiceData.SetPoint =1000;
        }        
        if(((ssSetPoint + VAL.V_p6_3__ZO*8) < g_PIDServiceData.Cur_ValvePoint )
           ||(g_PIDServiceData.Cur_ValvePoint >= 995))
        {                 
          statePSD = 2; 
          VAL.V_p6_1__LK = 0;//重置状态
        } 
        if(wPSDcnt > 3000 ) //1min
        {
          statePSD = 2;
          VAL.V_p6_1__LK = 1;//psd err
        }
      }break;
    case 2:
      {
        g_PIDServiceData.SetPoint = ssSetPoint - VAL.V_p6_3__ZO*10;        
        if(g_PIDServiceData.SetPoint < 0)
        {
          g_PIDServiceData.SetPoint = 0 ;
        }         
        if(((ssSetPoint - VAL.V_p6_3__ZO*8) > g_PIDServiceData.Cur_ValvePoint )
           ||(g_PIDServiceData.Cur_ValvePoint <= 5))
        {
          g_PIDServiceData.SetPoint = ssSetPoint; 
          statePSD = 3;
          VAL.V_p6_1__LK = 0;//重置状态
        }      
        if(wPSDcnt > 6000 ) //1min
        {
          g_PIDServiceData.SetPoint = ssSetPoint;           
          statePSD = 3;          
          VAL.V_p6_1__LK = 1;//psd err
        }        
      }break;
    case 3:
      {
        
      }break; 
    default : break;
    }        
  }
  
  //------------计算反馈位置0-1000----------------------------------------------
  
  if(((VAL.V_p1_1_RCR&0x01) == V_LINE)&&(CLK.OneKey_Flag ==0)) //处于正常控制模式
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
          else if(ucGasCnt < (VAL.V_p6_6_MIN))///2
          {
            if(g_PIDServiceData.Cur_Error < 30)
            {
              OutPut_Positive(950 -(ucGasCnt*(950 - VAL.V_p5_3__K3)/(VAL.V_p6_6_MIN -1)));//2*
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
          else if(ucGasCnt < (VAL.V_p6_7_MAX))
          {
            if(g_PIDServiceData.Cur_Error > -30)
            {
            OutPut_Negetive(950 - (ucGasCnt*(950 - VAL.V_p5_4__K4)/(VAL.V_p6_7_MAX -1)));  
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
            caloutput = (VLX-g_vl)*(g_PIDServiceData.Kp+enflag*a/VAL.V_p6_5__TO);
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
            caloutput = (g_vl-VLX)*(g_PIDServiceData.Kp+enflag*a/VAL.V_p6_5__TO);
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
            caloutput = (VLX-g_vl)*(g_PIDServiceData.Ki+enflag*a/VAL.V_p6_5__TO);
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
            caloutput = (g_vl-VLX)*(g_PIDServiceData.Ki+enflag*a/VAL.V_p6_5__TO);
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
      if((VAL.V_p1_1_RCR&0x01) == V_LINE)
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
            if((1 == VAL.V_p6_4__VR)&&(a >  20))  //从1改成2避免与输入超限报警冲突 20220623
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




///*************************************************
//*		函数名：PIDcotrol
//*		说  明：PID控制程序									
//*		日  期：2015.6.14
//*************************************************/
void PIDcontrol(void)
{
  int a,b;
  unsigned char enflag = 0;  //2%以内使能补气增加偏移
  //  unsigned int e;
  //  unsigned long c;
  //  long d;
  static unsigned long wPSDcnt = 0;//2155680000;  //计时
  static int  ssSetPoint = 0;
  static int  statePSD = 0;
  
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
    
    if(VAL.V_p6_2__SV >0)
    {
      wPSDcnt++;

      if(wPSDcnt >(2160000*(VAL.V_p6_2__SV -1)+ 15000))//处于PSD模式60*60*50=1h*12
      {
        wPSDcnt = 0;
        statePSD = 0;
      }      
    }
    else
    {
      statePSD = 3; //不检测psd
    }
    switch(statePSD)
    {
    case 0:
      {
        ssSetPoint = g_PIDServiceData.SetPoint;               
        statePSD = 1;
//        VAL.V_p6_1__LK = 0;//重置状态
      }break;
    case 1:
      {
        g_PIDServiceData.SetPoint = ssSetPoint + VAL.V_p6_3__ZO*10;
        if(g_PIDServiceData.SetPoint > 1000)
        {
          g_PIDServiceData.SetPoint =1000;
        }        
        if(((ssSetPoint + VAL.V_p6_3__ZO*8) < g_PIDServiceData.Cur_ValvePoint )
           ||(g_PIDServiceData.Cur_ValvePoint >= 995))
        {                 
          statePSD = 2; 
          VAL.V_p6_1__LK = 0;//重置状态
        } 
        if(wPSDcnt > 3000 ) //1min
        {
          statePSD = 2;
          VAL.V_p6_1__LK = 1;//psd err
        }
      }break;
    case 2:
      {
        g_PIDServiceData.SetPoint = ssSetPoint - VAL.V_p6_3__ZO*10;        
        if(g_PIDServiceData.SetPoint < 0)
        {
          g_PIDServiceData.SetPoint = 0 ;
        }         
        if(((ssSetPoint - VAL.V_p6_3__ZO*8) > g_PIDServiceData.Cur_ValvePoint )
           ||(g_PIDServiceData.Cur_ValvePoint <= 5))
        {
          g_PIDServiceData.SetPoint = ssSetPoint; 
          statePSD = 3;
          VAL.V_p6_1__LK = 0;//重置状态
        }      
        if(wPSDcnt > 6000 ) //1min
        {
          g_PIDServiceData.SetPoint = ssSetPoint;           
          statePSD = 3;          
          VAL.V_p6_1__LK = 1;//psd err
        }        
      }break;
    case 3:
      {
        
      }break; 
    default :;
    }    
    
  }
  
  //------------计算反馈位置0-1000----------------------------------------------
  if(((VAL.V_p1_1_RCR&0x01) == V_LINE)&&(CLK.OneKey_Flag ==0))  //处于正常控制模式
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

       if(VAL.V_p2_6_TUP > 100)
      {
         VAL.V_p2_6_TUP = 100;
      }
      if(VAL.V_p2_7_TDN > 100)
      {
        VAL.V_p2_7_TDN = 100;     
      } 
  
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
  else 
  {
    if(CLK.OneKey_Flag ==1) //处于整定模式 速关不起作用 20210304
    {
     ;
    }
    else //处于正常控制模式
    {    
      changeP();   //根据超调情况修改刹车速率20200505 
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
            caloutput = (VLX-g_vl)*(g_PIDServiceData.Kp+enflag*a/10);
            if(caloutput > 600)
            {
              caloutput = 600;
            }
            if(caloutput < 0)
            {
              caloutput = 0;
            }
            OutPut_Positive(VAL.V_p5_3__K3+ 20*MinGasFlag+caloutput);
            g_ucOutputState =  VAL.V_p5_3__K3+ 20*MinGasFlag+caloutput;
            CLK.Gasfilled ++;                                    
          }
          else if(g_vl > VLX)
          {
            caloutput = (g_vl-VLX)*(g_PIDServiceData.Kp+enflag*a/10);
            if(caloutput > 600)
            {
              caloutput = 600;
            }
            if(caloutput < 0)
            {
              caloutput = 0;
            }               
            OutPut_Negetive(VAL.V_p5_4__K4+ 20*MinOutFlag+caloutput); 
            g_ucOutputState =  -(VAL.V_p5_4__K4+ 20*MinOutFlag+caloutput);                
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
            caloutput = (VLX-g_vl)*(g_PIDServiceData.Ki+enflag*a/10);
            if(caloutput > 600)
            {
              caloutput = 600;
            }
            if(caloutput < 0)
            {
              caloutput = 0;
            }
            OutPut_Positive(VAL.V_p5_3__K3+ 20*MinGasFlag+caloutput);
            g_ucOutputState =  VAL.V_p5_3__K3+ 20*MinGasFlag+caloutput;                  
            CLK.Gasfilled ++;                                    
          }
          else if(g_vl > VLX)
          {
            caloutput = (g_vl-VLX)*(g_PIDServiceData.Ki+enflag*a/10);
            if(caloutput > 600)
            {
              caloutput = 600;
            }
            if(caloutput < 0)
            {
              caloutput = 0;
            }               
            OutPut_Negetive(VAL.V_p5_4__K4+ 20*MinOutFlag+caloutput);  
            g_ucOutputState =  -(VAL.V_p5_4__K4+ 20*MinOutFlag+caloutput);                  
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
      if((VAL.V_p1_1_RCR&0x01) == V_LINE)
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
        
      g_PIDServiceData.SumDeltaU=0;  //160118add
      if(g_PIDServiceData.SetPoint>995 || g_PIDServiceData.SetPoint<5 ) 
      {
        Boundary_Deal();   //边界处理
      }
      else
      {
        // OutPut_Positive(0); 
        //到区间还有速度，则应刹车处理，仅1次     记录状态            
        if(((AD.g_vlow > 4)||(g_vl > 2*K_VADJ))&&(0 == ucBrkflag))//(g_PIDServiceData.Cur_Error > 0)&&
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
        else if(((AD.g_vlow < -4)||(g_vl < -2*K_VADJ))&&(0 == ucBrkflag))//(g_PIDServiceData.Cur_Error < 0)&&
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
      if(a > 100)
      {
        if(++uwerrcnt  > 5000)//100s
        {
          uwerrcnt = 0;
          WDogflag = 1;
          //stop WD
        }          
      }
      else
      {
        uwerrcnt = 0;
        WDogflag = 0;        
      }
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
        if((SecGas >= 25)&&(0 == flag1))
        { 
          SecGas = 0;
          flag1 = 1;
          
          {
            flag1 = 0; 
            if((g_PIDServiceData.Cur_Error < -1*VAL.V_p1_3BRND)||(g_PIDServiceData.Cur_Error >  VAL.V_p1_3BRND))
            {
              flag4 += 0.5;
              if(flag4 > 10)
              {
                flag4 = 10; 
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
    
    
    if((1 == flag2) )
    { 
      e3flag = 1;// 补气  
      uptime = 0;
      
      if(1 == e3flag)
      {                 
        pwm_up++;
        if((g_ucUpPOSduty+g_ucUpNEGduty) < pwm_up)
        {
          g_ucOutputState =  0;                    
          OutPut_Negetive( 0 );
          e3flag = 0;// 补气
          pwm_up = 0; 
          flag2 = 0;
          flag1 = 0;
        }
        else if(((g_ucUpNEGduty+g_ucUpPOSduty) >= pwm_up)&&(g_ucUpPOSduty < pwm_up))
        {
          g_ucOutputState =  -VAL.V_p5_2__K2;                    
          OutPut_Negetive( VAL.V_p5_2__K2 );  
          CLK.Outgassing ++;                     
        }
        else if(pwm_up <= g_ucUpPOSduty)
        {
          g_ucOutputState =  VAL.V_p5_1__K1;                       
          CLK.Gasfilled ++;                  
          OutPut_Positive( VAL.V_p5_1__K1 ); //470
        }
        else
        {                  
          ;
        }
        
      }                         
    }
    else if((1 == flag3) )
    { 
      e4flag = 1;// 补气
      downtime = 0;
      
      if(1 == e4flag)
      {                 
        pwm_down++;
        
        if((g_ucDownPOSduty+g_ucDownNEGduty) < pwm_down)
        {
          g_ucOutputState =  0;                     
          OutPut_Positive( 0 );
          e4flag = 0;// 补气
          pwm_down = 0;  
          flag3 = 0;
          flag1 = 0;  //处于补气状态，斜率不介入
        }
        else if(((g_ucDownPOSduty+g_ucDownNEGduty) >= pwm_down)&&(g_ucDownPOSduty < pwm_down))
        {
          g_ucOutputState =  VAL.V_p5_1__K1;                     
          OutPut_Positive( VAL.V_p5_1__K1 );  
          CLK.Gasfilled ++;                   
        }
        else if(pwm_down <= g_ucDownPOSduty)
        {
          g_ucOutputState =  -VAL.V_p5_2__K2;                     
          CLK.Outgassing ++;                  
          OutPut_Negetive( VAL.V_p5_2__K2 ); //470
        }
        else
        {                  
          ;
        }
        
      }             
    }
    else if(0 !=ucBrkflag) //代表在死区进行过刹车，需要恢复输出为0
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




int addtestdata = 11;
unsigned jtestdata = 0;
void ValveRodSpeedNEW(unsigned char state)
{
  unsigned int i;
  unsigned Num = 10;
  
  if(5 == state)
  {
    Num = 10;
  }
  else
  {
    Num = 10;    
  }   
  AD.pretime++;
  if(Num == AD.pretime)
  {
    AD.firsttime = 1; //10次时间达到
    AD.pretime =0;
  }       
  for(i=1;i<Num;i++)
  {
    AD.Pos[i-1] = AD.Pos[i]; 
    AD.Speed[i-1] = AD.Speed[i]; 
    AD.Acc[i-1] = AD.Acc[i];          
  }
  AD.Pos[Num-1] = AD.CurrentBackRel;
  AD.Speed[Num-1] = AD.Pos[Num-1]- AD.Pos[Num-2];
  AD.Acc[Num-1]= AD.Speed[Num-1]- AD.Speed[Num-2];
  if(1 == AD.firsttime)
  {
    //g_vl = AD.Speed[9];
    if(0 == state)//  求上行最大速度及位置
    {
      if(OP.Meterage_L >=  OP.Down_Limited + OP.Range_M/4)
      {
        if(AD.Speed[Num-1] > AD.maxSpeedUp )
        {      
          AD.maxSpeedUpPos = OP.Meterage_L;//AD.Pos[Num-1];
          AD.maxSpeedUp = AD.Speed[Num-1];
        }
        else
        {
          AD.maxSpeedUpPos =  AD.maxSpeedUpPos;//AD.Pos[Num-2];
          AD.maxSpeedUp = AD.maxSpeedUp;     
        }
      }
      if( AD.maxSpeedUpPos > OP.Up_Limited - OP.Range_M/3)
      {
        AD.maxSpeedUpPos = OP.Up_Limited - OP.Range_M/3;
      }
      else
      {
        ;
      }
      
      
    }
    else if(1 == state) //  求下行最大速度及位置
    {
      if(OP.Meterage_L <=  OP.Up_Limited - OP.Range_M/4)
      {
        if(AD.Speed[Num-1] < AD.maxSpeedDown )
        {      
          AD.maxSpeedDownPos = OP.Meterage_L;//AD.Pos[Num-1];
          AD.maxSpeedDown = AD.Speed[Num-1];
        }
        else
        {
          AD.maxSpeedDownPos = AD.maxSpeedDownPos;//AD.Pos[Num-2];
          AD.maxSpeedDown = AD.maxSpeedDown;     
        }
      }
      if( AD.maxSpeedDownPos < OP.Down_Limited + OP.Range_M/3)
      {
        AD.maxSpeedDownPos = OP.Down_Limited + OP.Range_M/3;
      }
      else
      {
        ;
      }      
      
    }
    
    else if(5 == state)
    {
      g_vl = AD.Speed[Num-1];//+ AD.Speed[Num-2];
      //AD.g_vlow = AD.Speed[Num-1]+ AD.Speed[Num-2]+AD.Speed[Num-3]+ AD.Speed[Num-4]; //低速测量间隔需要大一些
      
      AD.g_vlow = AD.CurrentBackRel - AD.Pos[1];
      AD.g_vlow2 = AD.CurrentBackRel - AD.Pos[4];
      if(VAL.V_p3_3_RCT == V_CLOK)             
      {
        g_vl = -1*g_vl;
        AD.g_vlow = -1*AD.g_vlow;
        AD.g_vlow2 = -1*AD.g_vlow2;
      }
      
    }
    
  } 
  if(2 == state) //找上行超调位移
  {
    AD.maxDletaUp = OP.Meterage_L - AD.maxSpeedUpPos;
  }
  else if(3 == state)//找下行超调位移
  {
    AD.maxDletaDown = OP.Meterage_L - AD.maxSpeedDownPos;
  } 
  else
  {
    ;
  }
  //AD.Acc[Num-1] = g_PIDServiceData.Cur_ValvePoint;//AD.Speed[99]-AD.Speed[98]; 
}    

//----------------------------------------------------------//信号源
//---------------------------边界处理-----------------------//
//----------------------------------------------------------//
void Boundary_Deal (void)
{
  if( g_PIDServiceData.SetPoint>1000-VAL.V_p1_3BRND )// && ( VAL.V_p3_2_MAX ==OP.Up_Limited))
  {
    if(VAL.V_p3_3_RCT == V_CTCL)
    {
      if((VAL.V_p3_2_MAX == OP.Up_Limited))//&&(g_PIDServiceData.Cur_ValvePoint >= 999))
      {
        OutPut_Positive( 0 );// _NOP();
      }
      else
      {         
        if(VAL.V_p2_4_ACT == V_REVR) //风关型  VAL.V_p3_3_RCT == V_CLOK
        {
          OutPut_Positive( 950 ); //OutPut_Negetive( 900 );
        }
        else   //风开
        {
          OutPut_Positive( 950 );
        }
      }
    }
    else  //add
    {
      if((VAL.V_p3_1_MIN ==OP.Down_Limited))//&&(g_PIDServiceData.Cur_ValvePoint >= 999))
      {
        OutPut_Positive( 0 );// _NOP();
      }
      else
      {
        if(VAL.V_p2_4_ACT == V_REVR) //风关型
        {
          OutPut_Positive( 950 ); //OutPut_Positive( 900 );
        }
        else   //风开型
        {
          OutPut_Positive( 950 );
        } 
      }          
    }
    
  }
  if(( g_PIDServiceData.SetPoint < VAL.V_p1_3BRND+1 ))// && (VAL.V_p3_1_MIN ==OP.Down_Limited))
  {        
    if(VAL.V_p3_3_RCT == V_CTCL)
    {
      if((VAL.V_p3_1_MIN ==OP.Down_Limited))//&&(g_PIDServiceData.Cur_ValvePoint <= 1))
      {
        OutPut_Positive( 0 );// _NOP();
      }
      else
      {
        if(VAL.V_p2_4_ACT == V_REVR) //风关型
        {
          OutPut_Negetive( 950 ); //OutPut_Positive( 900 );
        }
        else   //风开型
        {
          OutPut_Negetive( 950 );
        } 
      }
    }
    else   //add
    {
      if(( VAL.V_p3_2_MAX ==OP.Up_Limited))//&&(g_PIDServiceData.Cur_ValvePoint <= 1))
      {
        OutPut_Positive( 0 );// _NOP();
      }
      else
      {         
        if(VAL.V_p2_4_ACT == V_REVR) //风关型  VAL.V_p3_3_RCT == V_CLOK
        {
          OutPut_Negetive( 950 );//OutPut_Positive( 800 ); //OutPut_Negetive( 900 );
        }
        else   //风开
        {
          OutPut_Negetive( 950 );// OutPut_Positive( 800 );
        }
      }                                    
    }
  } 
  
}


//------------------------------------------------------------------------------
//执行器泄露检测   20160701
//------------------------------------------------------------------------------
void Lesk_Detection (void)
{
  
}


