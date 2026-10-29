# include "Device.h"
int g_ssCur_ValvePointDis =0;
int Cur_ValvePointDIS = 0;
unsigned long Test_times=0;

unsigned long Count;
char times_flash_buffer[7];

char CalculateTestTimes(unsigned long DisTemp)
{
  unsigned short DisData_Up;  //万位以上显示上方
  unsigned short DisData_Down;//万位以下显示下方
  
  if(DisTemp>=10000)
  {
    DisData_Up=DisTemp/10000;
    DisData_Down=DisTemp%10000;
    
    DIS.Digital_Dis_W[3] = 38;
    DIS.Digital_Dis_W[2] = (DisData_Up/100)%10;
    DIS.Digital_Dis_W[1] = (DisData_Up/10)%10;
    DIS.Digital_Dis_W[0] = (DisData_Up)%10;
    
    DIS.Digital_Dis[0] = (DisData_Down/1000)%10;
    DIS.Digital_Dis[1] = (DisData_Down/100)%10;
    DIS.Digital_Dis[2] = (DisData_Down/10)%10;
    DIS.Digital_Dis[3] = (DisData_Down)%10;
    return 1;
  }
  else 
  {
    DIS.Digital_Dis_W[3] = 0;
    DIS.Digital_Dis_W[2] = 0;
    DIS.Digital_Dis_W[1] = 0;
    DIS.Digital_Dis_W[0] = 0;
    
    DIS.Digital_Dis[0] = (DisTemp/1000)%10;
    DIS.Digital_Dis[1] = (DisTemp/100)%10;
    DIS.Digital_Dis[2] = (DisTemp/10)%10;
    DIS.Digital_Dis[3] = (DisTemp)%10;
    return 0;
  }
}




void LineCalculate(void)
{
  g_dangel_now = AD.CurrentBack;
  g_dangel_now = (double)(g_dangel_now - 2048)/4096*3.141592653; 
  if(VAL.V_p1_1_RCR == V_LINE )
  {
    g_dsinAngel_now = sin(g_dangel_now);  
  }
  else 
  {
    g_dsinAngel_now = tan(g_dangel_now);  
  }
}
void Digital_Dis4B(unsigned char mode,unsigned int DisTemp )
{
  if(4 == mode)
  {
                                   DIS.Digital_Dis[0] = (DisTemp/1000)%10;
                                   DIS.Digital_Dis[1] = (DisTemp/100)%10;;
                                   DIS.Digital_Dis[2] = (DisTemp/10)%10;
                                   DIS.Digital_Dis[3] = (DisTemp)%10;  
  }
  else if(3 == mode)
  {
                                   DIS.Digital_Dis[0] = NULL_BIG; 
                                   DIS.Digital_Dis[1] = (DisTemp/100)%10;
                                   DIS.Digital_Dis[2] = (DisTemp/10)%10;
                                   DIS.Digital_Dis[3] = (DisTemp)%10;  
  }
  else if(2 == mode)
  {
                                   DIS.Digital_Dis[0] = NULL_BIG; 
                                   DIS.Digital_Dis[1] = (DisTemp/100)%10;
                                   DIS.Digital_Dis[2] = (DisTemp/10)%10;
                                   DIS.Digital_Dis[3] = NULL_BIG;  
  }
  else if(41 == mode)
  {
                               DIS.Digital_Dis[0] = (DisTemp/1000)%10;
                               DIS.Digital_Dis[1] = (DisTemp/100)%10;
                               DIS.Digital_Dis[2] = (DisTemp/10)%10;                                                                                             
                               DIS.Digital_Dis[3] =  39;  
  }
  else if(21 == mode)
  {
                                          DIS.Digital_Dis[0] = NULL_BIG; 
                                          DIS.Digital_Dis[1] = (DisTemp/10)%10;
                                          DIS.Digital_Dis[2] = DisTemp%10;
                                          DIS.Digital_Dis[3] = NULL_BIG; 
  }
  else
  {
    ;
  }
}

////////////////////////////////////////////////////////////////////////////
void Back_Calculate(void)
{ 
    unsigned long s = 0;  
    unsigned long sa = 0;     
    unsigned char i ,j;
    static int a[5]={0,0,0,0,0};

    if(( VAL.V_p1_1_RCR == V_LINE )||( VAL.V_p1_1_RCR == V_LT ))
    {
      if(MAN ==  SYS.bit.PageStateL) 
      {     
        LineCalculate();                         
      } 
      else  //此处借用ARUNUTO.C的计算值
      {
        ;   
      }
      Cur_ValvePointDIS = (g_dsinAngel_now - g_dsinAngel_down)*1000.0/(g_dsinAngel_up - g_dsinAngel_down);      
    }
    else
    {
      ;
    }        
    for(i=1;i<5;i++)
    {
      DIS.FIFORDIS[i] = DIS.FIFORDIS[i-1];
      a[i] = a[i-1];      
    }

    DIS.FIFORDIS[0] =AD.CurrentBack;
    a[0] = Cur_ValvePointDIS;
    
    for(i=0;i<4;i++)
    {
      s += DIS.FIFORDIS[i]; 
      sa += a[i];
    } 
    
    DIS.CurrentBackDis =s*0.25; 
    s=0; 
    g_ssCur_ValvePointDis = sa*0.25;
    sa = 0;
           
}
///////////////////////////////////////////////////////////////////////////
void Back_LINE(void) 
{
  if(( VAL.V_p1_1_RCR == V_LINE )||( VAL.V_p1_1_RCR == V_LT ))
  {
    if( g_ssCur_ValvePointDis > 0)
    {
      DIS.DisTemp = g_ssCur_ValvePointDis;      
      DIS.SignL &= ~MODE_; 
    } 
    else
    {
      DIS.DisTemp = -g_ssCur_ValvePointDis;      
      DIS.SignL |= MODE_;
    }     
  }
  else
  {
    if( DIS.CurrentBackDis > OP.Down_4_1   )
    {
      DIS.DisTemp = (float)(DIS.CurrentBackDis - OP.Down_4_1)/(float)( OP.Up_4_2 - OP.Down_4_1 )*1000;
      DIS.SignL &= ~MODE_; 
    } 
    else
    {
      
      DIS.DisTemp = (float)(OP.Down_4_1 - DIS.CurrentBackDis)/(float)( OP.Up_4_2 - OP.Down_4_1 )*1000;
      DIS.SignL |= MODE_;
    }  
  }  
}

//////////////////////////////////////////////////////////////////////////
void  Back_Sign(void)
{
  if(( VAL.V_p1_1_RCR == V_LINE )||( VAL.V_p1_1_RCR == V_LT ))
  {
    if(VAL.V_p4_3_VRL == V_OPEN)
      //if((VAL.V_p3_3_RCT == V_CLOK &&VAL.V_p4_3_VRL == V_OPEN) || ( VAL.V_p3_3_RCT == V_CTCL &&VAL.V_p4_3_VRL == V_OPEN))
    {
      if ( DIS.DisTemp < 1000 ) 
      { 
        //                                       DIS.DisTemp-=5;  //此删除为除去抬高的5%
        DIS.DisTemp = 1000 - DIS.DisTemp; 
        DIS.SignL &= ~MODE_; 
      }
      else
      {
        DIS.DisTemp = DIS.DisTemp - 1000;
        DIS.SignL |= MODE_;
      }   
      //                             DIS.DisTemp+=5;   //此删除为除去抬高的5%
    }  
  }
  else
  {
    if(VAL.V_p4_3_VRL == V_OPEN)
      //if((VAL.V_p3_3_RCT == V_CLOK &&VAL.V_p4_3_VRL == V_OPEN) || ( VAL.V_p3_3_RCT == V_CTCL &&VAL.V_p4_3_VRL == V_OPEN))
    {
      
      if( DIS.CurrentBackDis < OP.Up_4_2   )
      {
        DIS.DisTemp = (float)(OP.Up_4_2 - DIS.CurrentBackDis )/(float)( OP.Up_4_2 - OP.Down_4_1 )*1000;
        DIS.SignL &= ~MODE_; 
      } 
      else
      {
        
        DIS.DisTemp = (float)(DIS.CurrentBackDis -OP.Up_4_2 )/(float)( OP.Up_4_2 - OP.Down_4_1 )*1000;
        DIS.SignL |= MODE_;
      }                                
      
    }                                                         
  }
  if( (DIS.DisTemp/100)%10||(DIS.DisTemp/1000)%10 )
  { 
    if((DIS.DisTemp/1000)%10==0)
      Digital_Dis4B(3,DIS.DisTemp);                                      
    else  
      Digital_Dis4B(4,DIS.DisTemp);                                                                         
    DIS.Point = 2; 
  }
  else
  {
    Digital_Dis4B(21,DIS.DisTemp);                                                                        
    DIS.Point = 1; 
  }  
}
/////////////////////////////////////////////////////////////////////////////
void Back_ConfigSign(void)
{ 
  DIS.DisTemp = (float)(DIS.CurrentBackDis - OP.Down_4_1)/(float)(OP.Up_4_2 - OP.Down_4_1)*1000;
  if( DIS.DisTemp > 2000 ) { DIS.DisTemp = ( ~DIS.DisTemp ) + 1;  DIS.SignL |= MODE_;}
  else                     { DIS.SignL &= ~MODE_;                                    }
  if( ALA.all&0x0003ffff ) { DIS.SignL &= ~MODE_; }
  
  if(VAL.V_p4_3_VRL == V_OFF) {  DIS.DisTemp = 1000 - DIS.DisTemp;  }    
  if( (DIS.DisTemp/100)%10||(DIS.DisTemp/1000)%10 )
  { 
    if((DIS.DisTemp/1000)%10==0) 
      Digital_Dis4B(3,DIS.DisTemp); 
    else  
      Digital_Dis4B(4,DIS.DisTemp); 
    //DIS.SignR |= MODE_PERC; 
    DIS.Point = 2; 
  }
  else
  {
    
    Digital_Dis4B(21,DIS.DisTemp);                                           
    //DIS.SignR |= MODE_PERC; 
    DIS.Point = 1; 
  }
}
////////////////////////////////////////////////////////////////////////////


void Display_Data_Calculate(void)
{
   Back_Calculate();   

    
    //if(OP.Difference_Ex <VAL.V_p1_3BRND)
    //{
    //  DIS.CurrentBackDis =OP.Expectation;
    //}
   // DIS.CurrentBackDis = AD.CurrentBack;
    
    
    //g_PIDServiceData.SetPointDis =g_PIDServiceData.SetPoint+5;
    g_PIDServiceData.SetPointDis =g_PIDServiceData.SetPoint;
    if(VAL.V_p3_3_RCT == V_CLOK) {g_PIDServiceData.SetPointDis = 1000- g_PIDServiceData.SetPointDis; }
    if(VAL.V_p4_3_VRL == V_OPEN)   {g_PIDServiceData.SetPointDis = 1000- g_PIDServiceData.SetPointDis; }
    g_PIDServiceData.SetPointDis +=5;
    /*
    if(DIS.CurrentBackDis>DIS.CurrentBackDisL)  {j = DIS.CurrentBackDis-DIS.CurrentBackDisL; }
    else{j = DIS.CurrentBackDisL-DIS.CurrentBackDis; }
    if(j<2){DIS.CurrentBackDis =DIS.CurrentBackDisL;}
    else   {DIS.CurrentBackDisL =DIS.CurrentBackDis;}
    */
    switch( SYS.bit.PageStateL )
    {
       case AUTO   :{  
                        //if(OP.Difference_Ex <OP.BRND_1_3)
                        //{
                         //DIS.CurrentBackDis =OP.Expectation;
                        //}
         

                        if(SYS.bit.PageS2)    
                        {  
                            if(SYS.bit.PageS2==1) 
                            { 
                                //DIS.DisTemp = (VAL.V_p2_2_MAX - VAL.V_p2_1_MIN)*10.0/OP.Range_C*AD.Temp_In[4];//这个公式肯定有问题
                               // DIS.DisTemp = 1607.0/( VAL.V_p8_5_MAX - VAL.V_p8_4_MIN )*AD.Temp_In[4];   //这个公式需要验证
                               // DIS.DisTemp = 1600.0/( VAL.V_p8_5_MAX - VAL.V_p8_4_MIN )*AD.CurrentDis;   //这个公式需要验证
                                if(AD.CurrentDis>VAL.V_p8_4_MIN)
                                  {DIS.DisTemp = 400.0+1600.0/( VAL.V_p8_5_MAX - VAL.V_p8_4_MIN )*(AD.CurrentDis-VAL.V_p8_4_MIN);}   
                                else 
                                  {DIS.DisTemp = 400.0-1600.0/( VAL.V_p8_5_MAX - VAL.V_p8_4_MIN )*(VAL.V_p8_4_MIN-AD.CurrentDis);}
                                if( DIS.DisTemp > 5000)   DIS.DisTemp = 0;
                                if( (DIS.DisTemp/1000)%10 )
                                { 
 
                                   Digital_Dis4B(4,DIS.DisTemp);
                                   //DIS.SignR |= MODE_mA;
                                   DIS.Point = 1; 
                                }
                                else
                                {

 
                                   Digital_Dis4B(3,DIS.DisTemp);                                   
                                   //DIS.SignR |= MODE_mA; 
                                   DIS.Point = 1; 
                                }  
                            }    
                            else
                            {
                                DIS.DisTemp = ( int)(((REFVOL * AD.Temperature )/ 4096 - 0.986) /0.0000355); 
                                DIS.DisTemp = DIS.DisTemp-200;
                                if( (DIS.DisTemp/1000)%10 )
                                { 
                                   Digital_Dis4B(3,DIS.DisTemp);   
                                   //DIS.SignR |= MODE_DEGR; 
                                   DIS.Point = 2; 
                                }
                                else
                                {
                                   Digital_Dis4B(2,DIS.DisTemp);   
                                   //DIS.SignR |= MODE_DEGR; 
                                   DIS.Point = 1; 
                                } 
                            }
                        }
                        else
                        {
                          //--------------------------------------------------公式
                            Back_LINE(); 
                           
                            if( ALA.all&0x0003ffff ) { DIS.SignL &= ~MODE_; }
                            if(DIS.DisTemp>998 && DIS.DisTemp < 1010)
                            {
                                DIS.DisTemp =1000;
                            }
                            if(DIS.DisTemp <2)
                            {
                               DIS.DisTemp =0;
                            }                            
                            Back_Sign();                            
                            //--------------------------------------------------------------                            
                            DIS.SignR |= MODE_PERC; //此句不知之前为何不要？chen0917                            
                           
                        }
                        //if( g_PIDServiceData.SetPointDis/1000)
                        //{
                        //    DIS.Small_Dis[0] = (g_PIDServiceData.SetPointDis/10) %10;
                        //    DIS.Small_Dis[1] = (g_PIDServiceData.SetPointDis /100)%10;
                        //    DIS.Small_Dis[2] = g_PIDServiceData.SetPointDis /1000;
                        //}
                        //else if(g_PIDServiceData.SetPointDis/100)
                        //{
                        //    DIS.Small_Dis[0] = (g_PIDServiceData.SetPointDis /10)%10;
                        //    DIS.Small_Dis[1] = (g_PIDServiceData.SetPointDis /100)%10;
                        //    DIS.Small_Dis[2] = 38;
                        //}
                        //else 
                        //{
                        //    DIS.Small_Dis[0] =  (g_PIDServiceData.SetPointDis /10)%10;
                        //    DIS.Small_Dis[1] = 38;
                        //    DIS.Small_Dis[2] = 38;                       
                        //}
                        //Y:20230417 增加小数显示
                        g_PIDServiceData.SetPointDis -=5;
                        if (g_PIDServiceData.SetPointDis / 1000)
                        {
                            DIS.Small_Dis[0] = (g_PIDServiceData.SetPointDis / 10) % 10;
                            DIS.Small_Dis[1] = (g_PIDServiceData.SetPointDis / 100) % 10;
                            DIS.Small_Dis[2] = g_PIDServiceData.SetPointDis / 1000;
                        }
                        else if (g_PIDServiceData.SetPointDis / 100)
                        {
                            DIS.Small_Dis[0] = g_PIDServiceData.SetPointDis % 10;
                            DIS.Small_Dis[1] = (g_PIDServiceData.SetPointDis / 10) % 10;
                            DIS.Small_Dis[2] = (g_PIDServiceData.SetPointDis / 100) % 10;
                        }
                        else
                        {
                            DIS.Small_Dis[0] = g_PIDServiceData.SetPointDis % 10;
                            DIS.Small_Dis[1] = (g_PIDServiceData.SetPointDis / 10) % 10;
                            DIS.Small_Dis[2] = 38;
                        }
                        g_PIDServiceData.SetPointDis +=5;
                        //小数点修改结束
                                          
                      }  break;
        case MAN    : {
                            if(SYS.bit.PageS1==1) 
                            {
                              /*
                              //------------------------------------------------
                              if( VAL.V_p1_1_RCR )
                              {  
                                 if(OP.Meterage_L > OP.Middle_Valve)  
                                 {  
                                    DIS.SignL &= ~MODE_;
                                    DIS.DisTemp = (float)(OP.Meterage_L - OP.Middle_Valve)/(float)( OP.Up_4_2 - OP.Middle_Valve)*6000;
                                 }
                                 else
                                 {
                                     DIS.SignL |= MODE_;
                                     DIS.DisTemp = (float)(OP.Middle_Valve - OP.Meterage_L)/(float)(OP.Middle_Valve - OP.Down_4_1)*6000;
                                 }
                              }
                              else
                              {
                                 if(OP.Meterage_L > OP.Middle_Valve)  
                                 {  
                                    DIS.SignL &= ~MODE_;
                                    DIS.DisTemp = (float)(OP.Meterage_L - OP.Middle_Valve)/(float)(OP.Up_4_2 - OP.Middle_Valve)*3000;
                                 }
                                 else
                                 {
                                     DIS.SignL |= MODE_;
                                     DIS.DisTemp = (float)(OP.Middle_Valve - OP.Meterage_L)/(float)(OP.Middle_Valve - OP.Down_4_1)*3000;
                                 }
                              }
                               //------------------------------------------------
                              */
                                if(OP.Meterage_L > 2048)  
                                {  
                                    DIS.SignL &= ~MODE_;
                                    DIS.DisTemp = (float)(OP.Meterage_L - 2048)/2048*900;
                                }
                                else
                                {
                                    DIS.SignL |= MODE_;
                                    DIS.DisTemp = (float)(2048 - OP.Meterage_L)/2048*900;
                                }                             
                              

                               Digital_Dis4B(41,DIS.DisTemp); 
                               DIS.Point = 4; // DIS.SignR &= ~MODE_PERC; 
                            }    
                            else
                            {                               
                                Back_LINE(); 
                                if( ALA.all&0x0003ffff ) { DIS.SignL &= ~MODE_; }
                                
                                Back_Sign();  
                            }
                       }break;      
        case CONFIG : {
                          if( SYS.bit.PageS1==PG8&&SYS.bit.PageS2==P8_4_MIN)                                        //数字显示
                          {
                              DIS.Point = 4;
 
                                   Digital_Dis4B(4,AD.CurrentDis);                                 
                          }
                          else if(SYS.bit.PageS1==PG8&&SYS.bit.PageS2==P8_5_MAX)
                          {  
                              DIS.Point = 4;
                                   Digital_Dis4B(4,AD.CurrentDis);   
                          }   
                          else if( SYS.bit.PageS1==PG1&&SYS.bit.PageS2==P1_4STEP )
                          {  
                              if( SYS.bit.Step == STP_2 )
                              {
                                  if( OP.p1_4 > 0 )
                                  { 
                                      DIS.SignL &= ~MODE_;
                                      
                                  }
                                  else
                                  { 
                                      DIS.SignL |=  MODE_;
                                      OP.p1_4 = ( ~OP.p1_4 ) + 1; 
  
                                  }
                                      DIS.Point = 2;
                                      Digital_Dis4B(4,OP.p1_4); 
                                      if( (OP.p1_4/1000)%10 == 0 )                                           
                                      DIS.Digital_Dis[0] = NULL_BIG; 
                                      if( (OP.p1_4/100)%10 == 0  )   
                                      {
                                         Digital_Dis4B(21,OP.p1_4); 
                                         DIS.Point = 1;
                                      }                                     
                              }  
                          }  
                          else if( SYS.bit.PageS1==PG3&&SYS.bit.PageS2==P3_1_MIN )
                          {   
                              if( SYS.bit.Step == STP_2 )
                              {
                                      DIS.SignL &= ~MODE_; 
                                      DIS.Point = 2;
                                      Digital_Dis4B(4,OP.p1_4); 
                                      if( (OP.p1_4/1000)%10 == 0 )                                           
                                      DIS.Digital_Dis[0] = NULL_BIG; 
                                      if( (OP.p1_4/100)%10 == 0  )   
                                      {
                                         Digital_Dis4B(21,OP.p1_4); 
                                         DIS.Point = 1;
                                      }    
                                      
                                      //DIS.SignR |= MODE_DEGR; 
                                       
                                     
                              } 
                              if( SYS.bit.Step == STP_4 )
                              {
                                      DIS.Point = 4;
                                      Digital_Dis4B(4,VAL.V_p3_1_MIN);                                       
                              }  
                          }  
                          else if( SYS.bit.PageS1==PG3&&SYS.bit.PageS2==P3_2_MAX )
                          {  
                              if( SYS.bit.Step == STP_2 )
                              {
                                      DIS.SignL &= ~MODE_;
                                      DIS.Point = 2;
                                      Digital_Dis4B(4,OP.p1_4); 
                                      if( (OP.p1_4/1000)%10 == 0 )                                           
                                      DIS.Digital_Dis[0] = NULL_BIG; 
                                      if( (OP.p1_4/100)%10 == 0  )   
                                      {
                                         Digital_Dis4B(21,OP.p1_4); 
                                         DIS.Point = 1;
                                      }   
                              }
                              if( SYS.bit.Step == STP_4 )
                              {
                                      DIS.Point = 4;
                                      Digital_Dis4B(4,VAL.V_p3_2_MAX);                                       
                              }  
                          }  
                          else if( SYS.bit.PageS1==PG5 )
                          {  
                              if( SYS.bit.PageS2 > P5_12_NULL && SYS.bit.PageS2 < P5_11RST )
                              { 
                                 if( SYS.bit.Dida && KB.bit.KeyOK==0 && CLK.SecKB==0 )  
                                 {   
                                      DIS.Small_Dis[3] = DIS.Small_Dis[2] =  DIS.Small_Dis[1] = DIS.Small_Dis[0] = 38;        
                                 }
                                 else              
                                 { 
                                      DIS.Small_Dis[0] = KB.bit.CountKB%10;
                                      DIS.Small_Dis[1] = (KB.bit.CountKB/10)%10;
                                      DIS.Small_Dis[2] = (KB.bit.CountKB/100)%10;
                                      DIS.Small_Dis[3] = (KB.bit.CountKB/1000)%10;
                                 } 

                                    Back_ConfigSign();
                                 } 
                                  //-------------------------------------------
                             //}
                          } 
                          else if( SYS.bit.PageS1==PG2 && SYS.bit.PageS2==P2_9TEST )
                          {
                                if( SYS.bit.Step > STP_0 && SYS.bit.PageS2 < SP_18 )
                                {  
                                   Back_ConfigSign();
                                }    
                          
                          }  
                          else
                          {  
                              //DIS.Point = 4;
                              Digital_Dis4B(4,KB.bit.CountKB);                               
                              if(DIS.Digital_Dis[0]==0)
                              {
                                 DIS.Digital_Dis[0] = NULL_BIG;
                                 if(DIS.Digital_Dis[1]==0)
                                 {
                                    DIS.Digital_Dis[1] = NULL_BIG;
                                    if(DIS.Digital_Dis[2]==0&&DIS.Point == 4) DIS.Digital_Dis[2] = NULL_BIG;
                                 }     
                              }
                          }    
                      }break;

        default : break;
    }
}
