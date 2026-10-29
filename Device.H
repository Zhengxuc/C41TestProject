#ifndef __Device_H__
#define __Device_H__
//------------------------------------------------------------------------------
# include <msp430x16x.h>
# include "DisDataCalculate.H"
# include "MessageCenter.H"
# include "CurveFitting.H"
# include "RunConfig.H"
# include "RunAuto.H"
# include "RunMen.H"
# include "KEYBOARD.H"
# include "Flash_RW.H"
# include "HT1621.H"
# include "Alarm.h"
# include "AD421.H"
# include "Tick.H"
# include "Uart.H"
# include "Pwm.h"
# include "Ad.H"
# include "Hart.H"
# include "math.h"
//---------------------------------------------------------------------60个字节
extern OPERATION_REG OP;  // long      操作
extern KEYBORAD_REG KB;   // long      键盘 
extern ALARM_REG ALA;     // int       报警
extern TICK_REG CLK;      // long      时钟
extern SYS_REG SYS;       // int       系统
extern VAL_REG VAL;       // int*18    运行值保存
extern PWM_REG PWM;       // int
extern AD_REG AD;         // int*24    AD采样
//extern SYS1_REG SYS1;

extern PIDServiceData g_PIDServiceData;
//extern PWM_Duty g_PWM_Duty;
extern DeviceFeature g_DeviceFeature;

extern double g_dangel_up ,g_dangel_down ,g_dangel_now;
extern double g_dsinAngel_up,g_dsinAngel_down,g_dsinAngel_now;
//------------------------------------------------------------------------------
#endif
