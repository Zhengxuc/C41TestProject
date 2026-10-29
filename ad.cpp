# include "Device.h"
# include <msp430x16x.h>
AD_REG AD;


unsigned int Initdata[4];
unsigned int result_gd=0;
unsigned int Current_F;
unsigned int Current_L; //
unsigned int Current_Difference;
unsigned int Max_AD;
unsigned int Min_AD;
unsigned int Current_AD;
float Time_MeaSample[10];//时间测量中的样本
float Average_Time;     //整定出来的平均时间
float Sample_Time;  //自动测量模式下的时间

void Calculate_ADpp(char pp)
{
  
    ADC12_Convert(0);
      if(pp==MAX_PP)
          Max_AD= Current_AD;
      else 
          Min_AD= Current_AD;


}

//判断AD值是否到达峰值的容差范围
char Affirm_ADpp(char pp)
{
  
    ADC12_Convert(0);
    if(pp==MAX_PP)
    {
        if(Current_AD>=(Max_AD*0.98) && Current_AD<=(Max_AD*1.02))
          return 1;
        else return 0;
    }
    else
    {
        if(Current_AD>=(Min_AD*0.98) && Current_AD<=(Min_AD*1.02))
          return 1;
        else return 0;
    
    }
    
}


//计算10次测量的平均时间
float Calculate_AverageTime(void)
{
  char i;
  float Sum_Time=0;
 
  for(i=0;i<10;i++)
  {
      Sum_Time+=Time_MeaSample[i];
      
  }
   
  return Sum_Time/10;

}

/**************起始信号***************/
void start()
{       
        SDA_Out;
        _NOP();
	SDA_L;
        SCL_Out;
	SCL_H;
	SDA_H;
	SDA_L;  
	SCL_L;
}
/*************终止信号*************/
void stop()
{
        SDA_Out;
        _NOP();
	SDA_L;
	SCL_H;
	SDA_H;
}

/*************向I2C总线发送一个字节************/
void Send1byte(unsigned int byte)
{
	unsigned int i;
        ack();
        SDA_Out;
	for(i=0;i<8;i++)
	{
	   if((byte<<i) & 0x80)
		   SDA_H;
	   else
		   SDA_L;
	   SCL_H;
           SCL_L;
	 }
  SDA_H;
  _NOP();
  SCL_H;
  SCL_L;
}

/**************读取一个字节**************/
unsigned int Read1byte()
{
    unsigned int temp=0,i;
    SDA_In;
    _NOP();
    for(i=0;i<8;i++)
    {
    	temp=temp<<1;
    	SCL_L;
        SCL_H;
        if(P2IN&0x02)
        //if(P2IN&0x04)
        {
        	temp|=0x01;
        }

    }
//    ack();
    SDA_Out;
    SCL_L;
    SDA_L;
    SCL_H;
    SCL_L;
    SDA_H;
    return temp;
}

void ack ()
{
       unsigned int i;
       i =0;
      // SDA_In;
      // SCL_H;
       //delay_ns(15);
       while((P2IN&BIT1)&&(i<80))i++;
     // SDA_Out;
     //  SCL_L;
     //  delay_ns(15);
}



/*******************Config寄存器*********************/
void Confige_1115(unsigned char channel )
{
    unsigned int i=0;
    switch(channel)
    {
    case 0:  Initdata[2] = 0xc4; break;
    case 1:  Initdata[2] = 0xd4; break;
    case 2:  Initdata[2] = 0xe4; break;
    case 3:  Initdata[2] = 0xf4; break; 
    }
    Initdata[0] =0x90;       // 启动配置命令
    Initdata[1] =0x01;       // 指向配置寄存器
    Initdata[3] =0xe3;       // 配置字低字节
    SCL_H;
    start();
    for(i=0;i<4;i++)
    {
    	  Send1byte(Initdata[i]);
    	  _NOP();
    }
    ack();
    stop();
}
/************指向转换结果寄存器***************/
void Point_reg(void)
{
	unsigned char i=0;
	Initdata[0] =0x90;           // 地址 ＋ 写命令
	Initdata[1] =0x00;            // 指向转换结果寄存器
	  SDA_Out;
	  SCL_H;
           ack();
	  start();
	  for(i=0;i<2;i++)
	    {
                  //ack();
		  Send1byte(Initdata[i]);
		  _NOP();
                 // ack();
	    }
          ack();
	  stop();
}
/**************读取转换结果***************/
unsigned int Read_1115(void)
{
  unsigned char   ResultL,ResultH;
  unsigned int    Result;
  SCL_H;
  ack();
  start();
  Send1byte(0x91);
  ResultH=Read1byte();//高8位
  ResultL=Read1byte();//低8位
  ack();
  stop();
  Result=ResultH*256+ResultL;
  return   Result;
}



//------------------------------------------------------------------------------
//        初始化ADC12
//------------------------------------------------------------------------------
void ADC12_Initial(void) 
{
  P2SEL &= ~BIT0; 
  P2SEL &= ~BIT1; 
  P2IE &= ~BIT0; 
  P2IE &= ~BIT1; 
  SCL_Out;
  SDA_In;
  //------------------------------------以下是对DAC进行初始化
  //ADC12CTL0 = REF2_5V+REFON;                    // Internal 2.5V ref on    
  //DAC12_1CTL = DAC12IR + DAC12AMP_5 + DAC12ENC;   // Internal ref gain 1   
  //DAC12_1DAT = 0x0fff;  
  
  ADC12CTL0 =  REFON + REF2_5V;                    // Internal 2.5V ref on     //ADC12CTL0 =  REFON;                    // Internal 2.5V ref on    
  DAC12_1CTL = DAC12IR + DAC12AMP_2 + DAC12ENC ;   // Internal ref gain 1   
  DAC12_0CTL = DAC12IR + DAC12AMP_2 + DAC12ENC ; 
 
  
  //ADC12CTL0 =  REFON;                    // Internal 2.5V ref on    
  //DAC12_1CTL = DAC12IR + DAC12AMP_2 + DAC12ENC ;   // Internal ref gain 1   
 // DAC12_0CTL = DAC12IR + DAC12AMP_2 + DAC12ENC ; 

}

//---------------------------------------------
// 标准排序算法  1  冒泡
//---------------------------------------------
void Bubble_Sort( unsigned int * list, unsigned int length ) 
{
      //---------------------
      unsigned int i,j,flg = 1,Tmp;
	  //-------------------------------
      for(j = 0; j < length - 1 ; j++ )
      {
	  		for(i = 0; i < length - j ; i++ )
      		{
          		if( list[i] > list[i + 1] )   
          		{
               		Tmp = list[i];
               		list[i] = list[i + 1];
               		list[i + 1] = Tmp;  
					flg = 0;
          		}

      		}
			if( flg ) { break;  }
			else      { flg = 1;}
      }		
}

void ADC12_Convert( unsigned char mode )
{  
   unsigned char  i;
   unsigned char  j;
   unsigned long t = 0;

    _NOP();
       Confige_1115(2) ;
       delay(2);
       Point_reg();    
       delay(2);
       
       
       Current_AD =Read_1115();
      
    _NOP();              
  
       
       
       
//       Confige_1115(0) ;
//       delay(2);
//       Point_reg();   
//       delay(2);
// 
//      AD.CurrentIn =  Read_1115();
//      
//      AD.CurrentIn  =AD.CurrentIn*0.23;
//       
//    
//    AD.CurrentDis =AD.CurrentIn;
//    _NOP();
//
//   if( mode==1 )
//   {  
//      OP.Meterage_F  = OP.Meterage_L;      // 保存上次采样
//      OP.Meterage_L  = AD.CurrentBack;     // 保存当前采样
//      OP.Difference_F = OP.Difference;     // 保存上次偏差     
//      if (OP.Meterage_L > OP.Meterage_F)   { OP.Difference_Direction = 1;  OP.Difference  = OP.Meterage_L - OP.Meterage_F; } //生成当前正向偏差
//      else                                 { OP.Difference_Direction = 0;  OP.Difference  = OP.Meterage_F - OP.Meterage_L; } //生成当前反向偏差
//    
//      Current_F = Current_L;    //保存上次电流值
//      Current_L = AD.CurrentIn; //保存当前电流值
//      if(Current_F >Current_L)     { Current_Difference = Current_F -Current_L; }
//      else                         { Current_Difference = Current_L -Current_F; }
//      if(Current_Difference > 11)  { CLK.CurrentInRef =2;  }
//   
//   }
}

