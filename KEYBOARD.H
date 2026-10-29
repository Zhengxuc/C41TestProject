#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__
//------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------|
//                INC      key 1      key 2      MODE                          |
//                                                                             |
//                DEC      key 3      key 4     CONFIRM                        |
//-----------------------------------------------------------------------------|
//                         up  0      down 1                                   |
//-----------------------------------------------------------------------------|
//  interrupt mode                                                             |
//-----------------------------------------------------------------------------|
# include <msp430x16x.h>
# include "delay.h"
//------------------------------------------------------------------------------
//  硬件驱动部分涉及的
//------------------------------------------------------------------------------
# define KEY1   BIT4
# define KEY2   BIT5
# define KEY3   BIT6
# define KEY4   BIT7
//------------------------------------------------------------------------------
# define KEYDIR  P2DIR 
# define KEYIN   P2IN
# define KEYSEL  P2SEL
# define KEYIE   P2IE 
# define KEYIES  P2IES
# define KEYIFG  P2IFG
//--------------------------------------------------------------------------
 struct KB_BITS                           // 按键结构
{
    unsigned int KeyF:4;      // 0-3      用来存储键值
    unsigned int KeyL:4;      // 4-7      用来存储键值
    unsigned int KeyTmpF:4;      // 0-3      用来存储键值
    unsigned int KeyTmpL:4;      // 4-7      用来存储键值
    unsigned int Refresh:1;   // 8        新消息标志
    unsigned int PressL:1;    // 9        用来识别键盘的长短按压 
    unsigned int PressLtmp:1;    // 9        用来识别键盘的长短按压 
    unsigned int LockKB:3;    // 10-11    上锁计数
    unsigned int LockCF:1;    // 12-15
    unsigned int KeyOK:1;     // 12-15
    unsigned int Derection:1; // 12-15
    unsigned int LongKB:1;    // 12-15
    unsigned int LongKB_M:1;  // 12-15
    unsigned int QuickKB:1;   // 16       手动加速        
    unsigned int CountKB:13;  // 17-31    保留
    
};
//------------------------------------------------------------------------------
typedef  union  
{
   unsigned long   all;
   struct KB_BITS  bit;
   
}KEYBORAD_REG;
//------------------------------------------------------------------------------
extern void Keyboard_Initial(void);       // 初始化程序
extern unsigned char key_value_temp;
//------------------------------------------------------------------------------
#endif


