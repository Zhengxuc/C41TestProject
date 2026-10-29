# include <msp430x16x.h>
# include "Device.H"
# include "Limited.H"

//-------------------------
SYS_REG SYS;
VAL_REG VAL;
ALARM_REG ALA;
//SYS1_REG SYS1;
//-------------------------
extern const unsigned char small_sting[][8];
unsigned char MENU=main_menu;
_Control_Flag Control_Flag;



void flash_write_data(unsigned int addr,char *value, unsigned char length)
{
     
      char *Flash_ptr; //Flash指针
      unsigned int i;

      Flash_ptr = (char*) addr; //初始化Flash指针
      FCTL1 = FWKEY+ERASE; //允许擦除
      FCTL2 = FWKEY + FSSEL0 + FN0;
      FCTL3 = FWKEY; //解锁 
      
      *Flash_ptr = 0; //空写，启动擦除      
    
      
      FCTL1 = FWKEY+WRT; //允许写
     for(i=0;i<length;i++) //循环写信息段A的前length个字节
      {
          *Flash_ptr++= *value++;
            
      }
      FCTL1 = FWKEY;
      FCTL3 = FWKEY+LOCK; //锁定
      
      
      
     
}



//从地址addr开始读，读len个
void flash_read_data(unsigned int addr, char *data, unsigned char len)
{
    unsigned char i;
   for (i = 0; i < len; i++) {
        data[i] = *(unsigned short *) (addr+i );
       
   }
    
}



void Key_Deal(void)
{
     
      switch(MENU)
      {
        case main_menu:
          
            if ( KB.bit.Refresh )
            {
                 KB.bit.Refresh = 0;                 
                  switch(  key_value_temp )
                  {   
                      case 4: Control_Flag.mode=0;break;
                      case 2: Control_Flag.mode=1;break;
                      case 1: MENU=valve_menu;break;
                      default:  break;
                  }
                 key_value_temp=0; 
                 
            }
         break;
         case valve_menu:      
            if ( KB.bit.Refresh )
            {
                 KB.bit.Refresh = 0;                  
                  switch( key_value_temp )
                  {   
                      case 3 :  MENU=main_menu;Control_Flag.valve_sort=0;break;
                      case 4 :  Control_Flag.valve_sort=0;break;//安全
                      case 2 :  Control_Flag.valve_sort=1;break;//闭锁
                      case 1 :  MENU=(Control_Flag.mode?mode2_menu:mode1_menu);break;//确认
                      default:  break;
                  } 
                   key_value_temp=0; 
                 
            }
        case mode1_menu:      
            if ( KB.bit.Refresh )
            {
                 KB.bit.Refresh = 0;                 
                  switch( key_value_temp )
                  {   
                      case 3 : MENU=main_menu; Control_Flag.start=0; Control_Flag.empty=1;break;
                      case 4 :  //开始
                              Control_Flag.start=1;
                              Control_Flag.mode1_measure = adc_measure;
                              Control_Flag.empty=0;
                              break;
                      case 2 :  //停止
                              Control_Flag.start=0;
                              Control_Flag.mode1_measure=stop_measure;
                              Control_Flag.empty=0;
                              Control_Flag.flash_refresh=1;//写flash
                              break;
                      case 1 :  Control_Flag.empty=1;Test_times=0;Control_Flag.flash_refresh=1;break;//清空
                      default:  break;
                  }                    
                 key_value_temp=0; 
            }
         break;
        
        case mode2_menu:      
            if ( KB.bit.Refresh )
            {
                 KB.bit.Refresh = 0;                  
                  switch( key_value_temp )
                  {   
                      case 3 :  MENU=main_menu; Control_Flag.start=0; Control_Flag.empty=1;break;
                      case 4 :  Control_Flag.start=1;CLK.Out_Toggle=1;Control_Flag.empty=0;break;//开始
                      case 2 :  //停止
                                Control_Flag.start=0;
                                Control_Flag.empty=0;
                                Control_Flag.flash_refresh=1;//写flash
                                break;
                      case 1 :  Control_Flag.empty=1;Test_times=0;Control_Flag.flash_refresh=1;break;//清空
                      default:  break;
                  }
                   key_value_temp=0; 
                 
            }
         break;   
        default:break;        
      }
      
  
  
}

void MessageCenter( void )
{   
    KB.bit.KeyL = KB.bit.KeyTmpL; KB.bit.KeyF = KB.bit.KeyTmpF; KB.bit.PressL = KB.bit.PressLtmp;
    //------------------------------------------------------------------消息检查 
    switch( SYS.bit.PageStateL ) 
    {
        case AUTO :{  //---------------------------------------------AUTO----------------------------------------------AUTO----------------------------
                      //-----------------------------键盘消息处理------------------------  
                      if ( KB.bit.Refresh )
                      {
                              KB.bit.Refresh = ALA.all = 0;    
                              if( KB.bit.PressL )         //按下中断
                              {  
                                  switch(  KB.bit.KeyL )
                                  { 
                                     case CONFIRM :    { 
                                                           CLK.SecKB = 3; CLK.SecCONFIRM = 1;  DIS.Point = 4;
                                                           if( SYS.bit.PageS2==1 )      { DIS.SignR = 0; DIS.SignR &= ~MODE_mA;   }
                                                           else if( SYS.bit.PageS2==2 ) { DIS.SignR = 0; DIS.SignR &= ~MODE_DEGR; }
                                                           else                         { DIS.SignR = 0; DIS.SignR &= ~MODE_PERC; }
                                                       }break;
                                     case INC_CONFIRM  : if(KB.bit.LockKB<3) KB.bit.LockKB++; if(KB.bit.LockKB==3) KB.bit.LockCF=1; CLK.SecCONFIRM = 0; KB.bit.CountKB = 0; break;
                                     case DEC_CONFIRM  : if(KB.bit.LockKB>0) KB.bit.LockKB--; if(KB.bit.LockKB==0) KB.bit.LockCF=0; CLK.SecCONFIRM = 0; KB.bit.CountKB = 0; break;
                   //                  case MODE_CONFIRM :{ 
                   //                                         CLK.SecCONFIRM = 0;
                   //                                         SYS.bit.PageStateF = SYS.bit.PageStateL; 
                   //                                         SYS.bit.PageStateL = QUICK;  KB.bit.KeyOK = 0; SYS.bit.PageS2 = 0;
                   //                                         DIS.SignL &= ~MODE_AUTO; return;
                   //                                     }
                                     case INC :         {
                                                            CLK.SecKB = 5; CLK.SecINC =1; CLK.SecCONFIRM = 1; DIS.Point = 4;
                                                        }break;

                                     default: ;break;
                                  }  
                              }
                              else                //抬起中断
                              {
                                  switch(  KB.bit.KeyF )
                                  {   //-------------------------------键抬起操作--------
                                      case INC  : {
                                                    if( CLK.SecKB == 0 && CLK.SecINC)
                                                    {
                                                        CLK.OneKey_Flag =1;    // 一键整定标志
                                                        
                                                        OP.up_pwm =950;
                                                        OP.down_pwm =950;
                                                        OP.di_up =OP.di_down=0;  //置ADJ初值  160115
                                                        
                                                        SYS.bit.PageStateF = SYS.bit.PageStateL; 
                                                        SYS.bit.PageStateL = CONFIG;
                                                        SYS.bit.PageS1 = PG1;
                                                        SYS.bit.PageS2 = P1_2_RDJ;
                                                        OP.Function = P1_2F;
                                                    }
                                                  //  else
                                                   // {
 
                                                    //  SYS.bit.PageS2++; SYS.bit.PageS2 = SYS.bit.PageS2%3; SYS.bit.Step = 0;
                                                   //   if( SYS.bit.PageS2==1 )      { DIS.SignR = 0; DIS.SignR |= MODE_mA;   }
                                                   //   else if( SYS.bit.PageS2==2 ) { DIS.SignR = 0; DIS.SignR |= MODE_DEGR; }
                                                    //  else                         { DIS.SignR = 0; DIS.SignR |= MODE_PERC; } 
                                                  //  }
                                                     CLK.SecCONFIRM = 0;
                                                     CLK.SecINC =0;
                                                    }break;             
                                      case DEC  : {
                                                      SYS.bit.PageS2++; SYS.bit.PageS2 = SYS.bit.PageS2%3; SYS.bit.Step = 0;
                                                      if( SYS.bit.PageS2==1 )      { DIS.SignR = 0; DIS.SignR |= MODE_mA;   }
                                                      else if( SYS.bit.PageS2==2 ) { DIS.SignR = 0; DIS.SignR |= MODE_DEGR; }
                                                      else                         { DIS.SignR = 0; DIS.SignR |= MODE_PERC; } 
                                                  }break;    
                                      case MODE : { 
                                                   CLK.SecCONFIRM = 0;
                                                   SYS.bit.PageStateF = SYS.bit.PageStateL;
                                   //                if( SYS.bit.PageS1 == 0 )    //  自动页面跳转
                                   //                { 
                                   //                       SYS.bit.PageS1 = 1;   //  转到 AUTO_PID
                                   //                }
                                   //                else if( SYS.bit.PageS1 == 1 )
                                   //                {   
                                   //                       SYS.bit.PageS1 = 2;   //  转到 AUTO_MIX 
                                   //                } 
                                   //                else if( SYS.bit.PageS1 == 2 )
                                   //                {      
                                                        SYS.bit.PageStateL = MAN; SYS.bit.PageS1 = 0;
                                                        DIS.SignL &= ~MODE_AUTO; DIS.SignR = 0;
                                                        DIS.SignR |= MODE_PERC;                                                                                                                       DIS.Small_Dis[4] = 38; 
                                                        
                                                        
                                                        return;   
                                   //                }
                                                   
                                                 }break;  //页面变化            
                                     case CONFIRM :{
                                                      if( CLK.SecKB == 0 && CLK.SecCONFIRM )  
                                                      {  
                                                        
                                                        OutPut_Positive( 0);// 进入组态关闭输出  150123add
                                                        
                                                        SYS.bit.PageStateF = SYS.bit.PageStateL; 
                                                        SYS.bit.PageStateL = CONFIG;
                                                        SYS.bit.PageS1 = SYS.bit.PageS2 = 0;
                                                        DIS.Point = 4;
                                                        DIS.SignL = DIS.SignR = CLK.SecCONFIRM = KB.bit.CountKB = OP.Function = 0;
                                                        KB.bit.KeyOK = 1; return;
                                                      }
                                                      else
                                                      {
                                                         if( SYS.bit.PageS2==1 )      { DIS.SignR = 0; DIS.SignR |= MODE_mA;   }
                                                         else if( SYS.bit.PageS2==2 ) { DIS.SignR = 0; DIS.SignR |= MODE_DEGR; }
                                                         else                         { DIS.SignR = 0; DIS.SignR |= MODE_PERC; }  
                                                      }  
                                                      CLK.SecCONFIRM = 0;
                                                 }break;
                                     default: ; 
                                  }
                              }
                       }
                       //------------------------------------------------------------
                       if( CLK.SecCONFIRM )      { DIS.ScreenB = M__4M + CLK.SecKB; }
                       else                      { DIS.ScreenB = DIGITAL;}
//                       if( CLK.SecINC )      { DIS.ScreenB = M__4M + CLK.SecKB; }
//                       else                      { DIS.ScreenB = DIGITAL;}
                       //------------------------------------------------------------
                       if (KB.bit.LockCF) { DIS.SignR |= MODE_LOCK;   }
                       else               { DIS.SignR &= ~MODE_LOCK;  }
                      
                       if( SYS.bit.Keep_err )
                       {
                          if ( SYS.bit.Dida ) { DIS.ScreenS = NULL_SMA;  DIS.SignR &= ~MODE_LOCK; }
                          else                { DIS.ScreenS = AUTO_RDP;  DIS.SignR |= MODE_LOCK;  }
                       }
                       else //------------------------------------------------
                       {
                         
                            if( SYS.bit.PageS1 == 0 )    //  自动页面跳转
                            { 
                               DIS.ScreenS = AUTO_RDP;   //  转到 AUTO_PID 显示
                            }
                            /*
                            else if( SYS.bit.PageS1 == 1 )
                            {   
                               DIS.ScreenS = AUTO_PID;  //  转到 AUTO_MIX  显示
                            } 
                            else if( SYS.bit.PageS1 == 2 )
                            {
                               DIS.ScreenS = AUTO_MIX;  //  转到 AUTO_MIX  显示
                            }  
                        */ 
                       }   
                       //-------------------------------------------显示信息发送=
                       if ( SYS.bit.Dida )        { DIS.SignL &= ~MODE_AUTO;}
                       else                       { DIS.SignL |= MODE_AUTO; }
                       
                    }break;
        case MAN :{ //-----------------------------------------MAN------------------------MAN----------------------------------    
                      if ( KB.bit.Refresh )
                      {
                              KB.bit.Refresh = ALA.all = 0;     // 对键盘的更新做取消操作
                              if( KB.bit.PressL )               // 按下中断
                              {  
                                  switch(  KB.bit.KeyL )
                                  { 
                                     case INC  :{   
                                                   KB.bit.CountKB = VAL.V_p5_5__K5; KB.bit.LongKB = 1; CLK.SecKB = 4; KB.bit.Derection = 1;
                                                }break;    
                                     case DEC  :{   
                                                   KB.bit.CountKB = VAL.V_p5_6__K6; KB.bit.LongKB = 1; CLK.SecKB = 4; KB.bit.Derection = 0; 
                                                }break;
                                    // case INC_DEC :{
                                    //                KB.bit.QuickKB = 1;
                                    //              }break;
                                     case CONFIRM :{ 
                                                      CLK.SecKB = 3; CLK.SecCONFIRM = 1;
                                                      DIS.SignL &= ~MODE_;
                                                      DIS.SignR &= ~MODE_PERC;
                                                      DIS.Point = 4;
                                                   }break;
                                     case INC_CONFIRM  : if(KB.bit.LockKB<3) KB.bit.LockKB++; if(KB.bit.LockKB==3) KB.bit.LockCF=1; KB.bit.LongKB = 0;DIS.SignR = 0; KB.bit.CountKB = 0; break;
                                     case DEC_CONFIRM  : if(KB.bit.LockKB>0) KB.bit.LockKB--; if(KB.bit.LockKB==0) KB.bit.LockCF=0; KB.bit.LongKB = 0;DIS.SignR = 0; KB.bit.CountKB = 0; break;
                                     case MODE_CONFIRM :{ 
                                                           SYS.bit.PageStateF = SYS.bit.PageStateL; 
                                                           SYS.bit.PageStateL = QUICK;  KB.bit.KeyOK = 0; SYS.bit.PageS2 = 0;
                                                           DIS.SignL &= ~MODE_HAND; DIS.SignL &= ~MODE_; return;
                                                        }
                                     default: ;break;
                                  }  
                              }
                             else                //抬起中断
                              {
                                  switch(  KB.bit.KeyF )
                                  {
                                     case INC  : KB.bit.CountKB = 900; KB.bit.LongKB = 0; KB.bit.Derection = 1; KB.bit.QuickKB = 0;break; //if (KB.bit.CountKB<990)  KB.bit.CountKB++; KB.bit.LongKB = 0; break;          
                                     case DEC  : KB.bit.CountKB = 900; KB.bit.LongKB = 0; KB.bit.Derection = 0; KB.bit.QuickKB = 0;break; //if (KB.bit.CountKB>10)   KB.bit.CountKB--; KB.bit.LongKB = 0; break;    
                                     case MODE : {
                                                     SYS.bit.PageStateF = SYS.bit.PageStateL;
                                                     if( SYS.bit.PageS1 ) { SYS.bit.PageStateL = AUTO; SYS.bit.PageS1 = 0; DIS.SignL &= ~MODE_HAND; DIS.SignL &= ~MODE_; DIS.SignR |= MODE_PERC; return;}
                                                     else                 { SYS.bit.PageS1 = 1; DIS.SignL &= ~MODE_;  DIS.SignR &= ~MODE_PERC; }   
                                                 }break;  //页面变化
                                     case CONFIRM :{
                                                      if( CLK.SecKB==0 && CLK.SecCONFIRM )  
                                                      {  
                                                        SYS.bit.PageStateF = SYS.bit.PageStateL; 
                                                        SYS.bit.PageStateL = CONFIG;
                                                        SYS.bit.PageS1 = SYS.bit.PageS2 = 0;
                                                        DIS.SignL = DIS.SignR = CLK.SecCONFIRM = OP.Function = KB.bit.CountKB = 0; 
                                                        KB.bit.KeyOK = 1; DIS.SignL &= ~MODE_; return;
                                                      }
                                                      else
                                                      {
                                                        if ( SYS.bit.PageS2 == 0 )  DIS.SignR |= MODE_PERC;
                                                      }
                                                      CLK.SecCONFIRM = 0;
                                                 }break;
                                    
                                     default: ;                  
                                  }   
                              } 
                       }
                       //--------------------------------------------显示信息发送------------------------------------
                       if (KB.bit.LockCF) { DIS.SignR |= MODE_LOCK;   }           //上锁控制-------------------------
                       else               { DIS.SignR &= ~MODE_LOCK;  }
                       switch( SYS.bit.PageS1 )                                   //控制锁住手动的功能---------------
                       {
                           case 0 :{ 
                                        if( SYS.bit.Keep_err )
                                        {
                                           if ( SYS.bit.Dida )        { DIS.ScreenS = NULL_SMA;  DIS.SignR &= ~MODE_LOCK; }
                                           else                       { DIS.ScreenS = MAN_PENS;  DIS.SignR |= MODE_LOCK;  }
                                        }
                                        else   DIS.ScreenS = MAN_PENS;
                                         
                                    }break;          
                           case 1 : { 
                                        if( SYS.bit.Keep_err )
                                        {
                                           if ( SYS.bit.Dida )        { DIS.ScreenS = NULL_SMA;  DIS.SignR &= ~MODE_LOCK; }
                                           else                       { DIS.ScreenS = MAN_SENS;  DIS.SignR |= MODE_LOCK;  }
                                        }
                                        else  DIS.ScreenS = MAN_SENS;

                                    }break;      
                           default: break;
                       }//---------------------------------------------- 用来显示确认键---和进行键盘加速的-----------------
                     
                      if( CLK.SecCONFIRM == 0 )  {   DIS.ScreenB = DIGITAL;  }
                      else                       {   DIS.ScreenB = M__4M + CLK.SecKB;  }
                    
                      if( KB.bit.LongKB )
                      {
                           if(CLK.SecKB < 4 && KB.bit.CountKB < 800 )       { KB.bit.CountKB=KB.bit.CountKB+10;  }   
                      }
                      //-------------------------------------------------------
                       if ( SYS.bit.Dida )        { DIS.SignL &= ~MODE_HAND;} 
                       else                       { DIS.SignL |= MODE_HAND; }
                      //----------------------------------------运行状态信息处理
                    }break;
      case CONFIG :{ //---------CONFIG--------CONFIG------CONFIG-------CONFIG--------CONFIG------CONFIG-------
                      if ( KB.bit.Refresh )
                      {
                          KB.bit.Refresh = ALA.all = 0;     //对键盘的更新做取消操作
                          if( KB.bit.PressL )               //按下中断
                          {  
                                  switch(  KB.bit.KeyL )
                                  { 
                                     case INC  : {
                                                      //-------------------------------------------------------------------------------是为了在个别组态里放置键盘功能
                                                      if( SYS.bit.PageS2 == P1_4STEP && SYS.bit.PageS1 == PG1 && SYS.bit.Step == STP_2 )
                                                      {
                                                           KB.bit.CountKB = 150; KB.bit.LongKB = 1; CLK.SecKB = 3; KB.bit.Derection = 1;
                                                      }
                                                      else if ( SYS.bit.PageS2 == P3_1_MIN && SYS.bit.PageS1 == PG3 && SYS.bit.Step == STP_2 )
                                                      {
                                                           KB.bit.CountKB = 150; KB.bit.LongKB = 1; CLK.SecKB = 3; KB.bit.Derection = 1;
                                                      }  
                                                      else if ( SYS.bit.PageS2 == P3_2_MAX && SYS.bit.PageS1 == PG3 && SYS.bit.Step == STP_2 )
                                                      {
                                                           KB.bit.CountKB = 150; KB.bit.LongKB = 1; CLK.SecKB = 3; KB.bit.Derection = 1;
                                                      }
                                                      else//------------------------------------------------------------------------------------------到这里结束
                                                      {  
                                                         if( SYS.bit.PageS2 ) 
                                                         { 
                                                            KB.bit.LongKB = 1;  CLK.SecKB = 9; 
                                                            KB.bit.Derection = 1; KB.bit.KeyOK = 0;
                                                            //-------------------------------------
                                                            CLK.Message_Load_Rate = CLK.Second_modulate/15-1;   // 消息处理慢
                                                            CLK.Display_Load_Rate = CLK.Second_modulate/15-1;   // 显示快 
                                                            //-------------------------------------
                                                            
                                                            //KB.bit.CountKB = 150; KB.bit.LongKB = 1; CLK.SecKB = 3; KB.bit.Derection = 1;
                                                            
                                                          } CLK.SecCONFIRM = 0;
                                                      }  
                                                 }break;           
                                     case DEC  : {
                                                      //-------------------------------------------------------------------------------是为了在个别组态里放置键盘功能
                                                      if( SYS.bit.PageS2 == P1_4STEP && SYS.bit.PageS1 == PG1 && SYS.bit.Step == STP_2 )
                                                      {
                                                           KB.bit.CountKB = 150; KB.bit.LongKB = 1; CLK.SecKB = 3; KB.bit.Derection = 0;
                                                      }
                                                      else if ( SYS.bit.PageS2 == P3_1_MIN && SYS.bit.PageS1 == PG3 && SYS.bit.Step == STP_2 )
                                                      {
                                                           KB.bit.CountKB = 150; KB.bit.LongKB = 1; CLK.SecKB = 3; KB.bit.Derection = 0;
                                                      }  
                                                      else if ( SYS.bit.PageS2 == P3_2_MAX && SYS.bit.PageS1 == PG3 && SYS.bit.Step == STP_2 )
                                                      {
                                                           KB.bit.CountKB = 150; KB.bit.LongKB = 1; CLK.SecKB = 3; KB.bit.Derection = 0;
                                                      }
                                                      else//------------------------------------------------------------------------------------------到这里结束
                                                      {  
                                                          if( SYS.bit.PageS2 ) 
                                                          { 
                                                             KB.bit.LongKB = 1;  CLK.SecKB = 9; 
                                                             KB.bit.Derection = 0; KB.bit.KeyOK = 0;
                                                             //-------------------------------------
                                                             CLK.Message_Load_Rate = CLK.Second_modulate/15-1;   // 消息处理慢
                                                             CLK.Display_Load_Rate = CLK.Second_modulate/15-1;   // 显示快 
                                                             //-------------------------------------
                                                             
                                                             // KB.bit.CountKB = 150; KB.bit.LongKB = 1; CLK.SecKB = 3; KB.bit.Derection = 0; 
                                                              
                                                          } CLK.SecCONFIRM = 0;
                                                      }   
                                                 }break;  
                                     case CONFIRM :      if( SYS.bit.PageS2==0 ) { CLK.SecKB = 3; CLK.SecCONFIRM = 1; DIS.Point = 4;} break;
                                     case INC_CONFIRM  : if(SYS.bit.PageS2<LimitedPage2[SYS.bit.PageS1]) SYS.bit.PageS2++; CLK.SecCONFIRM = 0; KB.bit.KeyOK = 0;KB.bit.CountKB = 0;SYS.bit.Step = 0;break;
                                     case DEC_CONFIRM  : if(SYS.bit.PageS2>1)   SYS.bit.PageS2--; CLK.SecCONFIRM = 0; KB.bit.KeyOK = 0; KB.bit.CountKB = 0; SYS.bit.Step = 0; break;
                                     //-------------------------------------------
                                     case INC_MODE  : { 
                                                          if( SYS.bit.PageS2 ) 
                                                          { 
                                                             KB.bit.LongKB = 1;  CLK.SecKB = 9; 
                                                             KB.bit.LongKB_M = 1;
                                                             KB.bit.Derection = 1; 
                                                             //-------------------------------------
                                                             CLK.Message_Load_Rate = CLK.Second_modulate/10-1;   // 消息处理慢
                                                             CLK.Display_Load_Rate = CLK.Second_modulate/10-1;   // 显示快 
                                                             //-------------------------------------
                                                          } CLK.SecCONFIRM = 0;  break;
                                                      }  
                                     case DEC_MODE  : {  if( SYS.bit.PageS2 ) 
                                                         { 
                                                             KB.bit.LongKB = 1;  CLK.SecKB = 9; KB.bit.LongKB_M = 1; 
                                                             KB.bit.Derection = 0; KB.bit.KeyOK = 0;
                                                             //-------------------------------------
                                                             CLK.Message_Load_Rate = CLK.Second_modulate/10-1;   // 消息处理慢
                                                             CLK.Display_Load_Rate = CLK.Second_modulate/10-1;   // 显示快 
                                                             //-------------------------------------
                                                         } CLK.SecCONFIRM = 0;
                                                      }break;
                                     //-------------------------------------------
                                     case MODE_CONFIRM :{ 
                                                           CLK.SecCONFIRM = 0;
                                                           SYS.bit.PageS2 = 0;
                                                           DIS.SignR = SYS.bit.Step = 0;
                                                           KB.bit.KeyOK  = 1;
                                                        }break;
                                     case MODE :{ 
                                                      if( SYS.bit.PageS1 == PG1 )
                                                      { 
                                                         if ( SYS.bit.PageS2 == P1_5TEST || SYS.bit.PageS2 == P1_2_RDJ ) 
                                                         {  
                                                            CLK.SecCONFIRM = 1; CLK.SecKB = 3;
                                                         }   
                                                      }
                                                      else if( SYS.bit.PageS1 == PG2 && SYS.bit.PageS2 == P2_9TEST )
                                                      {
                                                            CLK.SecCONFIRM = 1; CLK.SecKB = 3;
                                                      }  
                                                }break;                  
                                     default: ;break;
                                  } 
                                  if( KB.bit.KeyL==INC_CONFIRM||KB.bit.KeyL==DEC_CONFIRM)
                                  {
                                      switch( SYS.bit.PageS1 )
                                      {
                                           case PG1 : 
                                          {    
                                                   switch ( SYS.bit.PageS2 )
                                                   {
                                                       case P1_1_RCR :  KB.bit.CountKB = VAL.V_p1_1_RCR; break;
                                                       case P1_2_RDJ :  KB.bit.CountKB = 0;              break;
                                                       case P1_3BRND :  KB.bit.CountKB = VAL.V_p1_3BRND; break;
                                                       case P1_4STEP :  KB.bit.CountKB = 0;              break;
//                                                       case P1_5TEST :  KB.bit.CountKB = 0;              break;
                                                       case P1_6SAVE :  KB.bit.CountKB = 0;              break;
                                                       default: break;  
                                                  } 
                                           } break;
                                           case PG2 : 
                                           {
                                                  switch ( SYS.bit.PageS2 )
                                                  {
                                                       case P2_1_MIN : KB.bit.CountKB = VAL.V_p2_1_MIN; break;
                                                       case P2_2_MAX : KB.bit.CountKB = VAL.V_p2_2_MAX; break;
                                                       case P2_3_CHR : KB.bit.CountKB = VAL.V_p2_3_CHR; break;
                                                       case P2_4_ACT : KB.bit.CountKB = VAL.V_p2_4_ACT; break;
                                                       case P2_5_QOF : KB.bit.CountKB = VAL.V_p2_5_QOF; break;                      
                                                       case P2_6_TUP : KB.bit.CountKB = VAL.V_p2_6_TUP; break;                        
                                                       case P2_7_TDN : KB.bit.CountKB = VAL.V_p2_7_TDN; break;
                                                       case P2_8_CUS : KB.bit.CountKB = 0; break;
//                                                       case P2_9TEST : KB.bit.CountKB = 0; break;
                                                       case P2_10SAV : KB.bit.CountKB = 0; break;
                                                       default:break;                             
                                                  } 
                                           }break;              
                                           case PG3 :
                                           {
                                                  switch ( SYS.bit.PageS2 )
                                                  {
                                                       case P3_1_MIN : KB.bit.CountKB = 0;              break; //KB.bit.CountKB = VAL.V_p3_1_MIN; break;
                                                       case P3_2_MAX : KB.bit.CountKB = 0;              break; //KB.bit.CountKB = VAL.V_p3_2_MAX; break;
                                                       case P3_3_RCT : KB.bit.CountKB = VAL.V_p3_3_RCT; break;
                                                       case P3_4SAVE : KB.bit.CountKB = 0;              break;
                                                       default:break;                             
                                                  }  
                                           }break;              
                                           case PG4 : 
                                           {  
                                                  switch ( SYS.bit.PageS2 )
                                                  {
                                                      case P4_1_MIN : KB.bit.CountKB = VAL.V_p4_1_MIN; break; 
                                                      case P4_2_MAX : KB.bit.CountKB = VAL.V_p4_2_MAX; break;
                                                      case P4_3_VRL : KB.bit.CountKB = VAL.V_p4_3_VRL; break;
                                                      case P4_4SAVE : KB.bit.CountKB = 0; break;
                                                      default:break;                             
                                                  } 
                                            }break;                            
                                            case PG5 :
                                            {
                                                  switch ( SYS.bit.PageS2 )
                                                  {
                                                      case P5_1FU : KB.bit.CountKB = VAL.V_p5_1__K1; OP.Function = P5_AF; break; 
                                                      case P5_2FD : KB.bit.CountKB = VAL.V_p5_2__K2; OP.Function = P5_AF; break; 
                                                      case P5_3FP : KB.bit.CountKB = VAL.V_p5_3__K3; OP.Function = P5_AF; break; 
                                                      case P5_4FB : KB.bit.CountKB = VAL.V_p5_4__K4; OP.Function = P5_AF; break; 
//                                                      case P5_5BT : KB.bit.CountKB = VAL.V_p5_5__K5; OP.Function = P5_AF; break; 
//                                                      case P5_6SU : KB.bit.CountKB = VAL.V_p5_6__K6; OP.Function = P5_AF; break; 
//                                                      case P5_7SD : KB.bit.CountKB = VAL.V_p5_7__K7; OP.Function = P5_AF; break;
//                                                      case P5_8TF : KB.bit.CountKB = VAL.V_p5_8__K8; OP.Function = P5_AF; break;
                                                      case P5_9TB : KB.bit.CountKB = VAL.V_p5_9__K9; OP.Function = P5_AF; break;
                                                      case P5_10T : KB.bit.CountKB = VAL.V_p5_10_K0; OP.Function = P5_AF; break;
                                                      case P5_11RST : KB.bit.CountKB = 0; break; 
                                                      case P5_12SAV : KB.bit.CountKB = 0; break; 
                                                      default:break; 
                                                  }   
                                            }break;               
                                            case PG6 :
                                            {
                                                  switch ( SYS.bit.PageS2 )
                                                  {
                                                      case  P6_1__LK : KB.bit.CountKB = VAL.V_p6_1__LK; break; 
                                                      case  P6_2__SV : KB.bit.CountKB = VAL.V_p6_2__SV; break; 
                                                      case  P6_3__ZO : KB.bit.CountKB = VAL.V_p6_3__ZO; break; 
                                                      case  P6_4__VR : KB.bit.CountKB = VAL.V_p6_4__VR; break; 
                                                      case  P6_5__TO : KB.bit.CountKB = VAL.V_p6_5__TO; break;
                                                      case  P6_6_MIN : KB.bit.CountKB = VAL.V_p6_6_MIN; break; 
                                                      case  P6_7_MAX : KB.bit.CountKB = VAL.V_p6_7_MAX; break; 
                                                      case  P6_8SAVE : KB.bit.CountKB = 0;              break; 
                                                      default:break;                             
                                                  } 
                                            }break;           
                                            case PG7 :
                                            {                             
                                                  switch ( SYS.bit.PageS2 )
                                                  {
//                                                      case P7_1_MIN : KB.bit.CountKB = VAL.V_p7_1_MIN; break; 
//                                                      case P7_2_MAX : KB.bit.CountKB = VAL.V_p7_2_MAX; break; 
                                                      case P7_3_ACT : KB.bit.CountKB = VAL.V_p7_3_ACT; break; 
//                                                      case P7_4TEST : KB.bit.CountKB = 0; break; 
                                                      case P7_5SAVE : KB.bit.CountKB = 0; break;
                                                      default:break;                             
                                                  }
                                            }break;       
                                            case PG8 :
                                            {                              
                                                  switch ( SYS.bit.PageS2 )
                                                  {
                                                      case P8_1_TRG : KB.bit.CountKB = VAL.V_p8_1_TRG; break; 
                                                      case P8_2_POS : KB.bit.CountKB = VAL.V_p8_2_POS; break;
                                                      case P8_3_FRC : KB.bit.CountKB = 0; break;
                                                      case P8_4_MIN : KB.bit.CountKB = 0; break; 
                                                      case P8_5_MAX : KB.bit.CountKB = 0; break; 
//                                                      case P8_6_POP : KB.bit.CountKB = VAL.V_p8_6_POP; break; 
//                                                      case P8_7_RDJ : KB.bit.CountKB = VAL.V_p8_7_RDJ; break; 
                                                      case P8_8SAVE : KB.bit.CountKB = 0; break; 
                                                      default:break;                             
                                                   }   
                                            }break; 
                                           /* 
                                            case PG9 :
                                              {
                                                  switch (SYS.bit.PageS2 )
                                                  {
                                                      case P9_1 : KB.bit.CountKB = VAL.V_p1_1_RCR; break;
                                                      case P9_2 : KB.bit.CountKB = VAL.V_p1_3BRND; break;
                                                      case P9_3 : KB.bit.CountKB = VAL.V_p2_1_MIN; break;
                                                      case P9_4 : KB.bit.CountKB = VAL.V_p2_2_MAX; break;
                                                      case P9_5 : KB.bit.CountKB = VAL.V_p2_3_CHR; break;
                                                      case P9_6 : KB.bit.CountKB = VAL.V_p2_4_ACT; break;
                                                      case P9_7 : KB.bit.CountKB = VAL.V_p2_5_QOF; break;

                                                      case P9_8 : KB.bit.CountKB = OP.Up_Limited; break;   //VAL.V_p3_1_MIN
                                                      case P9_9 : KB.bit.CountKB = OP.Down_Limited; break;   //VAL.V_p3_2_MAX
                                                      case P9_10 : KB.bit.CountKB = VAL.V_p3_3_RCT; break;
                                                      case P9_11 : KB.bit.CountKB = VAL.V_p4_1_MIN; break;
                                                      case P9_12 : KB.bit.CountKB = VAL.V_p4_2_MAX; break;
                                                      case P9_13 : KB.bit.CountKB = VAL.V_p4_3_VRL; break;
                                                      case P9_14 : KB.bit.CountKB = VAL.V_p5_1__K1; break;
                                                      case P9_15 : KB.bit.CountKB = VAL.V_p5_2__K2; break;
                                                      case P9_16 : KB.bit.CountKB = VAL.V_p5_3__K3; break;
                                                      case P9_17 : KB.bit.CountKB = VAL.V_p5_4__K4; break;
                                                      case P9_18 : KB.bit.CountKB = VAL.V_p5_5__K5; break;
                                                      case P9_19 : KB.bit.CountKB = VAL.V_p5_6__K6; break;
                                                      case P9_20 : KB.bit.CountKB = VAL.V_p5_7__K7; break;
                                                      case P9_21 : KB.bit.CountKB = VAL.V_p5_8__K8; break;
                                                      case P9_22 : KB.bit.CountKB = VAL.V_p5_9__K9; break;
                                                      case P9_23 : KB.bit.CountKB = OP.Middle_Valve; break;
                                                      case P9_24SAV : KB.bit.CountKB = 0; break;
                                                      default:break;  
                                                  }                                                                                 
                                              }
                                             */ 
                                            default:break; 
                                         }
                                  }  
                          }
                          else           //抬起中断
                          {
                                  switch(  KB.bit.KeyF )
                                  {
                                     case INC  : {  
                                                       if(CLK.OneKey_Flag ==1)
                                                         {  
                                                            CLK.OneKey_Flag =0;
                                                            SYS.bit.PageStateF = AUTO;
                                                            SYS.bit.PageStateL = AUTO; 
                                                            DIS.ScreenB = DIGITAL;
                                                            DIS.SignL = DIS.SignR = CLK.SecCONFIRM = 0; 
                                                            SYS.bit.PageS1 = 0; 
                                                            SYS.bit.PageS2 = 0; 
                                                        //    if(SYS.bit.Step == SP_OK)
                                                        //    {
                                                          //   OP.Function = SAVEF; return;
                                                                
                                                              //保存整定好的参数
                                                              
                                                            // ALA.bit.KB__FLAG = 1;  //报警初始化       // }
                                                            // ALA.bit.Break_AU =0;
                                                             Sys_initial();
                                                             CLK.PIDCon_Flag =1;
                                                             
                                                            
                                                              OP.Function = SAVEF; return;
                                                         } 
                                                       CLK.OneKey_Flag =0;
                                                           
                                                      //-------------------------------------------------------------------------------是为了在个别组态里放置键盘功能
                                                      if( SYS.bit.PageS2 == P1_4STEP && SYS.bit.PageS1 == PG1 && SYS.bit.Step == STP_2 )
                                                      {
                                                           KB.bit.CountKB = 900; KB.bit.LongKB = 0; KB.bit.Derection = 1; break;
                                                      }
                                                      else if ( SYS.bit.PageS2 == P3_1_MIN && SYS.bit.PageS1 == PG3 && SYS.bit.Step == STP_2 )
                                                      {
                                                           KB.bit.CountKB = 900; KB.bit.LongKB = 0; KB.bit.Derection = 1; break;
                                                      }  
                                                      else if ( SYS.bit.PageS2 == P3_2_MAX && SYS.bit.PageS1 == PG3 && SYS.bit.Step == STP_2 )
                                                      {
                                                           KB.bit.CountKB = 900; KB.bit.LongKB = 0; KB.bit.Derection = 1; break;
                                                      }
                                                      else//------------------------------------------------------------------------------------------到这里结束
                                                      { 
                                                         // if ((SYS.bit.PageS1 == PG9)&&(KB.bit.CountKB < MAX_P9[SYS.bit.PageS2-1])&&SYS.bit.PageS2 )
                                                         // {
                                                         //      KB.bit.CountKB++; KB.bit.LongKB = 0;  
                                                         // }
                                                        //  else 
                                                       //   {
                                                              if( (KB.bit.CountKB < MAX[SYS.bit.PageS1][SYS.bit.PageS2-1])&&SYS.bit.PageS2 )
                                                              {  
                                                                KB.bit.CountKB++; KB.bit.LongKB = 0; 
                                                              }  
                                                       //   }
                                                          CLK.Message_Load_Rate = CLK.Second_modulate/4-1;   // 消息处理慢
                                                          CLK.Display_Load_Rate = CLK.Second_modulate/3-1;   // 显示快 
                                                          KB.bit.LongKB_M = 0;  CLK.SecKB = 0; 
                                                      }
                                                    //KB.bit.CountKB = 900; KB.bit.LongKB = 0; break;
                                                     
                                                 }break;    
                                     case INC_MODE  : {
                                                         if ( KB.bit.LongKB_M )  
                                                        {  
                                                            if ( ( KB.bit.CountKB < MAX[SYS.bit.PageS1][SYS.bit.PageS2-1] - 10 )&&SYS.bit.PageS2 )
                                                            {  
                                                                KB.bit.CountKB+=10; KB.bit.LongKB = 0;  CLK.SecKB = 0; 
                                                            }
                                                        }
                                                      }break;
                                     case DEC  : { 
                                                   
                                                      //-------------------------------------------------------------------------------是为了在个别组态里放置键盘功能
                                                      if( SYS.bit.PageS2 == P1_4STEP && SYS.bit.PageS1 == PG1 && SYS.bit.Step == STP_2 )
                                                      {
                                                           KB.bit.CountKB = 900; KB.bit.LongKB = 0; KB.bit.Derection = 0; break;
                                                      }
                                                      else if ( SYS.bit.PageS2 == P3_1_MIN && SYS.bit.PageS1 == PG3 && SYS.bit.Step == STP_2 )
                                                      {
                                                           KB.bit.CountKB = 900; KB.bit.LongKB = 0; KB.bit.Derection = 0; break;
                                                      }  
                                                      else if ( SYS.bit.PageS2 == P3_2_MAX && SYS.bit.PageS1 == PG3 && SYS.bit.Step == STP_2 )
                                                      {
                                                           KB.bit.CountKB = 900; KB.bit.LongKB = 0; KB.bit.Derection = 0;break;
                                                      }
                                                      else//------------------------------------------------------------------------------------------到这里结束
                                                      { 
                                                      //   if((SYS.bit.PageS1 == PG9)&&(KB.bit.CountKB > MIN_P9[SYS.bit.PageS2-1])&&SYS.bit.PageS2)
                                                      //   {
                                                      //      KB.bit.CountKB--; KB.bit.LongKB = 0;
                                                      //   }
                                                      //   else
                                                      //   {
                                                            if ( (KB.bit.CountKB > MIN[SYS.bit.PageS1][SYS.bit.PageS2-1])&&SYS.bit.PageS2 )
                                                            {
                                                                KB.bit.CountKB--; KB.bit.LongKB = 0;
                                                            }  
                                                       //  }
                                                         CLK.Message_Load_Rate = CLK.Second_modulate/4-1;   // 消息处理慢
                                                         CLK.Display_Load_Rate = CLK.Second_modulate/3-1;   // 显示快 
                                                         KB.bit.LongKB_M = 0;
                                                         CLK.SecKB = 0; 
                                                      }  
                                                 }break;
                                     case DEC_MODE  : {
                                                          if ( KB.bit.LongKB_M )  
                                                          {  
                                                             if ( ( KB.bit.CountKB > MIN[SYS.bit.PageS1][SYS.bit.PageS2-1] + 10 )&&SYS.bit.PageS2 )
                                                             {  
                                                                KB.bit.CountKB-=10; KB.bit.LongKB = 0;  CLK.SecKB = 0; 
                                                             }
                                                           }
                                                       }break;
                                     case MODE : {
                                                    if( SYS.bit.PageS2==0 )  //--------------P1至P9切换
                                                      { 
                                                        SYS.bit.PageS1++;  
                                                        if( SYS.bit.PageS1 == 8  ) { SYS.bit.PageS1 = 0; }
                                                        SYS.bit.PageS2 = 0; KB.bit.KeyOK = 1;
                                                      }                                                   
                                                    KB.bit.LongKB = 0;
                                                    CLK.Message_Load_Rate = CLK.Second_modulate/4-1;   // 消息处理慢
                                                    CLK.Display_Load_Rate = CLK.Second_modulate/3-1;   // 显示快 
                                                    if ( SYS.bit.PageS1 == PG2 && SYS.bit.PageS2 == P2_8_CUS && KB.bit.LongKB_M == 0 )
                                                    {
                                                       if ( SYS.bit.Step < 17 )  SYS.bit.Step++;
                                                       else   {   SYS.bit.Step = 0;  }
                                                       switch( SYS.bit.Step )
                                                       {
                                                           case 1 : KB.bit.CountKB = VAL.V_p2_8_ST1; break;
                                                           case 2 : KB.bit.CountKB = VAL.V_p2_8_ST2; break;
                                                           case 3 : KB.bit.CountKB = VAL.V_p2_8_ST3; break;
                                                           case 4 : KB.bit.CountKB = VAL.V_p2_8_ST4; break;
                                                           case 5 : KB.bit.CountKB = VAL.V_p2_8_ST5; break;
                                                           case 6 : KB.bit.CountKB = VAL.V_p2_8_ST6; break;
                                                           case 7 : KB.bit.CountKB = VAL.V_p2_8_ST7; break;
                                                           case 8 : KB.bit.CountKB = VAL.V_p2_8_ST8; break;
                                                           case 9 : KB.bit.CountKB = VAL.V_p2_8_ST9; break;
                                                           case 10 : KB.bit.CountKB = VAL.V_p2_8_ST10; break;
                                                           case 11 : KB.bit.CountKB = VAL.V_p2_8_ST11; break;
                                                           case 12 : KB.bit.CountKB = VAL.V_p2_8_ST12; break;
                                                           case 13 : KB.bit.CountKB = VAL.V_p2_8_ST13; break;
                                                           case 14 : KB.bit.CountKB = VAL.V_p2_8_ST14; break;
                                                           case 15 : KB.bit.CountKB = VAL.V_p2_8_ST15; break;
                                                           case 16 : KB.bit.CountKB = VAL.V_p2_8_ST16; break;
                                                           case 17 : KB.bit.CountKB = VAL.V_p2_8_ST17; break;
                                                           default:;break;
                                                       }   
                                                    }
                                                    if( SYS.bit.PageS1 == PG1 )
                                                    { 
                                                         if ( SYS.bit.PageS2 == P1_5TEST || SYS.bit.PageS2 == P1_2_RDJ ) 
                                                         {  
                                                             if( CLK.SecKB==0 && CLK.SecCONFIRM)
                                                             {
                                                                OP.Function = NULLF;
                                                             }
                                                             CLK.SecCONFIRM = 0;
                                                         }   
                                                    }
                                                    else if( SYS.bit.PageS1 == PG2 && SYS.bit.PageS2 == P2_9TEST )
                                                    {
                                                         if( CLK.SecKB==0 && CLK.SecCONFIRM)
                                                         {
                                                              OP.Function = NULLF;
                                                         }
                                                         CLK.SecCONFIRM = 0;
                                                    }  
                                                    KB.bit.LongKB_M = 0;  
                                                 }break;
                                                 
                                     case CONFIRM :{
                                                      
                                                      if(SYS.bit.PageS2==0)
                                                      {  
                                                         if( CLK.SecKB==0 && CLK.SecCONFIRM)  
                                                         {  
                                                            SYS.bit.PageStateL = SYS.bit.PageStateF; 
                                                            DIS.ScreenB = DIGITAL;
                                                            DIS.SignL = DIS.SignR = CLK.SecCONFIRM = 0; 
                                                            SYS.bit.PageS1 = 0; OP.Function = SAVEF; return;
                                                            
                                                           // OP.Function = SAVEF;
                                                         }
                                                         CLK.SecCONFIRM = 0;
                                                      }
                                                      else
                                                      {
                                                          KB.bit.KeyOK  = 1;
                                                          switch( SYS.bit.PageS1 )
                                                          {
                                                               case PG1 : 
                                                               {    
                                                                  switch ( SYS.bit.PageS2 )
                                                                  {
                                                                     case P1_1_RCR :  VAL.V_p1_1_RCR = KB.bit.CountKB; KB.bit.KeyOK  = 1; break;
                                                                     case P1_2_RDJ :  if(KB.bit.CountKB) OP.Function = P1_2F ;break;
                                                                     case P1_3BRND :  VAL.V_p1_3BRND = KB.bit.CountKB; KB.bit.KeyOK  = 1; break;
                                                                     case P1_4STEP :  if(KB.bit.CountKB) OP.Function = P1_4F ; if( SYS.bit.Step == STP_2 ) SYS.bit.Step = SP_OK; break;  
//                                                                     case P1_5TEST :  if(KB.bit.CountKB) OP.Function = P1_5F ;break;
                                                                     case P1_6SAVE :  if(KB.bit.CountKB) OP.Function = P1_6F ;break;
                                                                     default: break;  
                                                                  } 
                                                               }  break;
                                                               case PG2 : 
                                                               {
                                                                  switch ( SYS.bit.PageS2 )
                                                                  {
                                                                     case P2_1_MIN : VAL.V_p2_1_MIN = KB.bit.CountKB; VAL.V_p6_6_MIN = VAL.V_p2_1_MIN - 5; KB.bit.KeyOK  = 1;break;
                                                                     case P2_2_MAX : VAL.V_p2_2_MAX = KB.bit.CountKB; VAL.V_p6_7_MAX = VAL.V_p2_2_MAX + 5; KB.bit.KeyOK  = 1;break;
                                                                     case P2_3_CHR : VAL.V_p2_3_CHR = KB.bit.CountKB; KB.bit.KeyOK  = 1;break;
                                                                     case P2_4_ACT : VAL.V_p2_4_ACT = KB.bit.CountKB; KB.bit.KeyOK  = 1;break;
                                                                     case P2_5_QOF : VAL.V_p2_5_QOF = KB.bit.CountKB; KB.bit.KeyOK  = 1;break;                      
                                                                     case P2_6_TUP : VAL.V_p2_6_TUP = KB.bit.CountKB; KB.bit.KeyOK  = 1;break;                        
                                                                     case P2_7_TDN : VAL.V_p2_7_TDN = KB.bit.CountKB; KB.bit.KeyOK  = 1;break;
                                                                     case P2_8_CUS : if(KB.bit.CountKB) OP.Function = P2_8F; break;
//                                                                     case P2_9TEST : if(KB.bit.CountKB) OP.Function = P2_9F; break;
                                                                     case P2_10SAV : if(KB.bit.CountKB) OP.Function = P2_10F; break;
                                                                     default:break;                             
                                                                  } 
                                                               }break;              
                                                               case PG3 :
                                                               {
                                                                    switch ( SYS.bit.PageS2 )
                                                                    {
                                                                        case P3_1_MIN : if(KB.bit.CountKB) OP.Function = P3_1F; if( SYS.bit.Step == STP_2 ) SYS.bit.Step = STP_3; break; //VAL.V_p3_1_MIN = KB.bit.CountKB;KB.bit.KeyOK  = 1; break;
                                                                        case P3_2_MAX : if(KB.bit.CountKB) OP.Function = P3_2F; if( SYS.bit.Step == STP_2 ) SYS.bit.Step = STP_3; break;//VAL.V_p3_2_MAX = KB.bit.CountKB;KB.bit.KeyOK  = 1; break;
                                                                        case P3_3_RCT : VAL.V_p3_3_RCT = KB.bit.CountKB;KB.bit.KeyOK  = 1; break;
                                                                        case P3_4SAVE : if(KB.bit.CountKB) OP.Function = P3_4F ; break;
                                                                        default:break;                             
                                                                    }  
                                                                }break;              
                                                                case PG4 : 
                                                                {  
                                                                    switch ( SYS.bit.PageS2 )
                                                                    {
                                                                        case P4_1_MIN : VAL.V_p4_1_MIN = KB.bit.CountKB;KB.bit.KeyOK  = 1; break; 
                                                                        case P4_2_MAX : VAL.V_p4_2_MAX = KB.bit.CountKB;KB.bit.KeyOK  = 1; break;
                                                                        case P4_3_VRL : VAL.V_p4_3_VRL = KB.bit.CountKB;KB.bit.KeyOK  = 1; break;
                                                                        case P4_4SAVE : if(KB.bit.CountKB) OP.Function = P4_4F ; break;
                                                                        default:break;                             
                                                                     } 
                                                                }break;                            
                                                                case PG5 :
                                                                {
                                                                     switch ( SYS.bit.PageS2 )
                                                                     {
                                                                        case P5_1FU : VAL.V_p5_1__K1 = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
                                                                        case P5_2FD : VAL.V_p5_2__K2 = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
                                                                        case P5_3FP : VAL.V_p5_3__K3 = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
                                                                        case P5_4FB : VAL.V_p5_4__K4 = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
                                                                        case P5_5BT : VAL.V_p5_5__K5 = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
//                                                                        case P5_6SU : VAL.V_p5_6__K6 = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
//                                                                        case P5_7SD : VAL.V_p5_7__K7 = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
//                                                                        case P5_8TF : VAL.V_p5_8__K8 = KB.bit.CountKB; KB.bit.KeyOK = 1; break;
//                                                                        case P5_9TB : VAL.V_p5_9__K9 = KB.bit.CountKB; KB.bit.KeyOK = 1; break;
                                                                        case P5_10T : VAL.V_p5_10_K0 = KB.bit.CountKB; KB.bit.KeyOK = 1; break;
                                                                        case P5_11RST : if(KB.bit.CountKB) OP.Function = P5_11F ;break; 
                                                                        case P5_12SAV : if(KB.bit.CountKB) OP.Function = P5_12F ;break;
                                                                        default:break;                             
                                                                      }   
                                                                 }break;               
                                                                 case PG6 :
                                                                 {
                                                                    switch ( SYS.bit.PageS2 )
                                                                    {
                                                                        case  P6_1__LK : VAL.V_p6_1__LK = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
                                                                        case  P6_2__SV : VAL.V_p6_2__SV = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
                                                                        case  P6_3__ZO : VAL.V_p6_3__ZO = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
                                                                        case  P6_4__VR : VAL.V_p6_4__VR = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
                                                                        case  P6_5__TO : VAL.V_p6_5__TO = KB.bit.CountKB; KB.bit.KeyOK = 1; break;  
                                                                        case  P6_6_MIN : VAL.V_p6_6_MIN = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
                                                                        case  P6_7_MAX : VAL.V_p6_7_MAX = KB.bit.CountKB; KB.bit.KeyOK = 1; break;  
                                                                        case  P6_8SAVE : if(KB.bit.CountKB) OP.Function = P6_8F ; break;
                                                                        default:break;                             
                                                                    } 
                                                                  }break;           
                                                                  case PG7 :
                                                                  {                             
                                                                      switch ( SYS.bit.PageS2 )
                                                                      {
//                                                                          case P7_1_MIN : VAL.V_p7_1_MIN = KB.bit.CountKB; KB.bit.KeyOK  = 1; break; 
//                                                                          case P7_2_MAX : VAL.V_p7_2_MAX = KB.bit.CountKB; KB.bit.KeyOK  = 1; break; 
                                                                          case P7_3_ACT : VAL.V_p7_3_ACT = KB.bit.CountKB; KB.bit.KeyOK  = 1; break;
//                                                                          case p7_4TEST : if(KB.bit.CountKB) OP.Function = P7_4F ; break;
                                                                          case P7_5SAVE : if(KB.bit.CountKB) OP.Function = P7_5F ; break;
                                                                          default:break;                             
                                                                      }
                                                                  }break;       
                                                                  case PG8 :
                                                                  {                              
                                                                      switch ( SYS.bit.PageS2 )
                                                                      {
                                                                          case P8_1_TRG : VAL.V_p8_1_TRG = KB.bit.CountKB;KB.bit.KeyOK  = 1; break; 
                                                                          case P8_2_POS : VAL.V_p8_2_POS = KB.bit.CountKB;KB.bit.KeyOK  = 1; break;
                                                                          case P8_3_FRC : if(KB.bit.CountKB) OP.Function = P8_3F; break;
                                                                          case P8_4_MIN : if(KB.bit.CountKB) OP.Function = P8_4F; break; 
                                                                          case P8_5_MAX : if(KB.bit.CountKB) OP.Function = P8_5F; break;
//                                                                          case P8_6_POP : VAL.V_p8_6_POP = KB.bit.CountKB;  KB.bit.KeyOK  = 1;break;  //if(KB.bit.CountKB) OP.Function = P8_6F; break; 
//                                                                          case P8_7_RDJ : VAL.V_p8_7_RDJ = KB.bit.CountKB;  KB.bit.KeyOK  = 1;break;  //if(KB.bit.CountKB) OP.Function = P8_7F; break; 
                                                                          case P8_8SAVE : if(KB.bit.CountKB) OP.Function = P8_8F; break; 
                                                                          default:break;                             
                                                                      }   
                                                                   }break;  
                                                                   /*
                                                                  case PG9 :
                                                                  {
                                                                      switch ( SYS.bit.PageS2 )
                                                                      {
                                                                          case P9_1 :  VAL.V_p1_1_RCR = KB.bit.CountKB; KB.bit.KeyOK  = 1; break;
                                                                          case P9_2 :  VAL.V_p1_3BRND = KB.bit.CountKB; KB.bit.KeyOK  = 1; break;
                                                                          case P9_3 : VAL.V_p2_1_MIN = KB.bit.CountKB; VAL.V_p6_6_MIN = VAL.V_p2_1_MIN - 5; KB.bit.KeyOK  = 1;break;
                                                                          case P9_4 : VAL.V_p2_2_MAX = KB.bit.CountKB; VAL.V_p6_7_MAX = VAL.V_p2_2_MAX + 5; KB.bit.KeyOK  = 1;break;
                                                                          case P9_5 : VAL.V_p2_3_CHR = KB.bit.CountKB; KB.bit.KeyOK  = 1;break;
                                                                          case P9_6 : VAL.V_p2_4_ACT = KB.bit.CountKB; KB.bit.KeyOK  = 1;break;
                                                                          case P9_7 : VAL.V_p2_5_QOF = KB.bit.CountKB; KB.bit.KeyOK  = 1;break; 
                                                                          case P9_8 : OP.Up_Limited = KB.bit.CountKB;KB.bit.KeyOK  = 1; break;
                                                                          case P9_9 : OP.Down_Limited = KB.bit.CountKB;KB.bit.KeyOK  = 1; break;
                                                                          case P9_10 : VAL.V_p3_3_RCT = KB.bit.CountKB;KB.bit.KeyOK  = 1; break;           
                                                                          case P9_11 : VAL.V_p4_1_MIN = KB.bit.CountKB;KB.bit.KeyOK  = 1; break; 
                                                                          case P9_12 : VAL.V_p4_2_MAX = KB.bit.CountKB;KB.bit.KeyOK  = 1; break;
                                                                          case P9_13 : VAL.V_p4_3_VRL = KB.bit.CountKB;KB.bit.KeyOK  = 1; break;
                                                                          case P9_14 : VAL.V_p5_1__K1 = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
                                                                          case P9_15 : VAL.V_p5_2__K2 = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
                                                                          case P9_16 : VAL.V_p5_3__K3 = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
                                                                          case P9_17 : VAL.V_p5_4__K4 = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
                                                                          case P9_18 : VAL.V_p5_5__K5 = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
                                                                          case P9_19 : VAL.V_p5_6__K6 = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
                                                                          case P9_20 : VAL.V_p5_7__K7 = KB.bit.CountKB; KB.bit.KeyOK = 1; break; 
                                                                          case P9_21 : VAL.V_p5_8__K8 = KB.bit.CountKB; KB.bit.KeyOK = 1; break;
                                                                          case P9_22 : VAL.V_p5_9__K9 = KB.bit.CountKB; KB.bit.KeyOK = 1; break;
                                                                          case P9_23 : OP.Middle_Valve = KB.bit.CountKB; KB.bit.KeyOK = 1; break;                                                                                                                                                  
                                                                          case P9_24SAV : if(KB.bit.CountKB) OP.Function = P9_24F; break;                                                                                                                                                                                                        
                                                                      }
                                                                  }
                                                                   */
                                                          }     
                                                        // KB.bit.KeyOK  = 1;
                                                      }  
                                                 }break;
                                   
                                     default: ;                  
                                 }   
                           }       
                      }
                      switch( SYS.bit.PageS1 )
                      {
                         case PG1 : 
                         {    
                                 switch ( SYS.bit.PageS2 )
                                 {
                                    case P1_6_NULL:{    //--------------------------------------------------------------------------
                                                       DIS.ScreenS = CONFIG_D; DIS.SignL &= ~MODE_HAND;  DIS.ScreenB = P1_6; if ( CLK.SecCONFIRM == 0 ) { DIS.Point = 1; }
                                                   }break;
                                    case P1_1_RCR :{    //--------------------------------------------------------------------------   
                                                        DIS.ScreenS = p1_1_RCR; DIS.SignR = 0;
                                                        switch( KB.bit.CountKB%3 )
                                                        {   
                                                            case 0 : DIS.ScreenB = LS; break; 
                                                            case 1 : DIS.ScreenB = ROTR; break;
                                                            case 2 : DIS.ScreenB = LT; break;
                                                            default:  break;
                                                        }break;
                                                   }
                                    case P1_2_RDJ: {   //--------------------------------------------------------------------------
                                                      
                                                        DIS.ScreenS = p1_2_RDJ; DIS.SignR = 0;
                                                        switch( SYS.bit.Step )
                                                        {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = RUN;   break; //运行阶段
                                                            case STP_2 :  DIS.ScreenB = ADJ10;  break;
                                                            case STP_3 :  DIS.ScreenB = ADJ20;  break;
                                                            case STP_4 :  DIS.ScreenB = ADJ30;  break;
                                                            case STP_5 :  DIS.ScreenB = ADJ40;  break;
                                                            case STP_6 :  DIS.ScreenB = ADJ50;  break;
                                                            case STP_7 :  DIS.ScreenB = ADJ60;  break;
                                                            case STP_8 :  DIS.ScreenB = ADJ70;  break;
                                                            case STP_9 :  DIS.ScreenB = ADJ80;  break;
                                                            case SP_10 :  DIS.ScreenB = ADJ90;  break;
                                                            case SP_11 :  DIS.ScreenB = ADJ100;  break;
                                                            case SP_12 :  DIS.ScreenB = ADJ110;  break;
                                                            case SP_13 :  DIS.ScreenB = ADJ120;  break;
                                                            case SP_14 :  DIS.ScreenB = ADJ130;  break;
                                                            case SP_15 :  DIS.ScreenB = _S6_;  break;
                                                            case SP_16 :  DIS.ScreenB = _S7_;  break;
                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
                                                            default:  break;
                                                        }  
                                                   } break;
                                    case P1_3BRND: {   //--------------------------------------------------------------------------
                                                        DIS.ScreenB = DIGITAL;  DIS.SignL &= ~MODE_HAND;
                                                        DIS.ScreenS = p1_3BRND; DIS.SignR |=  MODE_PERC; DIS.Point = 2;
                                                   } break;  
                                    case P1_4STEP: { //--------------------------------------------------------------------------
                                                      
                                                        DIS.ScreenS = p1_4STEP; DIS.SignR = 0;
                                                        switch( SYS.bit.Step )
                                                        {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = RUN;      break; //运行阶段
                                                            case STP_2 :  DIS.ScreenB = DIGITAL;  DIS.SignR |= MODE_PERC; break;
                                                            case STP_3 :  DIS.ScreenB = _S2_;     break;
                                                            case STP_4 :  DIS.ScreenB = _S3_;     break;
                                                            case STP_5 :  DIS.ScreenB = _S4_;     break;
                                                            case STP_6 :  DIS.ScreenB = _S5_;     break;//DIS.ScreenB = DIGITAL;  DIS.SignR |= MODE_PERC; break;
                                                            case SP_OK :  DIS.ScreenB = M__OK;    break;
                                                           default:  break;
                                                        } 
                                                        if ( SYS.bit.Dida )         { DIS.SignL &= ~MODE_HAND; }          //显示组态标志显示
                                                        else                       { DIS.SignL |= MODE_HAND;  }
                                                   } break;
//                                    case P1_5TEST: { //--------------------------------------------------------------------------
//                                                      
//                                                        DIS.ScreenS = p1_5TEST; DIS.SignR = 0; DIS.SignL &= ~MODE_HAND;
//                                                        switch( SYS.bit.Step )
//                                                        {
//                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
//                                                            case STP_1 :  DIS.ScreenB = REDY;  break; //运行阶段
//                                                            case STP_2 :  DIS.ScreenB = t_0;   break; //运行阶段
//                                                            case STP_3 :  DIS.ScreenB = t_25;  break; //运行阶段
//                                                            case STP_4 :  DIS.ScreenB = t_50;  break; //运行阶段
//                                                            case STP_5 :  DIS.ScreenB = t_75;  break; //运行阶段
//                                                            case STP_6 :  DIS.ScreenB = t_100; break; //运行阶段
//                                                            case STP_7 :  DIS.ScreenB = COMP;  break; //运行阶段
//                                                            case STP_8 :  DIS.ScreenB = SAVE;  break; //运行阶段
//                                                            case STP_9 :  DIS.ScreenB = PASS;  break; //运行阶段
//                                                            case SP_10 :  DIS.ScreenB = FAIL;  break; //运行阶段
//                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
//                                                            default:  break;
//                                                        }                                       
//                                                   } break;
                                     case P1_6SAVE: { //--------------------------------------------------------------------------
                                                      
                                                        DIS.ScreenS = p1_6SAVE; DIS.SignR = 0;
                                                        switch( SYS.bit.Step )
                                                        {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = REDY;  break; //运行阶段
                                                            case STP_2 :  DIS.ScreenB = RUN;   break; //运行阶段
                                                            case STP_3 :  DIS.ScreenB = SAVE;  break; //运行阶段
                                                            case STP_4 :  DIS.ScreenB = COMP;  break; //运行阶段
                                                            case STP_5 :  DIS.ScreenB = CACU;  break; //运行阶段
                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
                                                            default:  break;
                                                        }                                       
                                                   } break;               
                                    default: break;  
                                 } 
                            } break;
                            case PG2 : 
                            {
                                   switch ( SYS.bit.PageS2 )
                                   {
                                     case P2_10_NULL:{  //--------------------------------键盘-----------------------------------
                                                        
                                                        DIS.ScreenS = CONFIG_D;  DIS.ScreenB = P2_10;  if ( CLK.SecCONFIRM == 0 ) { DIS.Point = 1; }
                                                    }break;
                                     case P2_1_MIN: {   //-----------------------------------------------------------------------
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p2_1_MIN; DIS.SignR |= MODE_mA;   DIS.Point = 2;
                                                     } break;
                                     case P2_2_MAX: {   //----------------------------------------------------------------------
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p2_2_MAX; DIS.SignR |= MODE_mA;   DIS.Point = 2;
                                                    }break;
                                     case P2_3_CHR:{  //------------------------------------------------------------------------
                                                     
                                                        switch(  KB.bit.CountKB%6 )
                                                        {
                                                           case 0 : DIS.ScreenB = LINE;  break;
                                                           case 1 : DIS.ScreenB = M1_25; break;
                                                           case 2 : DIS.ScreenB = M1_50; break;
                                                           case 3 : DIS.ScreenB = M25_1; break;
                                                           case 4 : DIS.ScreenB = M50_1; break;  
                                                           case 5 : DIS.ScreenB = CUST;  break; 
                                                           default:  break;
                                                        }                
                                                        DIS.ScreenS = p2_3_CHR; DIS.SignR = 0;    
                                                     }break;     
                                     case P2_4_ACT:{ //--------------------------------------------------------------------------
                                                      
                                                        switch( KB.bit.CountKB%2 )
                                                        {
                                                           case 0 : DIS.ScreenB = DIRC;  break;
                                                           case 1 : DIS.ScreenB = REVR;  break;                                                      
                                                           default:  break;
                                                        }                
                                                        DIS.ScreenS = p2_4_ACT; DIS.SignR = 0; 
                                                     }break;    
                                       case P2_5_QOF:{   //--------------------------------------------------------------------------
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p2_5_QOF; DIS.SignR |= MODE_PERC;  DIS.Point = 2; 
                                                     }break;                        
                                       case P2_6_TUP:{   //--------------------------------------------------------------------------
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p2_6_TUP; DIS.Point = 4; DIS.SignR = 0;
                                                     }break;                          
                                       case P2_7_TDN:{   //--------------------------------------------------------------------------
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p2_7_TDN; DIS.Point = 4; DIS.SignR = 0;
                                                     } break; 
                                       case P2_8_CUS:{   //--------------------------------------------------------------------------
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.Point = 2;  DIS.SignR |= MODE_PERC; DIS.SignL &= ~MODE_;
                                                        switch( SYS.bit.Step )
                                                        {
                                                            case STP_0 :  DIS.ScreenS = p2_8_CUS; DIS.ScreenB = REDY; DIS.Point = 4; DIS.SignR &= ~MODE_PERC; break;
                                                            case STP_1 :  DIS.ScreenS = SET_10_1; break;
                                                            case STP_2 :  DIS.ScreenS = SET_10_2; break;
                                                            case STP_3 :  DIS.ScreenS = SET_10_3; break;
                                                            case STP_4 :  DIS.ScreenS = SET_10_4; break;
                                                            case STP_5 :  DIS.ScreenS = SET_10_5; break;
                                                            case STP_6 :  DIS.ScreenS = SET_10_6; break;
                                                            case STP_7 :  DIS.ScreenS = SET_10_7; break;
                                                            case STP_8 :  DIS.ScreenS = SET_10_8; break;
                                                            case STP_9 :  DIS.ScreenS = SET_10_9; break;
                                                            case SP_10 :  DIS.ScreenS = SET10_10; break;
                                                            case SP_11 :  DIS.ScreenS = SET10_11; break;
                                                            case SP_12 :  DIS.ScreenS = SET10_12; break;
                                                            case SP_13 :  DIS.ScreenS = SET10_13; break;
                                                            case SP_14 :  DIS.ScreenS = SET10_14; break;
                                                            case SP_15 :  DIS.ScreenS = SET10_15; break;
                                                            case SP_16 :  DIS.ScreenS = SET10_16; break;
                                                            case SP_OK :  DIS.ScreenS = SET10_17; break;
                                                           default:  break;
                                                        }
                                                       
                                                     } break; 
//                                       case P2_9TEST:{   //--------------------------------------------------------------------------
//                                                        //DIS.ScreenB = DIGITAL;
//                                                        //DIS.ScreenS = p2_9TEST; DIS.Point = 4; DIS.SignR = 0;
//                                                        
//                                                        DIS.SignR = 0; 
//                                                        DIS.ScreenB = DIGITAL;
//                                                        DIS.Point = 2;  DIS.SignR |= MODE_PERC; 
//                                                        switch( SYS.bit.Step )
//                                                        {
//                                                            case STP_0 :  DIS.ScreenS = p2_9TEST; if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; DIS.SignR &= ~MODE_PERC;DIS.Point = 4; DIS.SignL &= ~MODE_; break;//选择阶段
//                                                            case STP_1 :  DIS.ScreenS = SET_10_1; break;
//                                                            case STP_2 :  DIS.ScreenS = SET_10_2; break;
//                                                            case STP_3 :  DIS.ScreenS = SET_10_3; break;
//                                                            case STP_4 :  DIS.ScreenS = SET_10_4; break;
//                                                            case STP_5 :  DIS.ScreenS = SET_10_5; break;
//                                                            case STP_6 :  DIS.ScreenS = SET_10_6; break;
//                                                            case STP_7 :  DIS.ScreenS = SET_10_7; break;
//                                                            case STP_8 :  DIS.ScreenS = SET_10_8; break;
//                                                            case STP_9 :  DIS.ScreenS = SET_10_9; break;
//                                                            case SP_10 :  DIS.ScreenS = SET10_10; break;
//                                                            case SP_11 :  DIS.ScreenS = SET10_11; break;
//                                                            case SP_12 :  DIS.ScreenS = SET10_12; break;
//                                                            case SP_13 :  DIS.ScreenS = SET10_13; break;
//                                                            case SP_14 :  DIS.ScreenS = SET10_14; break;
//                                                            case SP_15 :  DIS.ScreenS = SET10_15; break;
//                                                            case SP_16 :  DIS.ScreenS = SET10_16; break;
//                                                            case SP_17 :  DIS.ScreenS = SET10_17; break;
//                                                            case SP_18 :  DIS.ScreenB = COMP; DIS.SignR &= ~MODE_PERC;DIS.Point = 4; DIS.SignL &= ~MODE_; break; //运行阶段
//                                                            case SP_19 :  DIS.ScreenB = SAVE; DIS.SignR &= ~MODE_PERC;DIS.Point = 4; DIS.SignL &= ~MODE_; break; //运行阶段
//                                                            case SP_20 :  DIS.ScreenB = PASS; DIS.SignR &= ~MODE_PERC;DIS.Point = 4; DIS.SignL &= ~MODE_; break; //运行阶段
//                                                            case SP_21 :  DIS.ScreenB = FAIL; DIS.SignR &= ~MODE_PERC;DIS.Point = 4; DIS.SignL &= ~MODE_; break; //运行阶段
//                                                            case SP_OK :  DIS.ScreenS = p2_9TEST; DIS.ScreenB = M__OK; DIS.SignR &= ~MODE_PERC;DIS.Point = 4; break;
//                                                            default:;break;
//                                                        }                           
//                                                     } break;               
                                       case P2_10SAV: { //--------------------------------------------------------------------------
                                                      
                                                        DIS.ScreenS = p2_10SAV; DIS.SignR = 0; DIS.SignL &= ~MODE_;
                                                        switch( SYS.bit.Step )
                                                        {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = REDY;  break; //运行阶段
                                                            case STP_2 :  DIS.ScreenB = RUN;   break; //运行阶段
                                                            case STP_3 :  DIS.ScreenB = SAVE;  break; //运行阶段
                                                            case STP_4 :  DIS.ScreenB = COMP;  break; //运行阶段
                                                            case STP_5 :  DIS.ScreenB = CACU;  break; //运行阶段
                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
                                                            default:;break;
                                                        }                                       
                                                   } break;                 
                                       default:break;                             
                                  } 
                            }break;              
                            case PG3 :
                            {
                                  switch ( SYS.bit.PageS2 )
                                  {
                                      case P3_4_NULL: {    //--------------------------------------------------------------------------
                                                        DIS.ScreenS = CONFIG_D;  DIS.ScreenB = P3_4; DIS.SignL &= ~MODE_HAND;  if ( CLK.SecCONFIRM == 0 ) { DIS.Point = 1; }
                                                     }break;
                                      case P3_1_MIN:  {   //--------------------------------------------------------------------------
                                                        DIS.ScreenS = p3_1_MIN;  DIS.Point = 2; 
                                                        switch( SYS.bit.Step )
                                                        {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO;DIS.SignR &= ~MODE_PERC;  break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = REDY; break;
                                                            case STP_2 :  DIS.ScreenB = DIGITAL;  DIS.SignR |= MODE_PERC;     break;
                                                            case STP_3 :  DIS.ScreenB = SAVE;     DIS.SignR &= ~MODE_PERC;    break;
                                                            case STP_4 :  DIS.ScreenB = DIGITAL;  break;
                                                            case STP_5 :  DIS.ScreenB = _S5_;     break;//DIS.ScreenB = DIGITAL;  DIS.SignR |= MODE_PERC; break;
                                                            case SP_OK :  DIS.ScreenB = M__OK;    break;
                                                            default:  break;
                                                        }
                                                        if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_HAND; }          //显示组态标志显示
                                                        else                       { DIS.SignL |= MODE_HAND;  }
                                                        //DIS.ScreenB = DIGITAL;
                                                        //DIS.ScreenS = p3_1_MIN; DIS.SignR |= MODE_PERC;  DIS.Point = 2; 
                                                      } break; 
                                      case P3_2_MAX:  {   //--------------------------------------------------------------------------
                                                         DIS.ScreenS = p3_2_MAX;    DIS.Point = 2;// DIS.SignR |= MODE_PERC;
                                                         switch( SYS.bit.Step )
                                                         {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO;DIS.SignR &= ~MODE_PERC;  break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = REDY;     break;
                                                            case STP_2 :  DIS.ScreenB = DIGITAL;  DIS.SignR |= MODE_PERC; break;
                                                            case STP_3 :  DIS.ScreenB = SAVE;     DIS.SignR &= ~MODE_PERC; break;
                                                            case STP_4 :  DIS.ScreenB = DIGITAL;  break;
                                                            case STP_5 :  DIS.ScreenB = _S5_;     break;//DIS.ScreenB = DIGITAL;  DIS.SignR |= MODE_PERC; break;
                                                            case SP_OK :  DIS.ScreenB = M__OK;    break;
                                                           default:  break;
                                                         } 
                                                         if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_HAND; }          //显示组态标志显示
                                                         else                       { DIS.SignL |= MODE_HAND;  }
                                                        //DIS.ScreenB = DIGITAL; 
                                                        //DIS.ScreenS = p3_2_MAX; DIS.SignR |= MODE_PERC;  DIS.Point = 2; 
                                                      }break; 
                                      case P3_3_RCT:  {   //--------------------------------------------------------------------------   
                                                      
                                                         DIS.ScreenS = p3_3_RCT; DIS.SignR = 0; DIS.SignL &= ~MODE_HAND;
                                                         switch( KB.bit.CountKB%2 )
                                                         {
                                                             case 0 : DIS.ScreenB = CLOK;  break;
                                                             case 1 : DIS.ScreenB = CTCL;  break;                                                      
                                                             default:  break;
                                                         }                
                                                      }break;
                                      case P3_4SAVE: { //--------------------------------------------------------------------------
                                                      
                                                        DIS.ScreenS = p3_4SAVE; DIS.SignR = 0;
                                                        switch( SYS.bit.Step )
                                                        {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = REDY;  break; //运行阶段
                                                            case STP_2 :  DIS.ScreenB = RUN;   break; //运行阶段
                                                            case STP_3 :  DIS.ScreenB = SAVE;  break; //运行阶段
                                                            case STP_4 :  DIS.ScreenB = COMP;  break; //运行阶段
                                                            case STP_5 :  DIS.ScreenB = CACU;  break; //运行阶段
                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
                                                            default:;break;
                                                        }                                       
                                                   } break;                       
                                      default:break;                             
                                   }  
                            }break;              
                            case PG4 : 
                            {     
                                  
                                   switch ( SYS.bit.PageS2 )
                                   {
                                      case P4_4_NULL:{//--------------------------------------------------------------------------
                                                       
                                                        DIS.ScreenS = CONFIG_D;  DIS.ScreenB = P4_4;  if ( CLK.SecCONFIRM == 0 ) { DIS.Point = 1; }
                                                     } break;   
                                      case P4_1_MIN: {   //--------------------------------------------------------------------------
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p4_1_MIN; DIS.SignR |= MODE_PERC;  DIS.Point = 2;
                                                     }  break;  
                                      case P4_2_MAX: {   //--------------------------------------------------------------------------
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p4_2_MAX; DIS.SignR |= MODE_PERC;  DIS.Point = 2; 
                                                     } break;   
                                      case P4_3_VRL: {   //--------------------------------------------------------------------------   
                                                      
                                                         DIS.ScreenS = p4_3_VRL;  DIS.SignR = 0;
                                                         switch( KB.bit.CountKB%2 )
                                                         {
                                                             case 0 : DIS.ScreenB = OFF;   break;
                                                             case 1 : DIS.ScreenB = OPEN;  break;                                                      
                                                             default:  break;
                                                         }
                                                     } break;
                                       case P4_4SAVE: { //--------------------------------------------------------------------------
                                                      
                                                        DIS.ScreenS = p4_4SAVE; DIS.SignR = 0;
                                                        switch( SYS.bit.Step )
                                                        {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = REDY;  break; //运行阶段
                                                            case STP_2 :  DIS.ScreenB = RUN;   break; //运行阶段
                                                            case STP_3 :  DIS.ScreenB = SAVE;  break; //运行阶段
                                                            case STP_4 :  DIS.ScreenB = COMP;  break; //运行阶段
                                                            case STP_5 :  DIS.ScreenB = CACU;  break; //运行阶段
                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
                                                            default:;break;
                                                        }                                       
                                                   } break;                      
                                      default:break;                             
                                   } 
                            }break;                            
                            case PG5 :
                            {
                                   switch ( SYS.bit.PageS2 )
                                   {
                                       case P5_12_NULL:{//--------------------------------------------------------------------------
                                                        
                                                        DIS.ScreenS = CONFIG_D; DIS.SignR &= ~MODE_PERC;  DIS.ScreenB = P5_12; DIS.SignL &= ~MODE_AUTO; if ( CLK.SecCONFIRM == 0 ) { DIS.Point = 1; }
                                                      }break;
                                       case P5_1FU: {   //--------------------------------------------------------------------------
                                                        //DIS.ScreenB = DIGITAL; 
                                                        //DIS.ScreenS = p5_1__K3; 
                                                         DIS.ScreenS = ALARM_S;    // 利用报警模式来切换小屏幕的闪烁显示
                                                         DIS.SignR = 0;  DIS.Point = 4; // DIS.ScreenS = p5_1FU;
                                                         DIS.Small_Dis[4] = 38; 
                                                         DIS.Small_Dis[5] = small_sting[p5_1FU][5];
                                                         DIS.Small_Dis[6] = small_sting[p5_1FU][6]; 
                                                         DIS.Small_Dis[7] = small_sting[p5_1FU][7];
                                                         DIS.ScreenB = DIGITAL;
                                                         DIS.SignR |= MODE_PERC; 
                                                         /*
                                                         switch( SYS.bit.Step )
                                                         {
                                                            case STP_1 :  DIS.ScreenB = DIGITAL;  break;
                                                            case STP_2 :  DIS.ScreenB = _S2_;     break;
                                                            case STP_5 :  DIS.ScreenB = _S3_;     break;//DIS.ScreenB = DIGITAL;  DIS.SignR |= MODE_PERC; break;
                                                            case SP_OK :  DIS.ScreenB = M__OK;    break;
                                                           default:  break;
                                                         }
                                                         */
                                                         if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
                                                         else                       { DIS.SignL |= MODE_AUTO;  }
                                                      } break;  
                                       case P5_2FD: {   //--------------------------------------------------------------------------
                                                        //DIS.ScreenB = DIGITAL;
                                                        //DIS.ScreenS = p5_2__K0; 
                                                         DIS.ScreenS = ALARM_S; 
                                                         //DIS.ScreenS = p5_2FD;  
                                                         DIS.SignR = 0;  DIS.Point = 4; 
                                                         DIS.Small_Dis[4] = 38; 
                                                         DIS.Small_Dis[5] = small_sting[p5_2FD][5];
                                                         DIS.Small_Dis[6] = small_sting[p5_2FD][6]; 
                                                         DIS.Small_Dis[7] = small_sting[p5_2FD][7];
                                                         DIS.ScreenB = DIGITAL;
                                                         DIS.SignR |= MODE_PERC; 
                                                         /*
                                                         switch( SYS.bit.Step )
                                                         {
                                                            case STP_1 :  DIS.ScreenB = DIGITAL;  break;
                                                            case STP_2 :  DIS.ScreenB = _S2_;     break;
                                                            case STP_5 :  DIS.ScreenB = _S3_;     break;//DIS.ScreenB = DIGITAL;  DIS.SignR |= MODE_PERC; break;
                                                            case SP_OK :  DIS.ScreenB = M__OK;    break;
                                                           default:  break;
                                                         }
                                                         */
                                                         if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
                                                         else                       { DIS.SignL |= MODE_AUTO;  }
                                                      } break;  
                                       case P5_3FP: {   //--------------------------------------------------------------------------
                                                        //DIS.ScreenB = DIGITAL;
                                                        //DIS.ScreenS = p5_3__K1;
                                                         DIS.ScreenS = ALARM_S; 
                                                         //DIS.ScreenS = p5_3FP;  
                                                         DIS.SignR = 0;  DIS.Point = 4;
                                                         DIS.Small_Dis[4] = 38; 
                                                         DIS.Small_Dis[5] = small_sting[p5_3FP][5];
                                                         DIS.Small_Dis[6] = small_sting[p5_3FP][6]; 
                                                         DIS.Small_Dis[7] = small_sting[p5_3FP][7];
                                                         DIS.ScreenB = DIGITAL;
                                                         DIS.SignR |= MODE_PERC; 
                                                         /*
                                                         switch( SYS.bit.Step )
                                                         {
                                                            case STP_1 :  DIS.ScreenB = DIGITAL;  break;
                                                            case STP_2 :  DIS.ScreenB = _S2_;     break;
                                                            case STP_5 :  DIS.ScreenB = _S3_;     break;//DIS.ScreenB = DIGITAL;  DIS.SignR |= MODE_PERC; break;
                                                            case SP_OK :  DIS.ScreenB = M__OK;    break;
                                                           default:  break;
                                                         }
                                                         */
                                                         if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
                                                         else                       { DIS.SignL |= MODE_AUTO;  }
                                                      } break;  
                                       case P5_4FB: {   //--------------------------------------------------------------------------
                                                        //DIS.ScreenB = DIGITAL;
                                                        //DIS.ScreenS = p5_4__K0;
                                                         DIS.ScreenS = ALARM_S; 
                                                         //DIS.ScreenS = p5_4FB; 
                                                         DIS.SignR = 0;  DIS.Point = 4; 
                                                         DIS.SignR |= MODE_PERC; 
                                                         DIS.Small_Dis[4] = 38; 
                                                         DIS.Small_Dis[5] = small_sting[p5_4FB][5];
                                                         DIS.Small_Dis[6] = small_sting[p5_4FB][6]; 
                                                         DIS.Small_Dis[7] = small_sting[p5_4FB][7];
                                                         DIS.ScreenB = DIGITAL;
                                                         /*
                                                         switch( SYS.bit.Step )
                                                         {
                                                            case STP_1 :  DIS.ScreenB = DIGITAL;  break;
                                                            case STP_2 :  DIS.ScreenB = _S2_;     break;
                                                            case STP_5 :  DIS.ScreenB = _S3_;     break;//DIS.ScreenB = DIGITAL;  DIS.SignR |= MODE_PERC; break;
                                                            case SP_OK :  DIS.ScreenB = M__OK;    break;
                                                           default:  break;
                                                         }
                                                         */
                                                         if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
                                                         else                       { DIS.SignL |= MODE_AUTO;  }
                                                      } break;    
//                                       case P5_5BT: {   //--------------------------------------------------------------------------
//                                                        //DIS.ScreenB = DIGITAL;
//                                                        //DIS.ScreenS = p5_5__K1;
//                                                         DIS.ScreenS = ALARM_S; 
//                                                         //DIS.ScreenS = p5_5BT;  
//                                                         DIS.SignR = 0;  DIS.Point = 4;
//                                                         DIS.Small_Dis[4] = 38; 
//                                                         DIS.Small_Dis[5] = small_sting[p5_5BT][5];
//                                                         DIS.Small_Dis[6] = small_sting[p5_5BT][6]; 
//                                                         DIS.Small_Dis[7] = small_sting[p5_5BT][7];
//                                                         DIS.ScreenB = DIGITAL;
//                                                         DIS.SignR |= MODE_PERC; 
//                                                         /*
//                                                         switch( SYS.bit.Step )
//                                                         {
//                                                            case STP_1 :  DIS.ScreenB = DIGITAL;  break;
//                                                            case STP_2 :  DIS.ScreenB = _S2_;     break;
//                                                            case STP_5 :  DIS.ScreenB = _S3_;     break;//DIS.ScreenB = DIGITAL;  DIS.SignR |= MODE_PERC; break;
//                                                            case SP_OK :  DIS.ScreenB = M__OK;    break;
//                                                           default:  break;
//                                                         }
//                                                         */
//                                                         if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
//                                                         else                       { DIS.SignL |= MODE_AUTO;  }
//                                                      } break;  
//                                       case P5_6SU: {   //--------------------------------------------------------------------------
//                                                         //DIS.ScreenB = DIGITAL;
//                                                         //DIS.ScreenS = p5_6__K0;
//                                                         DIS.ScreenS = ALARM_S; 
//                                                         //DIS.ScreenS = p5_6SU;  
//                                                         DIS.SignR = 0;  DIS.Point = 4;
//                                                         DIS.Small_Dis[4] = 38; 
//                                                         DIS.Small_Dis[5] = small_sting[p5_6SU][5];
//                                                         DIS.Small_Dis[6] = small_sting[p5_6SU][6]; 
//                                                         DIS.Small_Dis[7] = small_sting[p5_6SU][7];
//                                                         DIS.ScreenB = DIGITAL;
//                                                         DIS.SignR |= MODE_PERC; 
//                                                         /*
//                                                         switch( SYS.bit.Step )
//                                                         {
//                                                            case STP_1 :  DIS.ScreenB = DIGITAL;  break;
//                                                            case STP_2 :  DIS.ScreenB = _S2_;     break;
//                                                            case STP_5 :  DIS.ScreenB = _S3_;     break;//DIS.ScreenB = DIGITAL;  DIS.SignR |= MODE_PERC; break;
//                                                            case SP_OK :  DIS.ScreenB = M__OK;    break;
//                                                           default:  break;
//                                                         }
//                                                         */
//                                                         if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
//                                                         else                       { DIS.SignL |= MODE_AUTO;  }
//                                                      } break;  
//                                       case P5_7SD: {   //--------------------------------------------------------------------------
//                                                         //DIS.ScreenB = DIGITAL;
//                                                         //DIS.ScreenS = p5_7__K3;
//                                                         DIS.ScreenS = ALARM_S; 
//                                                         //DIS.ScreenS = p5_7SD;  
//                                                         DIS.SignR = 0;  DIS.Point = 4;
//                                                         DIS.Small_Dis[4] = 38; 
//                                                         DIS.Small_Dis[5] = small_sting[p5_7SD][5];
//                                                         DIS.Small_Dis[6] = small_sting[p5_7SD][6]; 
//                                                         DIS.Small_Dis[7] = small_sting[p5_7SD][7];
//                                                         DIS.ScreenB = DIGITAL;
//                                                         DIS.SignR |= MODE_PERC; 
//                                                         /*
//                                                         switch( SYS.bit.Step )
//                                                         {
//                                                            case STP_1 :  DIS.ScreenB = DIGITAL;  break;
//                                                            case STP_2 :  DIS.ScreenB = _S2_;     break;
//                                                            case STP_5 :  DIS.ScreenB = _S3_;     break;//DIS.ScreenB = DIGITAL;  DIS.SignR |= MODE_PERC; break;
//                                                            case SP_OK :  DIS.ScreenB = M__OK;    break;
//                                                           default:  break;
//                                                         }
//                                                         */
//                                                         if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
//                                                         else                       { DIS.SignL |= MODE_AUTO;  }
//                                                      } break; 
//                                       case P5_8TF: {   //--------------------------------------------------------------------------
//                                                        DIS.ScreenS = ALARM_S; 
//                                                        //DIS.ScreenS = p5_8TF;  
//                                                        DIS.SignR = 0;  DIS.Point = 4; 
//                                                        DIS.Small_Dis[4] = 38; 
//                                                        DIS.Small_Dis[5] = small_sting[p5_8TF][5];
//                                                        DIS.Small_Dis[6] = small_sting[p5_8TF][6]; 
//                                                        DIS.Small_Dis[7] = small_sting[p5_8TF][7];
//                                                        DIS.ScreenB = DIGITAL;
//                                                        DIS.SignR |= MODE_PERC; 
//                                                        /*
//                                                        switch( SYS.bit.Step )
//                                                        {
//                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
//                                                            case STP_1 :  DIS.ScreenB = RUN;   break; //运行阶段
//                                                            case STP_2 :  DIS.ScreenB = _S1_;  break;
//                                                            case STP_3 :  DIS.ScreenB = _S2_;  break;
//                                                            case STP_4 :  DIS.ScreenB = _S3_;  break;
//                                                            case STP_5 :  DIS.ScreenB = _S4_;  break;
//                                                            case STP_6 :  DIS.ScreenB = _S4_;  break;
//                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
//                                                            default:  break;
//                                                        }
//                                                        */
//                                                        DIS.ScreenB = DIGITAL;
//                                                        if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
//                                                        else                       { DIS.SignL |= MODE_AUTO;  }
//                                                      }break;
                                        case P5_9TB: {   //--------------------------------------------------------------------------
                                                        DIS.ScreenS = ALARM_S; 
                                                        //DIS.ScreenS = p5_9TB;  
                                                        DIS.SignR = 0;DIS.Point = 4;
                                                        DIS.Small_Dis[4] = 38; 
                                                        DIS.Small_Dis[5] = small_sting[p5_9TB][5];
                                                        DIS.Small_Dis[6] = small_sting[p5_9TB][6]; 
                                                        DIS.Small_Dis[7] = small_sting[p5_9TB][7];
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.SignR |= MODE_PERC; 
                                                        /*
                                                        switch( SYS.bit.Step )
                                                        {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = RUN;   break; //运行阶段
                                                            case STP_2 :  DIS.ScreenB = _S1_;  break;
                                                            case STP_3 :  DIS.ScreenB = _S2_;  break;
                                                            case STP_4 :  DIS.ScreenB = _S3_;  break;
                                                            case STP_5 :  DIS.ScreenB = _S4_;  break;
                                                            case STP_6 :  DIS.ScreenB = _S4_;  break;
                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
                                                            default:  break;
                                                        }
                                                        */
                                                         if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
                                                         else                       { DIS.SignL |= MODE_AUTO;  }
                                                      }break; 
                                         case P5_10T: {   //--------------------------------------------------------------------------
                                                        DIS.ScreenS = ALARM_S; 
                                                        //DIS.ScreenS = p5_10T;  
                                                        DIS.SignR = 0; DIS.Point = 4;
                                                        DIS.Small_Dis[4] = 38; 
                                                        DIS.Small_Dis[5] = small_sting[p5_10T][5];
                                                        DIS.Small_Dis[6] = small_sting[p5_10T][6]; 
                                                        DIS.Small_Dis[7] = small_sting[p5_10T][7];
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.SignR |= MODE_PERC; 
                                                        /*
                                                        switch( SYS.bit.Step )
                                                        {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = RUN;   break; //运行阶段
                                                            case STP_2 :  DIS.ScreenB = _S1_;  break;
                                                            case STP_3 :  DIS.ScreenB = _S2_;  break;
                                                            case STP_4 :  DIS.ScreenB = _S3_;  break;
                                                            case STP_5 :  DIS.ScreenB = _S4_;  break;
                                                            case STP_6 :  DIS.ScreenB = _S4_;  break;
                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
                                                            default:  break;
                                                        }
                                                        */
                                                         if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
                                                         else                       { DIS.SignL |= MODE_AUTO;  }
                                                      }break; 
                                          case P5_11RST: {   //--------------------------------------------------------------------------
                                                       
                                                        DIS.ScreenS = p5_11RST;  DIS.SignR = 0; DIS.SignL &= ~MODE_AUTO; DIS.SignR &= ~MODE_PERC;
                                                        switch( SYS.bit.Step )
                                                        {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = RUN;   break; //运行阶段
                                                            case STP_2 :  DIS.ScreenB = _S1_;  break;
                                                            case STP_3 :  DIS.ScreenB = _S2_;  break;
                                                            case STP_4 :  DIS.ScreenB = _S3_;  break;
                                                            case STP_5 :  DIS.ScreenB = _S4_;  break;
                                                            case STP_6 :  DIS.ScreenB = _S4_;  break;
                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
                                                            default:  break;
                                                        }                                
                                                      }break;
                                           case P5_12SAV: {   //--------------------------------------------------------------------------
                                                       
                                                        DIS.ScreenS = p5_12SAV;  DIS.SignR = 0; 
                                                        switch( SYS.bit.Step )
                                                        {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = REDY;  break; //运行阶段
                                                            case STP_2 :  DIS.ScreenB = RUN;   break; //运行阶段
                                                            case STP_3 :  DIS.ScreenB = SAVE;  break; //运行阶段
                                                            case STP_4 :  DIS.ScreenB = COMP;  break; //运行阶段
                                                            case STP_5 :  DIS.ScreenB = CACU;  break; //运行阶段
                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
                                                            default:;break;
                                                        }                                
                                                      }break;             
                                                     
                                       default:break;                             
                                   }   
                            }break;               
                            case PG6 :
                            { 
                                   
                                    switch ( SYS.bit.PageS2 )
                                    {
                                        case  P6_8_NULL:{//--------------------------------------------------------------------------
                                                        
                                                          DIS.ScreenS = CONFIG_D;  DIS.ScreenB = P6_6;  if ( CLK.SecCONFIRM == 0 ) { DIS.Point = 1; }
                                                        }break;
//                                        case  P6_1__LK :{   //--------------------------------------------------------------------------   
//                                                            DIS.SignR = 0;
//                                                            DIS.ScreenS = p6_1__LK;
//                                                            switch( KB.bit.CountKB%2 )
//                                                            {
//                                                               case 0 : DIS.ScreenB = NORC;   break;
//                                                               case 1 : DIS.ScreenB = ACTV;   break;                                                      
//                                                               default:  break;
//                                                            }
//                                                        }break;  
//                                        case  P6_2__SV :{   //--------------------------------------------------------------------------   
//                                                            DIS.SignR = 0;
//                                                            DIS.ScreenS = p6_2__SV;
//                                                            switch( KB.bit.CountKB%2 )
//                                                            {
//                                                               case 0 : DIS.ScreenB = NORC;   break;
//                                                               case 1 : DIS.ScreenB = ACTV;   break;                                                      
//                                                               default:  break;
//                                                            }
//                                                        }break;    
//                                        case  P6_3__ZO :{   //--------------------------------------------------------------------------   
//                                                            DIS.SignR = 0; 
//                                                            DIS.ScreenS = p6_3__ZO;
//                                                            switch( KB.bit.CountKB%2 )
//                                                            {
//                                                               case 0 : DIS.ScreenB = NORC;   break;
//                                                               case 1 : DIS.ScreenB = ACTV;   break;                                                      
//                                                               default:  break;
//                                                            }
//                                                        }break; 
                                       case P6_1__LK:{   //--------------------------------------------------------------------------
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p6_1__LK; 
                                                        DIS.Point = 4; 
                                                        DIS.SignR = 0;
                                                     } break;                                                        
                                       case P6_2__SV:{   //--------------------------------------------------------------------------
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p6_2__SV; 
                                                        DIS.Point = 4; 
                                                        DIS.SignR = 0;
                                                     } break; 
                                       case P6_3__ZO:{   //--------------------------------------------------------------------------
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p6_3__ZO; 
                                                        DIS.Point = 4; 
                                                        DIS.SignR = 0;
                                                     } break;                                                      
                                        case  P6_4__VR :{   //--------------------------------------------------------------------------   
                                                            DIS.SignR = 0;
                                                            DIS.ScreenS = p6_4__VR;
                                                            switch( KB.bit.CountKB%2 )
                                                            {
                                                               case 0 : DIS.ScreenB = VL;   break;
                                                               case 1 : DIS.ScreenB = VH;   break;                                                      
                                                               default:  break;
                                                            }
                                                        }break;   
                                                        
                                        case P6_5__TO:{   //--------------------------------------------------------------------------
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p6_5__TO; 
                                                        DIS.Point = 4; 
                                                        DIS.SignR = 0;
                                                     } break; 
                                        case P6_6_MIN:{   //--------------------------------------------------------------------------
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p6_6_MIN; 
                                                        DIS.Point = 4; 
                                                        DIS.SignR = 0;
                                                     } break; 
                                        case P6_7_MAX:{   //--------------------------------------------------------------------------
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p6_7_MAX; 
                                                        DIS.Point = 4; 
                                                        DIS.SignR = 0;
                                                     } break;                                                       
//                                        case  P6_5__TO :{   //--------------------------------------------------------------------------   
//                                                            DIS.SignR = 0;
//                                                            DIS.ScreenS = p6_5__TO;
//                                                            switch( KB.bit.CountKB%2 )
//                                                            {
//                                                               case 0 : DIS.ScreenB = NORC;   break;
//                                                               case 1 : DIS.ScreenB = ACTV;   break;                                                      
//                                                               default:  break;
//                                                            }
//                                                        }break;
//                                        case  P6_6_MIN :{   //--------------------------------------------------------------------------   
//                                                            DIS.ScreenB = DIGITAL;
//                                                            DIS.ScreenS = p6_6_MIN; DIS.SignR |= MODE_mA;   DIS.Point = 2;
//                                                        }break;
//                                        case  P6_7_MAX :{   //--------------------------------------------------------------------------   
//                                                            DIS.ScreenB = DIGITAL;
//                                                            DIS.ScreenS = p6_7_MAX; DIS.SignR |= MODE_mA;   DIS.Point = 2;
//                                                        }break;                
                                        case P6_8SAVE: { //--------------------------------------------------------------------------
                                                      
                                                        DIS.ScreenS = p6_8SAVE; DIS.SignR = 0;
                                                        switch( SYS.bit.Step )
                                                        {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = REDY;  break; //运行阶段
                                                            case STP_2 :  DIS.ScreenB = RUN;   break; //运行阶段
                                                            case STP_3 :  DIS.ScreenB = SAVE;  break; //运行阶段
                                                            case STP_4 :  DIS.ScreenB = COMP;  break; //运行阶段
                                                            case STP_5 :  DIS.ScreenB = CACU;  break; //运行阶段
                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
                                                            default:;break;
                                                        }                                       
                                                   } break;                                            
                                        default:break;                             
                                    } 
                            }break;           
                            case PG7 :
                            {                             
                                  switch ( SYS.bit.PageS2 )
                                  {
                                     case P7_5_NULL: {//--------------------------------------------------------------------------
                                                      
                                                        DIS.ScreenS = CONFIG_D;  DIS.ScreenB = P7_5; if ( CLK.SecCONFIRM == 0 ) { DIS.Point = 1; }  
                                                     }break;
//                                     case P7_1_MIN : {   //--------------------------------------------------------------------------
//                                                       DIS.ScreenS = p7_1_MIN; DIS.ScreenB = DIGITAL;
//                                                        DIS.SignR |= MODE_mA; DIS.Point = 2;
//                                                     }break; 
//                                     case P7_2_MAX : {   //--------------------------------------------------------------------------
//                                                       DIS.ScreenS = p7_2_MAX; DIS.ScreenB = DIGITAL;
//                                                        DIS.SignR |= MODE_mA; DIS.Point = 2;
//                                                     } break;  
                                     case P7_3_ACT : {     //--------------------------------------------------------------------------   
                                                            DIS.ScreenS = p7_3_ACT;  DIS.SignR = 0;
                                                            switch( KB.bit.CountKB%2 )
                                                            {
                                                               case 0 : DIS.ScreenB = DIRC; break;
                                                               case 1 : DIS.ScreenB = REVR; break;                                                      
                                                               default:  break;
                                                            }
                                                    }break;       
//                                     case P7_4TEST : {   //--------------------------------------------------------------------------
//                                                     
//                                                        DIS.ScreenS = p7_4TEST;  DIS.SignR = 0;
//                                                        switch( SYS.bit.Step )
//                                                        {
//                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
//                                                            case STP_1 :  DIS.ScreenB = RUN;   break; //运行阶段
//                                                            case STP_2 :  DIS.ScreenB = _S1_;  break;
//                                                            case STP_3 :  DIS.ScreenB = _S2_;  break;
//                                                            case STP_4 :  DIS.ScreenB = _S3_;  break;
//                                                            case STP_5 :  DIS.ScreenB = _S4_;  break;
//                                                            case STP_6 :  DIS.ScreenB = _S4_;  break;
//                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
//                                                            default:  break;
//                                                        }                                
//                                                     } break; 
                                      case P7_5SAVE: { //--------------------------------------------------------------------------
                                                      
                                                        DIS.ScreenS = p7_5SAVE; DIS.SignR = 0;
                                                        switch( SYS.bit.Step )
                                                        {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = REDY;  break; //运行阶段
                                                            case STP_2 :  DIS.ScreenB = RUN;   break; //运行阶段
                                                            case STP_3 :  DIS.ScreenB = SAVE;  break; //运行阶段
                                                            case STP_4 :  DIS.ScreenB = COMP;  break; //运行阶段
                                                            case STP_5 :  DIS.ScreenB = CACU;  break; //运行阶段
                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
                                                            default:;break;
                                                        }                                       
                                                   } break;                          
                                     default:break;                             
                                  }
                            }break;       
                            case PG8 :
                            {      
                                  
                                   switch ( SYS.bit.PageS2 )
                                   {
                                        case P8_8_NULL:{//--------------------------------------------------------------------------
                                                       
                                                         DIS.ScreenS = CONFIG_D;  DIS.ScreenB = P8_8; DIS.SignL &= ~MODE_AUTO;  if ( CLK.SecCONFIRM == 0 ) { DIS.Point = 1; }
                                                       }break;
                                        case P8_1_TRG: {   //--------------------------------------------------------------------------
                                                         DIS.ScreenB = DIGITAL;
                                                         DIS.ScreenS = p8_1_TRG; DIS.Point = 4; DIS.SignR = 0;
                                                       } break; 
                                        case P8_2_POS: {  //--------------------------------------------------------------------------   
                                                          DIS.SignR = 0;
                                                          DIS.ScreenS = p8_2_POS;
                                                          switch( KB.bit.CountKB%2 )
                                                          {
//                                                             case 0 : DIS.ScreenB = KEEP;  break;
                                                             case 0 : DIS.ScreenB = FCO;  break;
                                                             case 1 : DIS.ScreenB = FL;  break;  
//                                                             case 2 : DIS.ScreenB = M_OUT; break; 
                                                             default:  break;
                                                          }
                                                       }break;  
                                        case P8_3_FRC:{   //--------------------------------------------------------------------------
                                                          DIS.SignR = 0; DIS.SignL &= ~MODE_AUTO;
                                                          DIS.ScreenS = p8_3_FRC;
                                                          switch( SYS.bit.Step )
                                                          {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = RUN;   break; //运行阶段
                                                            case STP_2 :  DIS.ScreenB = _S1_;  break;
                                                            case STP_3 :  DIS.ScreenB = _S2_;  break;
                                                            case STP_4 :  DIS.ScreenB = _S3_;  break;
                                                            case STP_5 :  DIS.ScreenB = _S4_;  break;
                                                            case STP_6 :  DIS.ScreenB = _S4_;  break;
                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
                                                            default:  break;
                                                          }                                      
                                                       }break;
                                        case P8_4_MIN: {   //--------------------------------------------------------------------------
                                                          DIS.SignR = 0;
                                                          DIS.ScreenS = p8_4_MIN;
                                                          switch( SYS.bit.Step )
                                                          {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = RUN;      break; //运行阶段
                                                            case STP_2 :  DIS.ScreenB = DIGITAL;  break;
                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
                                                            default:  break;
                                                          }  
                                                          if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
                                                          else                       { DIS.SignL |= MODE_AUTO;  }
                                                       }break;
                                        case P8_5_MAX :{   //-------------------------------------------------------------------------
                                                          DIS.SignR = 0;
                                                          DIS.ScreenS = p8_5_MAX;
                                                          switch( SYS.bit.Step )
                                                          {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = RUN;      break; //运行阶段
                                                            case STP_2 :  DIS.ScreenB = DIGITAL;  break;
                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
                                                            default:  break;
                                                          }
                                                          if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
                                                          else                       { DIS.SignL |= MODE_AUTO;  }
                                                       }break;
//                                        case P8_6_POP: { //--------------------------------------------------------------------------
//                                                           DIS.ScreenS = p8_6_POP;  DIS.SignR = 0; DIS.SignL &= ~MODE_AUTO;
//                                                            switch( KB.bit.CountKB%5 )
//                                                            {
//                                                               case 0 : DIS.ScreenB = AT_R; break;
//                                                               case 1 : DIS.ScreenB = AT_P; break; 
//                                                               case 2 : DIS.ScreenB = AT_M; break;   
//                                                               case 3 : DIS.ScreenB = MA_P; break;   
//                                                               case 4 : DIS.ScreenB = MA_S; break;   
//                                                               default:  break;
//                                                            }                          
//                                                   } break;           
//                                        case P8_7_RDJ: { //--------------------------------------------------------------------------
//                                                      
//                                                        DIS.ScreenS = p8_7_RDJ;  DIS.SignR = 0;
//                                                        switch( KB.bit.CountKB%5 )
//                                                        {
//                                                               case 0 : DIS.ScreenB = SCLE; break;
//                                                               case 1 : DIS.ScreenB = MIDD; break;  
//                                                               case 2 : DIS.ScreenB = MISP; break; 
//                                                               case 3 : DIS.ScreenB = TINY; break;   
//                                                               case 4 : DIS.ScreenB = ALLL; break;   
//                                                               default:  break;
//                                                        }                                                         
//                                                   } break;           
                                        case P8_8SAVE: { //--------------------------------------------------------------------------
                                                      
                                                        DIS.ScreenS = p8_8SAVE; DIS.SignR = 0;
                                                        switch( SYS.bit.Step )
                                                        {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = REDY;  break; //运行阶段
                                                            case STP_2 :  DIS.ScreenB = RUN;   break; //运行阶段
                                                            case STP_3 :  DIS.ScreenB = SAVE;  break; //运行阶段
                                                            case STP_4 :  DIS.ScreenB = COMP;  break; //运行阶段
                                                            case STP_5 :  DIS.ScreenB = CACU;  break; //运行阶段
                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
                                                            default:;break;
                                                        }                                       
                                                   } break;                      
                                        default:break;                             
                                   }   
                             }break; 
                             /*
                             case PG9:
                               {
                                 switch( SYS.bit.PageS2 )
                                   {
                                        case P9_24_NULL:{//--------------------------------------------------------------------------
                                                       
                                                           DIS.ScreenS = CONFIG_D;  DIS.ScreenB = P9_24; DIS.SignL &= ~MODE_AUTO;  if ( CLK.SecCONFIRM == 0 ) { DIS.Point = 1; }
                                                        }break;                                     
                                        case P9_1: { 
                                                        DIS.ScreenS = p9_1; DIS.SignR = 0;  //DIS.Point = 0; 
                                                        DIS.ScreenB = DIGITAL; 
                                                      //  switch( KB.bit.CountKB%2 )
                                                      // {   
                                                      //      case 0 : DIS.ScreenB = LINE; break; 
                                                      //      case 1 : DIS.ScreenB = ROTR; break; 
                                                       //     default:  break;
                                                      //  }break;                                                                                                
                                                   }break;
                                        case P9_2: {
                                                        DIS.ScreenB = DIGITAL;  DIS.SignL &= ~MODE_HAND;
                                                        DIS.ScreenS = p9_2; DIS.SignR =  0; //DIS.Point = 2;                                          
                                                   }break;
                                        case P9_3: {
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p9_3; DIS.SignR = 0; //  DIS.Point = 2;                                          
                                                   }break;
                                        case P9_4: {
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p9_4; DIS.SignR = 0; //  DIS.Point = 2;
                                           
                                                   }break;
                                        case P9_5: { 
                                                       // switch(  KB.bit.CountKB%6 )
                                                       // {
                                                       //    case 0 : DIS.ScreenB = LINE;  break;
                                                       //    case 1 : DIS.ScreenB = M1_25; break;
                                                       //    case 2 : DIS.ScreenB = M1_50; break;
                                                       //    case 3 : DIS.ScreenB = M25_1; break;
                                                       //    case 4 : DIS.ScreenB = M50_1; break;  
                                                       //    case 5 : DIS.ScreenB = CUST;  break; 
                                                       //    default:  break;
                                                       // }  
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p9_5; DIS.SignR = 0;   //  DIS.Point = 0;                                                   
                                                   }break;
                                        case P9_6: {
                                                     //   switch( KB.bit.CountKB%2 )
                                                     //   {
                                                     //      case 0 : DIS.ScreenB = DIRC;  break;
                                                     //      case 1 : DIS.ScreenB = REVR;  break;                                                      
                                                      //     default:  break;
                                                      //  } 
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p9_6; DIS.SignR = 0;                                          
                                                   }break;
                                        case P9_7: {
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p9_7; DIS.SignR = 0; // DIS.Point = 2;                                                       
                                                   }break;
                                        case P9_8: {
                                                      DIS.ScreenB = DIGITAL;   DIS.SignR = 0;   //DIS.Point = 0; 
                                                      DIS.ScreenS = p9_8;                                        
                                                   }break;
                                        case P9_9: {
                                                      DIS.ScreenB = DIGITAL;   DIS.SignR = 0;   //DIS.Point = 0;
                                                      DIS.ScreenS = p9_9; 
                                          
                                                   }break;
                                        case P9_10:{
                                                         DIS.ScreenS = p9_10;  DIS.SignL &= ~MODE_HAND;  //DIS.SignR = 0;
                                                        // switch( KB.bit.CountKB%2 )
                                                        // {
                                                        //     case 0 : DIS.ScreenB = CLOK;  break;
                                                        //     case 1 : DIS.ScreenB = CTCL;  break;                                                      
                                                        //     default:  break;
                                                        // }   
                                                         DIS.ScreenB = DIGITAL;
                                                   }break;
                                        case P9_11:{ 
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p9_11; DIS.SignR = 0; // DIS.Point = 2;                                          
                                                   }break;
                                        case P9_12:{
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.ScreenS = p9_12; DIS.SignR = 0; // DIS.Point = 2;                                          
                                                   }break;
                                        case P9_13:{
                                                         DIS.ScreenS = p9_13;  DIS.SignR = 0; //DIS.Point = 0;
                                                         DIS.ScreenB = DIGITAL;
                                                       //  switch( KB.bit.CountKB%2 )
                                                       //  {
                                                       //      case 0 : DIS.ScreenB = OFF;   break;
                                                       //      case 1 : DIS.ScreenB = OPEN;  break;                                                      
                                                       //      default:  break;
                                                       //  }                                          
                                                   }break;
                                                   
                                                   
                                        case P9_14: {   //--------------------------------------------------------------------------
                                                        //DIS.ScreenB = DIGITAL; 
                                                        //DIS.ScreenS = p5_1__K3; 
                                                         DIS.ScreenS = p9_14;    // 利用报警模式来切换小屏幕的闪烁显示
                                                         DIS.SignR = 0;  //DIS.Point = 4; // DIS.ScreenS = p5_1FU;
                                                         DIS.Small_Dis[4] = 38; 
                                                         DIS.Small_Dis[5] = small_sting[p5_1FU][5];
                                                         DIS.Small_Dis[6] = small_sting[p5_1FU][6]; 
                                                         DIS.Small_Dis[7] = small_sting[p5_1FU][7];
                                                         DIS.ScreenB = DIGITAL;
                                                        // DIS.SignR |= MODE_PERC; 
                                                        // if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
                                                        // else                       { DIS.SignL |= MODE_AUTO;  }
                                                      } break;  
                                       case P9_15: {   //--------------------------------------------------------------------------
                                                        //DIS.ScreenB = DIGITAL;
                                                        //DIS.ScreenS = p5_2__K0; 
                                                         DIS.ScreenS = p9_15; 
                                                         DIS.SignR = 0;  //DIS.Point = 4; 
                                                         DIS.Small_Dis[4] = 38; 
                                                         DIS.Small_Dis[5] = small_sting[p5_2FD][5];
                                                         DIS.Small_Dis[6] = small_sting[p5_2FD][6]; 
                                                         DIS.Small_Dis[7] = small_sting[p5_2FD][7];
                                                         DIS.ScreenB = DIGITAL;
                                                         //DIS.SignR |= MODE_PERC; 
                                                        // if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
                                                        // else                       { DIS.SignL |= MODE_AUTO;  }
                                                      } break;  
                                       case P9_16: {   //--------------------------------------------------------------------------
                                                        //DIS.ScreenB = DIGITAL;
                                                        //DIS.ScreenS = p5_3__K1;
                                                         DIS.ScreenS = p9_16; 
                                                         //DIS.ScreenS = p5_3FP;  
                                                         DIS.SignR = 0; // DIS.Point = 4;
                                                         DIS.Small_Dis[4] = 38; 
                                                         DIS.Small_Dis[5] = small_sting[p5_3FP][5];
                                                         DIS.Small_Dis[6] = small_sting[p5_3FP][6]; 
                                                         DIS.Small_Dis[7] = small_sting[p5_3FP][7];
                                                         DIS.ScreenB = DIGITAL;
                                                        // DIS.SignR |= MODE_PERC; 
                                                        // if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
                                                        // else                       { DIS.SignL |= MODE_AUTO;  }
                                                      } break;  
                                       case P9_17: {   //--------------------------------------------------------------------------
                                                        //DIS.ScreenB = DIGITAL;
                                                        //DIS.ScreenS = p5_4__K0;
                                                         DIS.ScreenS = p9_17; 
                                                         DIS.SignR = 0; // DIS.Point = 4; 
                                                        // DIS.SignR |= MODE_PERC; 
                                                         DIS.Small_Dis[4] = 38; 
                                                         DIS.Small_Dis[5] = small_sting[p5_4FB][5];
                                                         DIS.Small_Dis[6] = small_sting[p5_4FB][6]; 
                                                         DIS.Small_Dis[7] = small_sting[p5_4FB][7];
                                                         DIS.ScreenB = DIGITAL;
                                                        // if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
                                                        // else                       { DIS.SignL |= MODE_AUTO;  }
                                                      } break;    
                                       case P9_18: {   //--------------------------------------------------------------------------
                                                        //DIS.ScreenB = DIGITAL;
                                                        //DIS.ScreenS = p5_5__K1;
                                                         DIS.ScreenS = p9_18;  
                                                         DIS.SignR = 0; // DIS.Point = 4;
                                                         DIS.Small_Dis[4] = 38; 
                                                         DIS.Small_Dis[5] = small_sting[p5_5BT][5];
                                                         DIS.Small_Dis[6] = small_sting[p5_5BT][6]; 
                                                         DIS.Small_Dis[7] = small_sting[p5_5BT][7];
                                                         DIS.ScreenB = DIGITAL;
                                                         DIS.SignR = 0; 
                                                         //if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
                                                         //else                       { DIS.SignL |= MODE_AUTO;  }
                                                      } break;  
                                       case P9_19: {   //--------------------------------------------------------------------------
                                                         //DIS.ScreenB = DIGITAL;
                                                         //DIS.ScreenS = p5_6__K0;
                                                         DIS.ScreenS = p9_19; 
                                                         //DIS.ScreenS = p5_6SU;  
                                                         DIS.SignR = 0; // DIS.Point = 4;
                                                         DIS.Small_Dis[4] = 38; 
                                                         DIS.Small_Dis[5] = small_sting[p5_6SU][5];
                                                         DIS.Small_Dis[6] = small_sting[p5_6SU][6]; 
                                                         DIS.Small_Dis[7] = small_sting[p5_6SU][7];
                                                         DIS.ScreenB = DIGITAL;
                                                         DIS.SignR = 0; 
                                                         //if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
                                                         //else                       { DIS.SignL |= MODE_AUTO;  }
                                                      } break;  
                                       case P9_20: {   //--------------------------------------------------------------------------
                                                         //DIS.ScreenB = DIGITAL;
                                                         //DIS.ScreenS = p5_7__K3;
                                                         DIS.ScreenS = p9_20; 
                                                         //DIS.ScreenS = p5_7SD;  
                                                         DIS.SignR = 0; // DIS.Point = 4;
                                                         DIS.Small_Dis[4] = 38; 
                                                         DIS.Small_Dis[5] = small_sting[p5_7SD][5];
                                                         DIS.Small_Dis[6] = small_sting[p5_7SD][6]; 
                                                         DIS.Small_Dis[7] = small_sting[p5_7SD][7];
                                                         DIS.ScreenB = DIGITAL;
                                                         DIS.SignR = 0; 
                                                        // if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
                                                        // else                       { DIS.SignL |= MODE_AUTO;  }
                                                      } break; 
                                       case P9_21: {   //--------------------------------------------------------------------------
                                                        DIS.ScreenS = p9_21; 
                                                        //DIS.ScreenS = p5_8TF;  
                                                        DIS.SignR = 0; // DIS.Point = 4; 
                                                        DIS.Small_Dis[4] = 38; 
                                                        DIS.Small_Dis[5] = small_sting[p5_8TF][5];
                                                        DIS.Small_Dis[6] = small_sting[p5_8TF][6]; 
                                                        DIS.Small_Dis[7] = small_sting[p5_8TF][7];
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.SignR = 0; 
                                                        DIS.ScreenB = DIGITAL;
                                                       // if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
                                                       // else                       { DIS.SignL |= MODE_AUTO;  }
                                                      }break;
                                        case P9_22: {   //--------------------------------------------------------------------------
                                                        DIS.ScreenS = p9_22; 
                                                        //DIS.ScreenS = p5_9TB;  
                                                        DIS.SignR = 0;//DIS.Point = 4;
                                                        DIS.Small_Dis[4] = 38; 
                                                        DIS.Small_Dis[5] = small_sting[p5_9TB][5];
                                                        DIS.Small_Dis[6] = small_sting[p5_9TB][6]; 
                                                        DIS.Small_Dis[7] = small_sting[p5_9TB][7];
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.SignR = 0; 
                                                         //if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
                                                         //else                       { DIS.SignL |= MODE_AUTO;  }
                                                      }break; 
                                         case P9_23: {   //--------------------------------------------------------------------------
                                                        DIS.ScreenS = p9_23; 
                                                         //DIS.ScreenS = p5_10T;  
                                                        DIS.SignR = 0; //DIS.Point = 4;
                                                        DIS.Small_Dis[4] = 38; 
                                                        DIS.Small_Dis[5] = small_sting[p5_10T][5];
                                                        DIS.Small_Dis[6] = small_sting[p5_10T][6]; 
                                                        DIS.Small_Dis[7] = small_sting[p5_10T][7];
                                                        DIS.ScreenB = DIGITAL;
                                                        DIS.SignR = 0; 
                                                         //if ( SYS.bit.Dida)         { DIS.SignL &= ~MODE_AUTO; }          //显示组态标志显示
                                                         //else                       { DIS.SignL |= MODE_AUTO;  }
                                                      }break; 

                                        case P9_24SAV:{
                                                        DIS.ScreenS = p9_24SAV;   DIS.SignR = 0;
                                                        switch( SYS.bit.Step )
                                                        {
                                                            case STP_0 :  if(KB.bit.CountKB%2)  DIS.ScreenB = YES; else DIS.ScreenB = NO; break;//选择阶段
                                                            case STP_1 :  DIS.ScreenB = REDY;  break; //运行阶段
                                                            case STP_2 :  DIS.ScreenB = RUN;   break; //运行阶段
                                                            case STP_3 :  DIS.ScreenB = SAVE;  break; //运行阶段
                                                            case STP_4 :  DIS.ScreenB = COMP;  break; //运行阶段
                                                            case STP_5 :  DIS.ScreenB = CACU;  break; //运行阶段
                                                            case SP_OK :  DIS.ScreenB = M__OK; break;
                                                            default:;break;
                                                        } 
                                          
                                                      }break;
                                         default:break; 
                                   }
                                 
                               
                               }
                               */
                             default:break; 
                          }
                          //--------------------------------------------------这里是什么已经不知道了
                          if( SYS.bit.PageS2 == 0 && SYS.bit.Step == SP_OK )
                          {
                               SYS.bit.PageStateL = SYS.bit.PageStateF; 
                               DIS.ScreenB = DIGITAL;
                               DIS.SignL = DIS.SignR = CLK.SecCONFIRM = 0;
                               SYS.bit.Step = STP_0; 
                               SYS.bit.PageS1 = 0; OP.Function = NULLF; return;
                          }
                      
                          if( CLK.SecCONFIRM ) { DIS.ScreenB = M__4M + CLK.SecKB; } 
                          //--------------------------------------------------键盘长按处理                         
                          if( KB.bit.LongKB )
                          {
                             if( KB.bit.Derection )  
                             { 
                                 //if(CLK.SecKB==2)       { if( KB.bit.CountKB<MAX[SYS.bit.PageS1][SYS.bit.PageS2-1]-20 ) KB.bit.CountKB += 20;}
                                 //else if(CLK.SecKB==0)  {  KB.bit.CountKB = MAX[SYS.bit.PageS1][SYS.bit.PageS2-1]; }
                                // if(CLK.SecKB==2)       { if( KB.bit.CountKB<MAX[SYS.bit.PageS1][SYS.bit.PageS2-1] ) KB.bit.CountKB++;}
                                if( SYS.bit.PageS2 == P1_4STEP && SYS.bit.PageS1 == PG1 && SYS.bit.Step == STP_2 )
                                {
                                   if(CLK.SecKB < 3 && KB.bit.CountKB < 400 )       { KB.bit.CountKB++;  }
                                }
                                else if ( SYS.bit.PageS2 == P3_1_MIN && SYS.bit.PageS1 == PG3 && SYS.bit.Step == STP_2 )
                                {
                                   if(CLK.SecKB < 3 && KB.bit.CountKB < 400 )       { KB.bit.CountKB++;  }
                                }  
                                else if ( SYS.bit.PageS2 == P3_2_MAX && SYS.bit.PageS1 == PG3 && SYS.bit.Step == STP_2 )
                                {
                                   if(CLK.SecKB < 3 && KB.bit.CountKB < 400 )       { KB.bit.CountKB++;  }
                                }
                                else
                                {  
                                   if(CLK.SecKB==0)         {  KB.bit.CountKB = MAX[SYS.bit.PageS1][SYS.bit.PageS2-1]; }
                                   else if (CLK.SecKB < 8)  
                                   {  
                                        if( KB.bit.LongKB_M )
                                        {
                                            if( KB.bit.CountKB<MAX[SYS.bit.PageS1][SYS.bit.PageS2-1] - 10 ) KB.bit.CountKB+=10;
                                        }
                                        else
                                        {  
                                            if( KB.bit.CountKB<MAX[SYS.bit.PageS1][SYS.bit.PageS2-1] ) KB.bit.CountKB++; 
                                        }   
                                    }
                                    else;
                                } 
                     
                             } 
                             else          
                             {
                                 //if(CLK.SecKB==2)       { if( KB.bit.CountKB>MIN[SYS.bit.PageS1][SYS.bit.PageS2-1]+20 ) KB.bit.CountKB -= 20;} 
                                 //else if(CLK.SecKB==0)  {  KB.bit.CountKB = MIN[SYS.bit.PageS1][SYS.bit.PageS2-1];}
                                // if(CLK.SecKB==2)       { if( KB.bit.CountKB>MIN[SYS.bit.PageS1][SYS.bit.PageS2-1] ) KB.bit.CountKB--;} 
                                if( SYS.bit.PageS2 == P1_4STEP && SYS.bit.PageS1 == PG1 && SYS.bit.Step == STP_2 )
                                {
                                   if(CLK.SecKB < 3 && KB.bit.CountKB < 400 )       { KB.bit.CountKB++;  }
                                }
                                else if ( SYS.bit.PageS2 == P3_1_MIN && SYS.bit.PageS1 == PG3 && SYS.bit.Step == STP_2 )
                                {
                                   if(CLK.SecKB < 3 && KB.bit.CountKB < 400 )       { KB.bit.CountKB++;  }
                                }  
                                else if ( SYS.bit.PageS2 == P3_2_MAX && SYS.bit.PageS1 == PG3 && SYS.bit.Step == STP_2 )
                                {
                                   if(CLK.SecKB < 3 && KB.bit.CountKB < 400 )       { KB.bit.CountKB++;  }
                                }
                                else
                                {  
                                    if(CLK.SecKB==0)         {  KB.bit.CountKB = MIN[SYS.bit.PageS1][SYS.bit.PageS2-1];}
                                    else if(CLK.SecKB < 8)   
                                    {   
                                        if( KB.bit.LongKB_M )
                                        {
                                            if( KB.bit.CountKB>MIN[SYS.bit.PageS1][SYS.bit.PageS2-1] + 10 ) KB.bit.CountKB-=10;
                                        }
                                        else
                                        {  
                                            if( KB.bit.CountKB>MIN[SYS.bit.PageS1][SYS.bit.PageS2-1] ) KB.bit.CountKB--; 
                                        }      
                                  }
                                  else;
                                } 
                             }   
                          }
                          if ( SYS.bit.Dida)         { DIS.SignR |= MODE_BOOK;  }          //显示组态标志显示
                          else                       { DIS.SignR &= ~MODE_BOOK; }     
                      
                          if( KB.bit.KeyOK==0 && CLK.SecKB==0 && SYS.bit.PageS2 )              //用来先显示闪烁后来确定
                          {
                             if(SYS.bit.Dida&& ( SYS.bit.PageS1!= PG5) )  
                             {  
                                DIS.ScreenB = NULL_B;  
                             }
                             else ;
                          }
                     }break;

        default : break;
    }
    //----------------------------------------------------------公用错误处理中心
    if( ALA.all&0x0003ffff )   // 
    {  
       unsigned char i,j=0;
       //----------------------------------------严重错误并且在自动模式下 复位
       if( CLK.SecAL==0) { CLK.SecAL=6; }
       //----------------------------------------
       DIS.Alarm_Dis[1] = 0;
       for(i=0;i<18;i++)
       {
           if((ALA.all)>>i&0x000000001)  
           { 
              DIS.Alarm_Dis[1]++;
           }
       }
       if( CLK.SecAL==6)  //用来控制下面的显示的
       {  
          CLK.SecAL = 5; 
          if( DIS.Alarm_Dis[3]<DIS.Alarm_Dis[1] )  {  DIS.Alarm_Dis[3]++;   }
          else                                     {  DIS.Alarm_Dis[3] = 1; }    
       } 
       if( DIS.Alarm_Dis[3] > DIS.Alarm_Dis[1] ) {  DIS.Alarm_Dis[3] = DIS.Alarm_Dis[1]; }
       for(i=0;i<18;i++)
       {
          if((ALA.all>>i)&0X00000001)  
          { 
              j++;
              if(DIS.Alarm_Dis[3]==j) break;
          }
       }
       
       //---------------------------------------------控制消隐报警的
       if( SYS.bit.PageStateL == AUTO )
       {  
          if( ALA.bit.KB__FLAG ) 
          {
              DIS.ScreenB = ALARM;
              if(SYS.bit.Dida)  {   DIS.ScreenS = NULL_SMA;      }
              else              {   DIS.ScreenS = LERK_ERR + i;  }   
              DIS.SignR &= ~( MODE_mA + MODE_DEGR + MODE_PERC );
              if( DIS.ScreenS == BREAK_AU )                       //严重错误的自动重启
              { 
                 DIS.ScreenS = ALARM_S;
                 //----------------------------------
                 DIS.Small_Dis[0] = OP.Bre_cnt%10;
                 DIS.Small_Dis[1] = OP.Bre_cnt/10;
                 DIS.Small_Dis[2] = 36;
                 //----------------------------------  
                 DIS.Small_Dis[3] = 20;
                 DIS.Small_Dis[4] = 10; 
                 DIS.Small_Dis[5] = 14;
                 DIS.Small_Dis[6] = 27;
                 DIS.Small_Dis[7] = 11;
              }
          }
          else
          {
           if(SYS.bit.Dida)  {   DIS.ScreenS = AUTO_;         }
           else              
           {    
              DIS.ScreenS = ALARM_S;
              //----------------------------------
              DIS.Small_Dis[0] = small_sting[LERK_ERR + i][6];
              DIS.Small_Dis[1] = small_sting[LERK_ERR + i][7];
              
              DIS.Small_Dis[2] = DIS.Alarm_Dis[1];
              //----------------------------------  
              DIS.Small_Dis[3] = 38;
              DIS.Small_Dis[4] = 24; 
              DIS.Small_Dis[5] = 29;
              DIS.Small_Dis[6] = 30;
              DIS.Small_Dis[7] = 10;
           }
             if( ALA.bit.Break_AU && SYS.bit.PageS2 == 0 && CLK.SecCONFIRM ==0)                       //严重错误的自动重启
             {  
                DIS.ScreenB = M_ER_M; 
             }
          }
            //---------------------------------------------严重错误时的上锁显示
            if( ALA.bit.Act__ERR || ALA.bit.Break_AU || ALA.bit.Elec_ERR || ALA.bit.Sens_ERR )
            {
                if(SYS.bit.Dida)  {   DIS.SignR |= MODE_LOCK;     }
                else              {   DIS.SignR &= ~MODE_LOCK;    } 
            } 
            //------------------------------------------------------------------
        }  
        else if ( SYS.bit.PageStateL == MAN )
        {
           if( ALA.bit.KB__FLAG ) 
           {
              DIS.ScreenB = ALARM;
              if(SYS.bit.Dida)  {   DIS.ScreenS = NULL_SMA;      }
              else             
              {  
                  DIS.ScreenS = LERK_ERR + i; 
                  if( ALA.bit.Break_AU )                       //严重错误的自动重启
                  {  
                      DIS.ScreenB = M_ER_M; 
                      DIS.ScreenS = ALARM_S;
                      //----------------------------------
                      DIS.Small_Dis[0] = OP.Bre_cnt%10;
                      DIS.Small_Dis[1] = OP.Bre_cnt/10;
              
                      DIS.Small_Dis[2] = 36;
                      //----------------------------------  
                      DIS.Small_Dis[3] = 20;
                      DIS.Small_Dis[4] = 10; 
                      DIS.Small_Dis[5] = 14;
                      DIS.Small_Dis[6] = 27;
                      DIS.Small_Dis[7] = 11;
                  } 
              }
           }
           else
           {
              if( ALA.bit.Break_AU && CLK.SecCONFIRM ==0 )                       //严重错误的自动重启
              {  
                  DIS.ScreenB = M_ER_M; 
              }
              
           }
           DIS.SignR &= ~MODE_PERC;
            //---------------------------------------------严重错误时的上锁显示
            if( ALA.bit.Act__ERR || ALA.bit.Break_AU || ALA.bit.Elec_ERR || ALA.bit.Sens_ERR )
            {
                if(SYS.bit.Dida)  {   DIS.SignR |= MODE_LOCK;     }
                else              {   DIS.SignR &= ~MODE_LOCK;    } 
            } 
            //------------------------------------------------------------------
        }
        else if ( SYS.bit.PageStateL == CONFIG || SYS.bit.PageStateL == QUICK )
        {
            DIS.ScreenB = ALARM;
            if(SYS.bit.Dida)  {   DIS.ScreenS = NULL_SMA;      }
            else              {   DIS.ScreenS = LERK_ERR + i;  }   
        } 
    //   else
    //   {
    //       DIS.SignR &= ~MODE_LOCK; 
    //   }  
    //   if(ALA.bit.Vrlv_ERR)                      //严重错误的自动重启
    //   {  
    //         DIS.ScreenB = M___M + CLK.SecAL;
    //         DIS.ScreenS = VRLV_ERR;
    //         if( CLK.SecAL==1 ) { TBCCTL0 &=~CCIE; }    
    //   }
    //   else                                      
    //   {
    //        if(SYS.bit.Dida)  {    DIS.ScreenS = NULL_SMA;        }
    //        else              {    DIS.ScreenS = LERK_ERR + i;    }    
    //   }
       //---------------------------------------------------------
       
       
    } 
}
