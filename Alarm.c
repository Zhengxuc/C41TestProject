# include "Device.h"

void Alarm_Deal(void)
{
   if (  SYS.bit.PageStateL==AUTO || SYS.bit.PageStateL== MAN  )
   {  
     if( SYS.bit.PageStateL==AUTO )
     {  
        //------------------------------------------------
//        if( VAL.V_p6_1__LK == V_ACTV ) //空气泄露报警
//        {
//          if( OP.Leak_time < 50 ) 
//          { 
//             if( OP.Leak_time == 0 )  { OP.Leak_cnt = 0; }
//             OP.Leak_time++; 
//          } 
//          else                     
//          {  
//             OP.Leak_time = 0;
//             if ( OP.Leak_cnt > 60 )   { ALA.bit.Leak_ERR = 1; }
//             else                      { ALA.bit.Leak_ERR = 0; }   
//          }    
//        }
        if( VAL.V_p6_1__LK == V_ACTV ) //空气泄露报警V_NOCR  改成PSD故障报警
        {
             ALA.bit.Leak_ERR = 1;    
        } 
        if( VAL.V_p6_1__LK == V_NOCR ) //空气泄露报警V_NOCR  改成PSD故障报警
        {
             ALA.bit.Leak_ERR = 0;    
        }       
        //-------------------------------------------------
//        if( VAL.V_p6_2__SV == V_ACTV  ) //电流输入超范围报警
//        {
//          if( AD.Temp_In[4] > OP.Up_C_alarm + 20 )        {  ALA.bit.Insv_HIG = 1; }
//          else if(  AD.Temp_In[4] < OP.Down_C_alarm - 20) {  ALA.bit.Insv_LOW = 1; }
//          else    {  ALA.bit.Insv_ERR = ALA.bit.Insv_HIG = ALA.bit.Insv_LOW = 0; }
//        } 
        
        //-------------------------------------------------
        if( VAL.V_p6_3__ZO == V_ACTV ) //阀零位超差报警信息
        {
            
        } 
        //------------------------------------------------
        if( VAL.V_p6_4__VR == V_ACTV ) //超阀位报警信息
        {
            if( OP.Meterage_L < OP.Down_4_1 - 15 || OP.Meterage_L > OP.Up_4_2 + 15 )  { ALA.bit.Vrlv_ERR = 1; OP.Error_serious = 1; }
            else { ALA.bit.Vrlv_ERR = 0; OP.Error_serious = 0; }
        }
        //------------------------------------------------
        if( VAL.V_p6_5__TO == V_ACTV ) //定位超时报警信息
        {
          if( (OP.Difference_Ex > OP.BRND_1_3)&&(SYS.bit.PageStateL==AUTO) )
          {
              if( AD.Temp_In[4] > OP.TIM_ERR_FOMER + 100 || AD.Temp_In[4] < OP.TIM_ERR_FOMER - 100 ) 
              {
                  OP.Time_Out_Error_Counter = 0; 
              }
              
              if( OP.Time_Out_Error_Counter == 0 )  // 初始化赋值
              {  
                   if( OP.Difference_Direction_Ex ) //正动
                   {  
                      if ( VAL.V_p2_6_TUP > VAL.V_p8_1_TRG ) CLK.SecRun = VAL.V_p2_6_TUP - 1;
                      else                                   CLK.SecRun = VAL.V_p8_1_TRG - 1;
                   }  
                   else
                   {
                      if ( VAL.V_p2_7_TDN > VAL.V_p8_1_TRG ) CLK.SecRun = VAL.V_p2_7_TDN - 1;
                      else                                   CLK.SecRun = VAL.V_p8_1_TRG - 1;
                   }
                   OP.Time_Out_Error_Counter = 1; 
                   OP.ACT_ERR_FOMER = AD.Temp_Back[4];
                   OP.TIM_ERR_FOMER = AD.Temp_In[4];
              }
              else                                 
              {
                    if( CLK.SecRun%( VAL.V_p8_1_TRG ) == 0 )     
                    {
                        ALA.bit.Time_OUT = 1;  
                    }//最大时间超值
                    if( OP.Difference_Direction_Ex )    //正动
                    {   
                       if( CLK.SecRun%( VAL.V_p2_6_TUP ) == 0 )  
                       {
                         ALA.bit.Time_ERR = 1;  
                       }//上行时间超值
                    }
                    else
                    {  
                       if( CLK.SecRun%( VAL.V_p2_7_TDN ) == 0 )  {  ALA.bit.Time_ERR = 1;  }//下行时间超值
                    }   
                    if( CLK.SecRun == 0 )  {  OP.Time_Out_Error_Counter = 0; }            // 归零
              }  
            }
            else
            {
              ALA.bit.Time_OUT = 0;  OP.Time_Out_Error_Counter = 0;  ALA.bit.Time_ERR = 0;  ALA.bit.Act__ERR = 0; 
            }  
          //--------控制上行程超时报警
        }
     } 
 
      //--------------------------------------------------------------单片机内部固有报警
      if( 1 )        //不可屏蔽的严重错误
      {
          ALA.bit.Break_AU = 0; OP.Error_serious = 0;
          //-----------------------------设定参数检测
          if( VAL.V_p8_5_MAX > 3800 )                   {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_8_5_UP; OP.Error_serious = 1; }//8_5设置存在严重偏差 0 
          if( VAL.V_p8_5_MAX < 3400 )                   {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_8_5_DN; OP.Error_serious = 1; }//8_5设置存在严重偏差 1
          if( VAL.V_p8_4_MIN > 900  )                   {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_8_4_UP; OP.Error_serious = 1; }//8_4设置存在严重偏差 2
          if( VAL.V_p8_4_MIN < 500  )                   {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_8_4_DN; OP.Error_serious = 1; }//8_4设置存在严重偏差 3
          if( VAL.V_p3_1_MIN > VAL.V_p3_2_MAX )         {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_3_1_D_3_2; OP.Error_serious = 1; }//机械行程整定边界控制存在严重错误     4
          if( VAL.V_p4_1_MIN > VAL.V_p4_2_MAX )         {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_4_1_D_4_2; OP.Error_serious = 1; }//控制行程设置存在严重错误             5
          if( VAL.V_p2_1_MIN > VAL.V_p2_2_MAX )         {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_2_1_D_2_2; OP.Error_serious = 1; }//起始与结束电流设置存在严重错误       6
          if( VAL.V_p7_1_MIN > VAL.V_p7_2_MAX )         {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_7_1_D_7_2; OP.Error_serious = 1; }//反馈起始与结束电流设置存在严重错误   7
          if( OP.Down_Limited > OP.Up_Limited )         {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_M_DN_D_M_UP; OP.Error_serious = 1; }//机械行程位置存在严重设置错误       8    
          if( OP.Down_4_1 < OP.Down_Limited )           {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_M_DN_D_S_DN; OP.Error_serious = 1; }//控制行程位置超过机械行程位置最低值 9
          if( OP.Up_Limited < OP.Up_4_2 )               {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_M_UP_D_S_UP; OP.Error_serious = 1; }//控制行程位置超过机械行程位置最高值 10
          if( VAL.V_p3_2_MAX - VAL.V_p3_1_MIN < 401 )   {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_MACHINE_10; OP.Error_serious = 1; } //机械行程只用到了传感器范围的10%    12
          if( VAL.V_p4_2_MAX - VAL.V_p4_1_MIN < 101 )   {  ALA.bit.Break_AU = 1; OP.Bre_cnt = E_CONTROL_10; OP.Error_serious = 1; } //控制行程只用到了机械行程的10%      13 
          //ALA.bit.Break_AU = 1; OP.Bre_cnt = 19;//报警测试
          //-----------------------------电路故障报警
          if( AD.CurrentIn < 20 || AD.CurrentIn > 4050 )   {  ALA.bit.Elec_ERR = 1; OP.Error_serious = 1; }//不正确的电流输入值
          else                                               {  ALA.bit.Elec_ERR = 0; OP.Error_serious = 0; }
          //-----------------------------传感器失灵报警
          if( AD.CurrentBack < 20 || AD.CurrentBack > 4050 ) { ALA.bit.Sens_ERR = 1; OP.Error_serious = 1; OP.Difference_Ex =0;} //不正确的反馈输入值
          else
          {
              ALA.bit.Sens_ERR = 0;  OP.Error_serious = 0;
              //-----------------------------驱动故障报警
              if( (OP.Difference_Ex > OP.BRND_1_3)&&(SYS.bit.PageStateL == AUTO) )
              {
                  if( OP.ACT_ERR_FOMER > AD.CurrentBack && OP.ACT_ERR_FOMER < AD.CurrentBack + 20 )       { OP.Act_cnt++; }
                  else if ( AD.CurrentBack > OP.ACT_ERR_FOMER && AD.CurrentBack < OP.ACT_ERR_FOMER + 20 ) { OP.Act_cnt++; }
                  else if ( OP.ACT_ERR_FOMER == AD.Temp_Back[4] )                                           { OP.Act_cnt++; } 
                  else                                                                                      { OP.Act_cnt = 0;  ALA.bit.Act__ERR = 0; }                                                                                                            
                  
                  if( OP.Act_cnt > 50 )  { ALA.bit.Act__ERR = 1; }   //驱动故障不做 （因为驱动单元已坏）
              } 
              else {  ALA.bit.Act__ERR = 0; OP.Act_cnt = 0; } 
          }
          
      }  
  //    if(  ALA.bit.Time_OUT==1 || ALA.bit.Rng__ERR==1 )   {  OP.Error_serious = 1; }  //严重错误标志至位
 //     else                                                {  OP.Error_serious = 0; }
      
      ALA.bit.Delay++;
      if( ALA.bit.Delay == 36 ) 
      { 
        ALA.bit.Delay = 0; 
        ALA.bit.KB__FLAG = 1; 
      }
   }
   //------------------------------------------------
}

/*
   unsigned int  Leak_ERR  : 1; //0      
   unsigned int  Insv_ERR  : 1; //1
   unsigned int  Zero_ERR  : 1; //2
   unsigned int  Vrlv_ERR  : 1; //3          
   unsigned int  Time_ERR  : 1; //4
   unsigned int  Elec_ERR  : 1; //5
   unsigned int  Sens_ERR  : 1; //6
   unsigned int  Act__ERR  : 1; //7
   unsigned int  Calc_ERR  : 1; //8   
   unsigned int  Break_AU  : 1; //9
   unsigned int  Out__RNG  : 1; //10
   unsigned int  No_SCALE  : 1; //11
   unsigned int  Rng__ERR  : 1; //12
   unsigned int  Time_OUT  : 1; //13
   unsigned int  Spr__ERR  : 1; //14
   unsigned int  Keep_ERR  : 1; //15
*/
