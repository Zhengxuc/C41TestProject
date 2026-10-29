# include <msp430x16x.h>
# include "Device.h"
# include "Dis_lib.h"

# define MOSFET  0
//------------------------------------------------------------------------------
DIS_REG DIS;    // 有MOSFET
//------------------------------------------------------------------------------
#if  MOSFET

#else

//------------------------------------------------------------------------------
// 没有MOSFET
void send_data(unsigned int data_temp,unsigned char loop)
{
    for(;loop!=0;loop--)
    {
       LCD_PORT &= ~WR;                  // 产生下降沿               1
       if((data_temp&0X8000)==0)   
       {
          LCD_PORT &= ~DATA;             // 数据生成                 1
       }
       else
       {
          LCD_PORT |=DATA;                                 //     1
       }
       LCD_PORT |= WR;                 // 数据锁定                 1 
       data_temp=(data_temp<<1);
    }
}

//显示字符串
void send_string(unsigned char mode,unsigned char t,unsigned char upordown)
{
    unsigned char i; 
    if(upordown){
        LCD_PORT &=~CS0;           
        send_data(0xa000,3);        
        send_data(0x0000,6);
        for(i=0;i<t;i++)
        {         
            send_data(number0[debug_string[mode][i]] ,16);
           
        }
        LCD_PORT |=CS0;
    }
    else {
      LCD_PORT &=~CS1;           
        send_data(0xa000,3);        
        send_data(0x0400,6);
        for(i=0;i<t;i++)
        {         
            send_data(number1[debug_big_string[mode][i]] ,16);

        }
        LCD_PORT |=CS1;
      
      
    }
}


//显示数字
void send_num(unsigned char t,unsigned char Dis_W)
{
  unsigned char i;
  if(Dis_W)//显示万位
  {
      LCD_PORT &=~CS0;           
      send_data(0xa000,3);        
      send_data(0x0000,6);
      for(i=0;i<t-1;i++)
      {         
          send_data(number0[DIS.Digital_Dis_W[i]],16);
       
      }
      LCD_PORT |=CS0;
      
      delay(10);
      
      LCD_PORT &=~CS1;           
      send_data(0xa000,3);        
      send_data(0x0400,6);
      for(i=0;i<t;i++)
      {         
          send_data(number1[DIS.Digital_Dis[i]],16);

      }
      LCD_PORT |=CS1;
    
  }
  else
  {
      LCD_PORT &=~CS1;           
      send_data(0xa000,3);        
      send_data(0x0400,6);
      for(i=0;i<t;i++)
      {         
          send_data(number1[DIS.Digital_Dis[i]],16);

      }
      LCD_PORT |=CS1;
  }
}





//显示时间
void send_time(unsigned char t,float time)
{
      unsigned char i;
      unsigned short time_temp;
      time_temp=(short)(time*10);//显示小数点后一位
      for(i=0;i<4;i++)
      {
          DIS.Dis_Time[i]=38;
      }
      if(time_temp>=100)
      {    
          DIS.Dis_Time[7]=(time_temp/100)%10;
          DIS.Dis_Time[6]=(time_temp/10)%10;  
          DIS.Dis_Time[5]=36;   //" _ "
          DIS.Dis_Time[4]=time_temp%10;
      }
      else
      {
          DIS.Dis_Time[7]=(time_temp/10)%10;
          DIS.Dis_Time[6]=36;  //" _ "
          DIS.Dis_Time[5]=time_temp%10;   
          DIS.Dis_Time[4]=38;
           
      }
      LCD_PORT &=~CS0;           
      send_data(0xa000,3);        
      send_data(0,6);
      for(i=0;i<t;i++)
      {         
          send_data(number0[DIS.Dis_Time[i]],16);
       
      }
      LCD_PORT |=CS0;
}

//-----------------------------------------------------------------------
// HT1621初始化    
//-----------------------------------------------------------------------

void HT1621_Initial(void)
{
    unsigned char t;             
  //  P3DIR = 0;                  //将P1口所有的管脚在初始化的时候设置为输入方式
  //  P3SEL = 0;                  //将P1口所有的管脚设置为一般I/O口
    //--------------------------------------------
    P3DIR |= (WR+DATA+CS1+CS0);
    LCD_PORT |=(CS1);          // 液晶引脚置为输出
    LCD_PORT |=(CS0);          // 液晶引脚置为输出
    LCD_PORT |=WR; 
    LCD_PORT |=DATA;           // 液晶引脚置为输出
    //---------------------------------------------  
    LCD_PORT &=~(CS1|CS0);       // HT1621使能                           
    send_data(COMMAND,3);       // 送3位命令模式代码100
    for(t=0;t<=7;t++) 
    {
      send_data(init_tab[t],9); // 送9位命令代码 
    }
    LCD_PORT |=CS1;            // HT1621关闭                           
    LCD_PORT |=CS0;            // HT1621关闭                           
    //-------------------------------------------------------------
    LCD_PORT &=~CS1;               //HT1621使能    因硬件电路取反 
    LCD_PORT &=~CS0;               //HT1621使能   因硬件电路取反
    send_data(0xa000,3);          //送3位“写 ”模式命令代码101
    send_data(0x0000,6);          //送6位首地址00h   
    for(;t<=32;t++)                
    {
        send_data(0x00,4);        //下面的四个数字
    }         
    LCD_PORT |=CS1;              //HT1621关闭   因硬件电路取反   
    LCD_PORT |=CS0;              //HT1621关闭   因硬件电路取反  
    //--------------------------------------------------------------
    LCD_PORT &=~(CS0+CS1);         // HT1621使能    因硬件电路取反 
    send_data(0xa000,3);        // 送3位“写 ”模式命令代码101
    send_data(0x0000,6);        // 送6位首地址00h 
    for(t=0;t<=7;t++)
    {
       send_data(0xffff,16);       
    }  
    LCD_PORT |=(CS0+CS1);       // HT1621关闭   因硬件电路取反  
    //delay(5000); 
}

void Display_Debug(void)
{
  unsigned char t;

    
    
    LCD_PORT &=~CS1;               //  HT1621使能    因硬件电路取反 
    send_data(0xa000,3);          //  送3位“写 ”模式命令代码101
    send_data(0x0400,6);          //  送6位首地址00h 
    for( t=0;t<=7;t++ )
           {
              send_data(0,16);       
           } 
    LCD_PORT |=CS1;              //  HT1621关闭   因硬件电路取反 
    
    
    LCD_PORT &=~CS1;               //  HT1621使能    因硬件电路取反 
    send_data(0xa000,3);          //  送3位“写 ”模式命令代码101
    send_data(0x0000,6);          //  送6位首地址00h 
    send_data(DIS.SignL,4);       //  写入左边需要显示的图形
    LCD_PORT |=CS1;              //  HT1621关闭   因硬件电路取反 
     delay(10); 
    //---------------------------------------------------------------
    LCD_PORT &=~CS1;               //  HT1621使能    因硬件电路取反 
    send_data(0xa000,3);          // 送3位“写 ”模式命令代码101
    send_data(0x4400,6);          // 送6位首地址00h 
    send_data(DIS.SignR,12);      // 写入左边需要显示的图形
    LCD_PORT |=CS1;              //  HT1621关闭   因硬件电路取反 

    //----------------------------------------------------------小   
        switch(MENU)
        {
          case main_menu: 
                if(!Control_Flag.mode)
                  send_string(0,8,1);//mode1  上
                else send_string(1,8,1);//mode2 上
                delay(10); 
                send_string(0,4,0);
                break;
          case valve_menu:
                if(!Control_Flag.valve_sort)
                  send_string(5,8,1);//safe  上
                else send_string(6,8,1);//lock 上
                delay(10); 
                send_string(0,4,0);
               break;
          
          case mode1_menu:                            
                  if(!Control_Flag.start && !Control_Flag.empty)
                  {
                    send_string(2,8,1);//stop 上                                       
                    send_num(4,CalculateTestTimes(Test_times)); //显示数字                                                                                               
                    
                  }
                  else if(Control_Flag.empty)
                  {
                     send_string(4,8,1);//EMPTY 上
                     send_string(1,4,0);//0000   下  
                  }
                  else 
                  {
                      switch(Control_Flag.mode1_measure)
                      {
                          case adc_measure:
                                          send_string(7,8,1);//WAITTING 上
                                          send_string(1,4,0);//0000   下
                                          break;
                          case time_measure:
                                          send_string(8,8,1);//SETTING 上
                                          send_num(4,CalculateTestTimes(Test_times));//显示次数   下
                                          break;
                          case auto_measure:
                                          send_time(8,Average_Time);                 //平均时间 上
                                          send_num(4,CalculateTestTimes(Test_times));//显示次数   下
                                          break;
                          case stop_measure:
                                          if(CLK.Alarm_Toggle)send_string(9,8,1);         //报警图标 上
                                          else send_string(10,8,1);
                                          send_num(4,CalculateTestTimes(Test_times));//显示次数   下
                                          break;
                          default:break;
                      }
                                     
                  }
               break;
           case mode2_menu:                            
                  if(!Control_Flag.start && !Control_Flag.empty)   //
                  {
                    send_string(2,8,1);//stop 上                                       
                    send_num(4,CalculateTestTimes(Test_times)); //显示数字                                                                                               
                     
                  }
                  else if(Control_Flag.empty)
                  {
                     send_string(4,8,1);//EMPTY 上
                     send_string(1,4,0);//0000   下  
                  }
                  else 
                  {
                    send_string(3,8,1);  //start
                    send_num(4,CalculateTestTimes(Test_times)); //显示数字 
                    
                    
                  }
               break;
        default: break; 
               
               
                            
        }
        
}
void Display(void)
{
    unsigned char t;
    //----------------------------------------------------------小
    LCD_PORT &=~CS0;             //HT1621使能   因硬件电路取反
    send_data(0xa000,3);        //送3位“写 ”模式命令代码101
    send_data(0x0000,6);        //送6位首地址00h
    if( DIS.ScreenS == ALARM_S )
    {
        for( t=0;t<=7;t++ )
        {
           send_data( number0[ DIS.Small_Dis[t] ], 16 );       
        }
    }
    else
    { //增加自动模式下显示目标值

        if( SYS.bit.PageStateL == AUTO )
        {
           for( t=0;t<=2;t++ )
           {
             //send_data( number0[ DIS.Small_Dis[t] ], 16 );
             // Y:增加小数显示
                if (g_PIDServiceData.SetPointDis < 1005)
                {
                    if (t == 1)
                    {
                        send_data((number0[DIS.Small_Dis[t]] | 0x0001), 16); // Y：带小数的显示
                    }
                    else
                    {
                        send_data(number0[DIS.Small_Dis[t]], 16);
                    }
                }
                else
                    send_data(number0[DIS.Small_Dis[t]], 16);
           }       
           for( t=3;t<=7;t++ )
           {
              send_data(number0[ small_sting[DIS.ScreenS][t] ],16);       
           }         
        
        }
        else
        {
           for( t=0;t<=7;t++ )
           {
              send_data(number0[ small_sting[DIS.ScreenS][t] ],16);       
           }
        }
    }  
    LCD_PORT |=CS0;            //HT1621关闭   因硬件电路取反
    //------------------------------------------------------------大
    delay(10); 
    LCD_PORT &=~CS1;                      // HT1621使能    因硬件电路取反      
    send_data(0xa000,3);                 // 送3位“写 ”模式命令代码101
    send_data(0x0400,6);                 // 送6位首地址00h 
    if(DIS.ScreenB==DIGITAL)
    {
        for(t=0;t<=3;t++)                
        {   
            if(DIS.Point==t) { send_data(number1[ DIS.Digital_Dis[t] ] | 0X1000,16); }
            else             { send_data(number1[ DIS.Digital_Dis[t] ],16);          } //下面的四个数字
        } 
    }  
    else if(DIS.ScreenB==ALARM)
    {
        for(t=0;t<=3;t++)                
        {   
             send_data(number1[ DIS.Alarm_Dis[t] ],16); 
        } 
    }    
    else
    {  
        if( SYS.bit.PageS2 == 0 && SYS.bit.PageStateL == CONFIG )
        {  
            for(t=0;t<=3;t++)                
            {   
              if(DIS.Point==t) { send_data(number1[  big_sting[DIS.ScreenB][t]  ] | 0X1000,16); }
              else             { send_data(number1[  big_sting[DIS.ScreenB][t]  ],16);     } //下面的四个数字
            } 
        }
        else
        {
            for(t=0;t<=3;t++)                
            {   
                send_data(number1[  big_sting[DIS.ScreenB][t]  ],16);   //下面的四个数字
            }  
        }  
    }
    LCD_PORT |=CS1;                     //HT1621关闭   因硬件电路取反 
    delay(10); 
    //------------------------------------------------------------
    LCD_PORT &=~CS1;               //  HT1621使能    因硬件电路取反 
    send_data(0xa000,3);          //  送3位“写 ”模式命令代码101
    send_data(0x0000,6);          //  送6位首地址00h 
    send_data(DIS.SignL,4);       //  写入左边需要显示的图形
    LCD_PORT |=CS1;              //  HT1621关闭   因硬件电路取反 
     delay(10); 
    //---------------------------------------------------------------
    LCD_PORT &=~CS1;               //  HT1621使能    因硬件电路取反 
    send_data(0xa000,3);          // 送3位“写 ”模式命令代码101
    send_data(0x4400,6);          // 送6位首地址00h 
    send_data(DIS.SignR,12);      // 写入左边需要显示的图形
    LCD_PORT |=CS1;              //  HT1621关闭   因硬件电路取反  
}



//----------------------------------------------------------------
//--有来显示摩擦LOGO      2017年3月10日
//----------------------------------------------------------------
//                             Welcome
//                               MORC
//----------------------------------------------------------------

void Dis_LOGO (void)
{
    unsigned char t;
    //----------------------------------------------------------小
    LCD_PORT &=~CS0;             //HT1621使能   因硬件电路取反
    send_data(0xa000,3);        //送3位“写 ”模式命令代码101
    send_data(0x0000,6);        //送6位首地址00h
    
    for( t=0;t<=7;t++ )
    {
       send_data( number0[ small_sting[130][t]  ], 16 );      
    }    
      
    LCD_PORT |=CS0;            //HT1621关闭   因硬件电路取反
    
    delay(10); 
    LCD_PORT &=~CS1;                      // HT1621使能    因硬件电路取反      
    send_data(0xa000,3);                 // 送3位“写 ”模式命令代码101
    send_data(0x0400,6);                 // 送6位首地址00h 

    for(t=0;t<=3;t++)                
    {   
       send_data(number1[ big_sting[101][t] ],16); 
    }     
              
    LCD_PORT |=CS1;                     //HT1621关闭   因硬件电路取反 
    delay(10); 
    //------------------------------------------------------------
    LCD_PORT &=~CS1;               //  HT1621使能    因硬件电路取反 
    send_data(0xa000,3);          //  送3位“写 ”模式命令代码101
    send_data(0x0000,6);          //  送6位首地址00h 
    send_data(DIS.SignL,4);       //  写入左边需要显示的图形
    LCD_PORT |=CS1;              //  HT1621关闭   因硬件电路取反 
     delay(10); 
    //---------------------------------------------------------------
    LCD_PORT &=~CS1;               //  HT1621使能    因硬件电路取反 
    send_data(0xa000,3);          // 送3位“写 ”模式命令代码101
    send_data(0x4400,6);          // 送6位首地址00h 
    send_data(DIS.SignR,12);      // 写入左边需要显示的图形
    LCD_PORT |=CS1;              //  HT1621关闭   因硬件电路取反    
    delay(800); 
   
}

#endif
