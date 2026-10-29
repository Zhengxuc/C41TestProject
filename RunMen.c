#include "Device.h"
//------------------------------------------------------------------------------

void Man_IP(void)
{
    if(KB.bit.LongKB)  //说明有按键按下去了
    {  
        if(KB.bit.Derection)
        {
                if(CLK.SecKB > 2)
                 {
                  //  OutPut_Positive(VAL.V_p5_1__K1); // delay( VAL.V_p5_5__K5*3);  OutPut_Negetive( 0 ); 
                      OutPut_Positive(950);
                 }
                else
                 {  
                  //OutPut_Positive(VAL.V_p5_1__K1);  delay( KB.bit.CountKB );  OutPut_Negetive( 0 );  delay(60);  //150
                  // OutPut_Positive(VAL.V_p5_1__K1);  //delay( KB.bit.CountKB );  OutPut_Negetive( 0 );  //delay(60);  //150
                    OutPut_Positive(950);  
                 }  
        }
        else
        {
             if(CLK.SecKB > 2 )
             {
                    // OutPut_Negetive(VAL.V_p5_1__K1); // delay( VAL.V_p5_6__K6*3);  OutPut_Negetive( 0 ); 
                   OutPut_Negetive(950);
             }
             else
             {
               // OutPut_Negetive(VAL.V_p5_2__K2);  delay( KB.bit.CountKB );  OutPut_Negetive( 0 );  delay(60);   //150
                //OutPut_Negetive(VAL.V_p5_2__K2); // delay( KB.bit.CountKB );  OutPut_Negetive( 0 );  //delay(60);   //150
                   OutPut_Negetive(950); 
             }  
        }
    }
    else              //停止动作
    {  
        if( KB.bit.CountKB > 800 )
        { 
          OutPut_Positive(0);
        }  
        else
        {
          OutPut_Positive(0);
        }   
    }
    ADC12_Convert(1);
}


/*
void Man_IP(void)
{
    if(KB.bit.LongKB)  //说明有按键按下去了
    {  
        if(KB.bit.Derection)
        {
           if(CLK.SecKB == 3)
           { 
              OutPut_Positive(VAL.V_p5_1__K1);  delay(150);  OutPut_Negetive( 0 );  delay(300);
           }
           else if(CLK.SecKB == 2)
           {
              OutPut_Positive(VAL.V_p5_1__K1);  delay(200);  OutPut_Negetive( 0 );  delay(300);
           }  
           else if(CLK.SecKB == 1)
           {
              OutPut_Positive(VAL.V_p5_1__K1);  delay(250);  OutPut_Negetive( 0 );  delay(300);
           }
           else
           {
              OutPut_Positive(VAL.V_p5_1__K1);  delay(300);  OutPut_Negetive( 0 );  delay(300);
           }
        }
        else
        {
           if(CLK.SecKB == 3)
           { 
              OutPut_Negetive(VAL.V_p5_2__K2);  delay(150);  OutPut_Negetive( 0 );  delay(300); 
           }
           else if(CLK.SecKB == 2)
           {
              OutPut_Negetive(VAL.V_p5_2__K2);  delay(200);  OutPut_Negetive( 0 );  delay(300);
           }  
           else if(CLK.SecKB == 1)
           {
              OutPut_Negetive(VAL.V_p5_2__K2);  delay(250);  OutPut_Negetive( 0 );  delay(300);
           }
           else
           {
              OutPut_Negetive(VAL.V_p5_2__K2);  delay(300);  OutPut_Negetive( 0 );  delay(300);
           }
        }
    }
    else              //停止动作
    {
         OutPut_Positive(Minimum_Speed); 
    }
    ADC12_Convert(1);
}
*/

