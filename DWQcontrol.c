#include "Device.h"

//control.c

/*************************************************
*		函数名：PIDcotrol
*		说  明：PID控制程序									
*		日  期：2015.6.14
*************************************************/
void PIDcontrol(void)
{
  int a,b;
  unsigned int e;
  unsigned long c;
  long d;
  g_PIDServiceData.Cur_Error=(g_PIDServiceData.SetPoint-g_PIDServiceData.Cur_ValvePoint);
  a=abs(g_PIDServiceData.Cur_Error); 
  if (a>g_DeviceFeature.FastSector_Lower) 
  {   
     if( g_PIDServiceData.Cur_Error > 0) 
     {    // below: fast adjust valve position
          //将 g_PWM_Duty.FastGas_Filled 送CCRX寄存器；
          // 置充气方式；
          //启动PWM输出；
       OutPut_Positive( g_PWM_Duty.Fast_GasFilled );
     }
     else 
     {
       // 将 g_PWM_Duty.FastGas_Decreaseded 送CCRX寄存器；
       // 置放气方式；
       // 启动PWM输出；
       OutPut_Negetive( g_PWM_Duty.Fast_Outgassing ); 
     }
     refreshdata();                   // refresh PID_service_data
     g_PIDSeerviceData.SumDeltaU=0;
     return;
   
  }
   if (a>g_DeviceFeature.SlowSector_Lower) 
   {   
                       // below: slow adjust valve position
     e=ValveRodSpeed();                   // calculate valve_rod_speed                
     if( e>g_DeviceFeature.ValveRodSpeed1) 
        {
          if (g_PIDServiceData.Cur_Error > 0 )
          {
             // 将 g_PWM_Duty.SlowGas_Decreaseded  送CCRX寄存器；
             // 置放气方式;
             //启动PWM输出；
             OutPut_Positive( g_PWM_Duty.Slow_Outgassing );
           }
          else 
          {
             // 将 g_PWM_Duty.SlowGas_Filled 送CCRX寄存器；
             // 置充气方式；
             // 启动PWM输出；
             OutPut_Negetive( g_PWM_Duty.Slow_Gasfilled ); 
          }
          refreshdata();                // refresh PID_service_data
          g_PIDServiceData.SumDeltaU=0;
          return;
        }
     else 
     {
        if (g_PIDServiceData.Cur_Error > 0) 
        {
           // 将 g_PWM_Duty.SlowGas_Fiilled 送CCRX寄存器；
           // 置充气方式;
           // 启动PWM输出；
          OutPut_Positive( g_PWM_Duty.Slow_Gasfilled );
        }
        else 
        {
           // 将 g_PWM_Duty.SlowGas_Decreased 送CCRX寄存器；
           // 置放气方式；
           // 启动PWM输出；
          OutPut_Negetive( g_PWM_Duty.Slow_Outgassing); 
        }
        refreshdata();                // refresh PID_service_data
        g_PIDSeerviceData.SumDeltaU=0;
        return;
     }
  }
   if (a>g_DeviceFeature.DeadSection) 
   {               
                          // below: weak adjust valve position
     e=ValveRodSpeed();                   // calculate valve_rod_speed      
     if (e>g_DeviceFeature.ValveRodSpeed2)
     {  
        if (g_PIDServiceData.Cur_Error > 0) 
        {
            //将 g_PWM_Duty.WeakGas_Decreaseded 送CCRX寄存器；
            //置放气方式;
            //启动PWM输出；
            OutPut_Negetive( g_PWM_Duty.Weak_Outgassing ); 
        }
        else 
        {
           // 将 g_PWM_Duty.SlowGas_Filled 送CCRX寄存器；
           // 置充气方式；
           // 启动PWM输出；
            OutPut_Positive( g_PWM_Duty.Slow_Gasfilled );
        }
        refreshdata();              // refresh PID_service_data
        g_PIDSeerviceData.SumDeltaU=0;
        return;
     }
     else 
     {
            d=PIDcalculate();      //calculate pid_out DeltaU
            g_PIDServiceData.SumDeltaU+=d;
            c=abs(g_PIDServiceData.SumDeltaU);
            if (c< g_PIDServiceData.Transform)
                 {
                    OutPut_Positive( 0);// 将 0 送CCRX寄存器,使PWM占空比为0；
                    // 启动PWM输出；
                     refreshdata(); // refresh PID_service_data
                     return;
                 }
            else if (g_PIDServiceData.SumDeltaU > 0)
                 {
                    g_PIDServiceData.SumDeltaU-=g_PIDServiceData.Transform;
                    OutPut_Positive( g_PWM_Duty.Weak_Gasfilled);//将 g_PWM_Duty.WeakGas_Fiilled 送CCRX寄存器；
                    //置充气方式;
                    //启动PWM输出；
                 }
            else 
                 {
                    g_PIDServiceData.SumDeltaU+=g_PIDServiceData.Transform;
                    OutPut_Negetive( g_PWM_Duty.Weak_Outgassing); 
                    //将 g_PWM_Duty.WeakGas_Decreased 送CCRX寄存器；
                   // 置放气方式；
                   // 启动PWM输出；
                 }
            refreshdata();  // refresh PID_service_data
            return;
     }
  }
  else  
  {                  // valve position in Dead_Section
        // 将 0 送CCRX寄存器,使PWM占空比为0；
        //启动PWM输出；
        OutPut_Positive( 0);
        refreshdata(); // refresh PID_service_data
        return;
  }
}
//
/*************************************************
*		函数名：refreshdata
*		说  明	刷新PID运算数据								
*		日  期：2015.6.15
*************************************************/
void refreshdata(void)
{
     g_PIDServiceData.Prev_ValvePoint=g_PIDServiceData.LastValvePoint;
     g_PIDServiceData.LastValvePoint=g_PIDServiceData.Cur_ValvePoint;
     g_PIDServiceData.Prev_Error=g_PIDServiceData.LastError;  //e(k-2) <- e(k-1)
     g_PIDServiceData.LastError=g_PIDServiceData.Cur_Error;   //e(k-1) <- e(k)
}

/*************************************************
*		函数名：PIDcalculate
*		说  明	计算PID控制输出值						
*		日  期：2015.6.16
*************************************************/
long PIDcalculate(void)
{
     int error,d_error,dd_error;
     long a;
     error=g_PIDServiceData.Cur_Error;                      //e(k)
     d_error=error-g_PIDServiceData.LastError;              //e(k)-e(k-1)
     dd_error=d_error-g_PIDServiceData.LastError;           //e(k)-2e(k-1)
     dd_error+=g_PIDServiceData.Prev_Error;                 //e(k)-2e(k-1)+e(k-2)
     a=(signed long) g_PIDServiceData.Kp*d_error + 
           g_PIDServiceData.Ki*error +
           g_PIDServiceData.Kd*dd_error;
     return a;
}

/*************************************************
*		函数名：CTRInit
*		说  明	PID控制初始化					
*		日  期：2015.6.16
*************************************************/
void CTRInit(void)
{
    g_PIDServiceData.SumDeltaU=0;  
    g_PIDServiceData.LastError=0;
    g_PIDServiceData.Prev_Error=0;

}

/*************************************************
*		函数名：TestInit
*		说  明 为测试PID控制程序做的初始化					
*		日  期：2015.6.16
*************************************************/
void TestInit(void)
{
    g_PIDServiceData.Kp=10;
    g_PIDServiceData.Ki=1;
    g_PIDServiceData.Kd=2;
  
    g_PIDServiceData.Transform= 10;
    g_DeviceFeature.ValveRodSpeed1=3;
    g_DeviceFeature.ValveRodSpeed2=2;
    g_DeviceFeature.FastSector_Lower=30;
    g_DeviceFeature.SlowSector_Lower=20;
    g_DeviceFeature.DeadSevtion=4;
  
    g_PWM_Duty.Fast_Gasfilled=480;      ;PWM_duty 0.48
    g_PWM_Duty.Fast_Outgassing=490;     ;... 0.49
    g_PWM_Duty.Slow_Gasfilled=430;      ;... 0.43
    g_PWM_Duty.Slow_Outgassing=440;     ;... 0.44
    g_PWM_Duty.Weak_Gasfilled=410;      ;... 0.41
    g_PWM_Duty.Weak_Outgassing=420;     ;... 0.42
    
}   
    
/*************************************************
*		函数名：ValveRodSpeed
*		说  明  求阀杆运动速度					
*		日  期：2015.6.16
*************************************************/    
unsigned int ValveRodSpeed(void)
{
    int a;
    unsigned int b;
    a=g_PIDServiceData.Cur_ValvePoint-g_PIDServiceData.Prev_ValvePoint;
    b=abs(a);
    return b;
}  
    




