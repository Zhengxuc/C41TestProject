#ifndef __Tick_H__
#define __Tick_H__
//------------------------------------------------------------------------------
//# define CPU_SPEED         1000    // 200 - 3000
//------------------------------------------------------------------------------
typedef struct TICK_BITS
{
    unsigned int Ttem               : 7;  //延长计数时间的最大32    0 - 4
    unsigned int SecRun             : 9;  //计数最大512             5 -11    
    unsigned int SecKB              : 5;  // 32                     12-15
    unsigned int SecAL              : 4;  // 16                     16-19  用来控制报警的
    unsigned int DIS_SPEED          : 6;  // 64  
    unsigned char RUN_SPEED         : 8;  // 256   
    unsigned int Second_modulate    : 7;  // 128                     20-23
    unsigned int SecCONFIRM         : 7;  // 128                    20-23
    unsigned int Message_Load_Rate  : 6;  // 64                     24-27
    unsigned int Display_Load_Rate  : 6;  // 64                     28-31
    unsigned int Timer_Config;       // 65535                  28-31
    unsigned int SecINC             : 7;  //20131106增加 用于LINE一键整定
    unsigned int Gasfilled          : 10;   //充气计数 160120  //自适应相关参数
    unsigned int Outgassing         : 10;   //排气计数 160120  //自适应相关参数
    unsigned int OneKey_Flag        : 1; //  20131106增加  一键整定标志
    unsigned int PIDCon_Flag        : 1;
    unsigned int AdjReady           : 1;
    unsigned int Adj_Flag           : 1;
    unsigned int CurrentInRef       : 2;   //指令变化标志 //自适应相关参数
    unsigned int Hart_Flag          : 1; //20151010add hart
    unsigned int received           : 1; // 接收到一
    unsigned int SecHT              : 5; //20151010add hart    
    unsigned short  Times_cnt       : 13;
    unsigned char ADmea_TimeFlag    : 3;
    unsigned char TImea_TimeFlag    : 3;
    unsigned char Out_Toggle        : 1;
    unsigned char Alarm_Toggle      : 1;

}TICK_REG;

extern unsigned char   WDogflag;
//------------------------------------------------------------------------------
extern void Cpu_Tick_Initial( void );
//------------------------------------------------------------------------------
#endif
