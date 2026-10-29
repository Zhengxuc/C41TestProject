#ifndef __RunControl_H__
#define __RunControl_H__
//-------------------------------------------------------------------------------------------------
# include "Device.h"

#define Maximum_Speed 1003
#define Minimum_Speed  0

extern unsigned int semi_wavex1; 
extern unsigned int semi_wavex2;
extern unsigned int semi_wavey1;
extern unsigned int semi_wavey2;
extern unsigned int semi_wavex;
extern unsigned int semi_wavey;
extern unsigned int PWMduty_ZDF;        //gas-filled PWM-duty in ZD  
extern unsigned int PWMduty_ZDS;        //gas-decreased PWM-duty in ZD 
extern unsigned int PWMduty_ZDF1;     //2016.1.7 a  
extern unsigned int PWMduty_ZDS1;     //2016.1.7 a 
extern unsigned char DataSegmentNumber;
extern unsigned char RelayCounter;
extern unsigned int  LastPer;
//extern float Itest;
extern unsigned char SWflag;            // 0: positive half-wave; 1: negative half-wave
extern unsigned char RelayEndflag;      // 1: relay end 
extern unsigned char RelayReadyflag;    // 1: relay ready ok 
extern unsigned char ValveMoveDIRflag;  // 1: valve position value increase
                                        // 0: valve position value decrease
extern unsigned char RelayDataEndflag;  // 1: data end
extern unsigned char PID_ZZDflag;       // 1: PID ZZD end
extern unsigned int Rdj_times;

extern unsigned int Times1;   //在整理高中低速时用
extern unsigned char Dir_Flag;  //整定速度方向

extern unsigned int g_usup_pwm_max;
extern unsigned int g_usdown_pwm_max;
//-------------------------------------------这个结构体中的参数必须初始化
typedef struct OPERATION_BITS
{
    //-----------------------------------------------------------组态参数
    unsigned int Difference_Direction_Ex    : 1;   // 期望偏差方向
    unsigned int Difference_Direction       : 1;   // 偏差方向
    unsigned int Differential_Direction_Ex  : 1;   // 微分方向
    unsigned int Time_Out_Error_Counter     : 1;   // 微分方向
    unsigned int Error_serious              : 1;   // 微分方向
    unsigned int Direction                  : 2;   // 方向
    //--------------------------------------------------------------------
    unsigned int Function;       // 计数最大32              5 -11
    //-------------------------------------------------------基本控制参数
    unsigned int Expectation;              //期 望 值
    unsigned int Meterage_F;               //测 量 值  前
    unsigned int Meterage_L;               //测 量 值  后
    unsigned int Difference_F;             //偏 差 值  前 
    unsigned int Difference;               //偏 差 值  后
    unsigned int Difference_F_Ex;          //偏 差 值  前
    unsigned int Difference_Ex;            //偏 差 值  后
    unsigned int Differential_Ex;          //微 分 值
    unsigned char Tolerance_Diff;          //容差上限    
    //-------------------------------------------------------------------
    unsigned int Up_Limited;               // 上限值
    unsigned int Down_Limited;             // 下限值
    //----------------------------------------------
    unsigned int Up_Limited_Test;          // 上限值
    unsigned int Down_Limited_Test;        // 下限值
    //----------------------------------------------
    unsigned int Middle_Valve;             // 中点值
    //----------------------------------------------
    unsigned int up_pwm,down_pwm;
    unsigned int i,j;
    int Timer_F[2];
    int Timer_B[2];
    int p1_4;
    unsigned int di_up,di_down;
    unsigned int pwm_up_0[3];
    unsigned int pwm_down_0[3];
    unsigned int ff1,ff2,cc1,ss1;
    unsigned int Range_D;
    unsigned int Range_M;
    unsigned int Range_C;
    
    unsigned int Up_C;
    unsigned int Down_C;
    
    unsigned int Up_C_alarm;
    unsigned int Down_C_alarm;
    
    unsigned int Up_4_2;
    unsigned int Down_4_1;
    
    unsigned int Up_2_5;
    unsigned int Down_2_5;
    
    unsigned int BRND_1_3;
    unsigned int BRND;
    
    unsigned int Up_50;
    unsigned int Down_50;
    
    unsigned int Machine_Up_4_2;
    unsigned int Machine_Down_4_1;
    
    unsigned int ACT_ERR_FOMER;    // 执行期错误的
    unsigned int Act_cnt;
    unsigned int Bre_cnt;
    unsigned int Rdj_cnt;
    unsigned int Rdj_flg;
    unsigned int TIM_ERR_FOMER;
    unsigned char Leak_time;       // 泄露报警时间控制          
    unsigned char Leak_cnt;        // 泄露报警计数  
    unsigned char fh , fl;         //
}OPERATION_REG;



extern void PIDZZD(void);
extern void ReadytoRelay(void);
extern void wave_formPAR(void);
extern void changePWMduty(void);
extern void calculateKpKiKd(void);
extern void P1_2_Rdj_H(void);
extern void P1_2_Rdj(void);
extern void RunControl(void);
extern void SAVE_FLH(  unsigned int tmp );

//-------------------------------------------------------------------------------------------------
#endif
