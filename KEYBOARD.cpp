//*****************************************************************************|
// Author:      stefanie                                                       |
// date:        2007-10-24                                                     |
// date:        2008-09-18                                                     |
// description: Keyboard for msp430F                                           |
// cpu:         MSP430 F 149                                                   |
// crystal:                                                                    |
// Version:     V1.0                                                           |
//*****************************************************************************|
//# include "KEYBOARD.H"
#include "Device.h"
//------------------------------------------------------------------------------
// 键盘初始化
//------------------------------------------------------------------------------
KEYBORAD_REG KB;
unsigned char key_value_temp;
//------------------------------------------------------------------------------
void Keyboard_Initial(void)            // 9
{
    KEYSEL &=~(KEY1|KEY2|KEY3|KEY4);   // 定义引脚功能       （普通IO口）
    KEYDIR &=~(KEY1|KEY2|KEY3|KEY4);   // 定义引脚方向
    KEYIES &=~(KEY1|KEY2|KEY3|KEY4);   // 由低跳变到高会引发中断
    KEYIFG &=~(KEY1|KEY2|KEY3|KEY4);   // 清除中断标志位    （必须由手动清除）                                                                    
    KEYIE  |= (KEY1|KEY2|KEY3|KEY4);   // 引脚中断功能开
    KB.all = 0x0000;
} 

#pragma vector=PORT2_VECTOR 
__interrupt void Port_Isr(void)
{
   unsigned char temp;
   WDTCTL = WDTPW + WDTHOLD;         // 关闭看门狗
   temp = KEYIN&0XF0;  
   delay(100);                       // 延时祛痘  这个延时影响功耗
   if((KEYIN&0XF0)==temp)            // 确认                  
   { 
      KB.bit.Refresh = 1; 
      switch( KEYIFG&0xF0 )
      {
         case  INC_F     : {
//                            if( KEYIES&INC_F ) { KB.bit.PressLtmp = 0; } //抬起中断
//                            else               { KB.bit.PressLtmp = 1; } //按下中断
                              if( KEYIES&INC_F )key_value_temp=1;
                        } break;     
         case  DEC_F     : {
//                            if( KEYIES&DEC_F ) { KB.bit.PressLtmp = 0; } //抬起中断
//                            else               { KB.bit.PressLtmp = 1; } //按下中断
                             if( KEYIES&DEC_F )key_value_temp=2;
                        } break;       
         case  MODE_F    : {
//                            if( KEYIES&MODE_F ) { KB.bit.PressLtmp = 0; } //抬起中断
//                            else                { KB.bit.PressLtmp = 1;  } //按下中断
                            if( KEYIES&MODE_F )key_value_temp=3;
                        } break;      
         case  CONFIRM_F : {
//                            if( KEYIES&CONFIRM_F ) { KB.bit.PressLtmp = 0; } //抬起中断
//                            else                   { KB.bit.PressLtmp = 1; } //按下中断
                             if( KEYIES&CONFIRM_F  )key_value_temp=4;
                        } break;  
         default:  break;
      }
      KEYIES = temp;
      if( (KB.bit.KeyTmpF==INC_CONFIRM)||(KB.bit.KeyTmpF==DEC_CONFIRM)||(KB.bit.KeyTmpF==MODE_CONFIRM))//||(KB.bit.KeyF==INC_MODE)||(KB.bit.KeyF==DEC_MODE) )  
      { 
         //KB.bit.PressL = 0;
         if( temp==0 )  KB.bit.KeyTmpF = 0;
      }
      else { KB.bit.KeyTmpF = KB.bit.KeyTmpL; }
    }
    ALA.bit.KB__FLAG = 0;
//    ALA.bit.Delay = 0;
    KB.bit.LongKB = 0;
    //----------------------------------
    if( CLK.SecKB ) CLK.SecCONFIRM = 0;
    //----------------------------------
    //CLK.SecCONFIRM = 0;
    //KB.bit.LongKB_M = 0;
    KB.bit.KeyTmpL = temp>>4; 
    KEYIFG &= 0X0F;                    // 清除所有标记     
    WDTCTL = WDT_MRST_32;              // 放狗,并选时钟源为SMCLK,即定时时间为SMCLK/32768
  // _BIC_SR_IRQ(LPM1_bits);          // 退出低功耗  
}


         
