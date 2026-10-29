# include "Device.h"


void AD421_Initial(void)
{
    AD421_DIR |= ( LATCH_AD421 | DATA_AD421 | CLOCK_AD421 );
    AD421_SEL &= (~LATCH_AD421);
    AD421_SEL &= (~DATA_AD421);
    AD421_SEL &= (~CLOCK_AD421);
    AD421_PORT = 0xFF; //先输出低电平 
}
//-----------------------------------------------------------------------------
/*
void SEND_AD421(void)     //AD5421驱动
{ 
     unsigned int data_temp,loop=0; 
     unsigned int data_temp0=0;
     unsigned char data_temp1 =0;
     data_temp0= DIS.CurrentBackDis-3;
     //if(OP.Difference_Ex <VAL.V_p1_3BRND)
     //  {
     //   data_temp0 =OP.Expectation;
     //  }
     if(data_temp0 >OP.Up_4_2) {data_temp0 =OP.Up_4_2; }
     if(data_temp0 <OP.Down_4_1) {data_temp0 =OP.Down_4_1; }      
     
    // data_temp = (float)(AD.CurrentBack - OP.Down_4_1)/(float)(OP.Up_Limited - OP.Down_4_1)*1000;
     data_temp = (float)(data_temp0 - OP.Down_4_1)/(float)(OP.Up_4_2 - OP.Down_4_1)*1000;

    // if(AD.CurrentBack >OP.Up_4_2) { data_temp =998; }
    // if(AD.CurrentBack <OP.Down_4_1) { data_temp =0; }   //为解决电位器反馈值在超范围 20130407
     if(VAL.V_p7_3_ACT == V_REVR) {  data_temp = 1000 - data_temp;  }
     if(VAL.V_p3_3_RCT ==0 )      {  data_temp = 1000 - data_temp;  }
     if( data_temp > 998 ) {data_temp = 998; }
     if( data_temp <1 ) {data_temp = 0; }
     data_temp =  410.0*(data_temp/1000.0)*(VAL.V_p7_2_MAX-40) ; 
     data_temp +=  410.0*(VAL.V_p7_1_MIN-40);
     //AD421_PORT |=LATCH_AD421; 
     //AD421_PORT &=~LATCH_AD421; 
     //delay_ns(30);
    // data_temp  =0x7e65;
     data_temp1 =0x01;
     AD421_PORT |=LATCH_AD421; 
     delay_ns(30);
     for(loop=0;loop<8;loop++)   //发送指令
     {
        AD421_PORT &= (~CLOCK_AD421);     // clk 拉高       
        delay_ns(30);
        if((data_temp1&0X80)==0)   
        {
            AD421_PORT|=DATA_AD421;   
               
        }
        else
        {
            AD421_PORT&=~DATA_AD421;
        }
        delay_ns(30);
        AD421_PORT|=CLOCK_AD421;            
        delay_ns(30);
        data_temp1<<=1 ;       

      }
     for(loop=0;loop<16;loop++)   //发送数据
     {
        AD421_PORT &= (~CLOCK_AD421);     // clk 拉高       
        delay_ns(30);
        if((data_temp&0X8000)==0)   
        {
            AD421_PORT|=DATA_AD421;   
               
        }
        else
        {
            AD421_PORT&=~DATA_AD421;
        }
        delay_ns(30);
        AD421_PORT|=CLOCK_AD421;            
        delay_ns(30);
        data_temp<<=1 ;       

      } 
 
      delay_ns(30);
      AD421_PORT &=~LATCH_AD421; 
      delay(10);
      AD421_PORT |=LATCH_AD421;

}
*/


//-----------------------------------------------------------------------------
void SEND_AD421(void)
{ 
     unsigned int data_temp,loop=0; 
     unsigned int data_temp0=0;
     data_temp0= AD.CurrentBack;
     //if(OP.Difference_Ex <VAL.V_p1_3BRND)
     //  {
     //   data_temp0 =OP.Expectation;
     //  }
     if(data_temp0 >OP.Up_4_2) 
     {
       //data_temp0 =OP.Up_4_2; 
       //data_temp = 998;
       data_temp = 1000;
     }
     else if(data_temp0 <OP.Down_4_1) 
     {
       //data_temp0 =OP.Down_4_1; 
      // data_temp = 4;
       data_temp = 0;
     } 
     else     
     
    // data_temp = (float)(AD.CurrentBack - OP.Down_4_1)/(float)(OP.Up_Limited - OP.Down_4_1)*1000;
     {
       if( (VAL.V_p1_1_RCR&0x01) == V_LINE )
       {       

//         g_dangel_now = AD.CurrentBack;
//         g_dangel_now = (double)(g_dangel_now - 2048)/4096*3.141592653; 
//         g_dsinAngel_now = sin(g_dangel_now);   
         LineCalculate(); 
         g_PIDServiceData.Cur_ValvePoint = (g_dsinAngel_now - g_dsinAngel_down)*1000.0/(g_dsinAngel_up - g_dsinAngel_down);         
         data_temp = g_PIDServiceData.Cur_ValvePoint;        
       }
       else
       {
         data_temp = (float)(data_temp0 - OP.Down_4_1)/(float)(OP.Up_4_2 - OP.Down_4_1)*1000.0;
       }
     }

     if(VAL.V_p7_3_ACT == V_REVR) {  data_temp = 1000 - data_temp;  }
     if(VAL.V_p3_3_RCT ==1 )      {  data_temp = 1000 - data_temp;  }

      data_temp = data_temp*0.992 +4 ;
      TACCR1 = ( unsigned long )TACCR0*data_temp/1000;
      TACCR2 = 0;
      TACTL|=MC0; 
}

/*
void AD421_Initial(void)
{
    AD421_DIR |= ( LATCH_AD421 | DATA_AD421 | CLOCK_AD421 );
    AD421_SEL &= (~LATCH_AD421);
    AD421_SEL &= (~DATA_AD421);
    AD421_SEL &= (~CLOCK_AD421);
    AD421_PORT = 0x00; //先输出低电平 
}
//-----------------------------------------------------------------------------
void SEND_AD421(void)
{ 
     unsigned int data_temp,loop=0; 
     data_temp = (float)(AD.CurrentBack - OP.Down_4_1)/(float)(OP.Up_Limited - OP.Down_4_1)*1000;
     if( data_temp > 999 ) {data_temp = 999; }
     if(VAL.V_p7_3_ACT == V_REVR) {  data_temp = 1000 - data_temp;  }
     data_temp =  410.0*(data_temp/1000.0)*(VAL.V_p7_2_MAX-40) ;
     data_temp +=  410.0*(VAL.V_p7_1_MIN-40);
     AD421_PORT &=~LATCH_AD421; 
     delay_ns(30);
     for(loop=0;loop<16;loop++)
     {
        AD421_PORT &= (~CLOCK_AD421);            
        delay_ns(30);
        if((data_temp&0X8000)==0)   
        {
           AD421_PORT&=~DATA_AD421;     
        }
        else
        {
           AD421_PORT|=DATA_AD421;   
        }
        delay_ns(30);
        AD421_PORT|=CLOCK_AD421;            
        delay_ns(30);
        data_temp<<=1 ;
      } 
      delay_ns(30);
      AD421_PORT |=LATCH_AD421;              
      delay_ns(30);
      AD421_PORT &=~LATCH_AD421; 
  
}
*/
//-----------------------------------7_4测试的时候要用的东东
void SEND_AD421_Test( unsigned int data_temp )
{ 
     unsigned int loop=0;
     AD421_PORT |=LATCH_AD421; 
     delay_ns(30);
     for(loop=0;loop<16;loop++)
     {
        AD421_PORT|=CLOCK_AD421;            
        delay_ns(30);
        if((data_temp&0X8000)==0)   
        {
           AD421_PORT|=DATA_AD421;           
        }
        else
        {
            AD421_PORT&=~DATA_AD421;            
        }
        delay_ns(30);
        AD421_PORT&=~CLOCK_AD421;            
        delay_ns(30);
        data_temp<<=1 ;
      } 
      delay_ns(30);
      AD421_PORT &=~LATCH_AD421;              
      delay_ns(30);
      AD421_PORT |=LATCH_AD421; 
}

