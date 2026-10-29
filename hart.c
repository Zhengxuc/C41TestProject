
//-------------------------------------------------------------------------------------------------
//# include <msp430x14x.h>
#include "Device.h"

extern void uart0_init();   //串口初始化 波特率9600
extern void hart_task (void);
//extern void clock_init();   //时钟初始化
//extern void timer_A_init();  //定时器A初始化 增模式 定时时间30ms
extern unsigned char Checksum(unsigned char *s , unsigned char nbytes);//计算校验和
extern void com_send(unsigned char *send, unsigned char send_bytes);//通过串口发送数据
extern void hart_resp( unsigned char *resp_data, unsigned char resp_num, unsigned char *addr ,unsigned char cmd_num);//按HART协议的格式生成待发送的数据帧send[]
extern unsigned int strncmp(const char *cs, const char *ct, unsigned char count);
unsigned char  x1;               //用于区别长结构和短结构的变量，x1＝1 短，x1=2时长
unsigned char receive[100];     //用于存放接收的数据 
unsigned char send[100];        //用于存放发送的数据 
unsigned char cmd_data[25];     //命令数据区
unsigned char byte_count;       //数据长度
unsigned char start_short[3]={0xFF,0xFF,0x02};   //用于寻找短结构起始字节
unsigned char start_long[3]={0xFF,0xFF,0x82};    //用于寻找长结构起始字节
unsigned char rec_ptr;           //用于给接收到的数据编号
unsigned char received;          //recived＝1时表示已接收到一帧数据，对数据帧解析完recived被置为0        
unsigned char rec_bytes;         //接收的字节数
unsigned char error,comm_error,command_error,old_device,needs_burn;
unsigned char polling_addr;     //轮询地址,初始值为0
unsigned char *p_polling_addr=&polling_addr;//指向轮询地址的指针
//unsigned char short_addr = 0x80 ;     //本机短结构地址，初始值为0x80
unsigned char short_addr = 0x00 ;     //本机短结构地址，ABB握手时的值
unsigned char long_addr[5]={0x16,0x41,0x06,0x04,0xc8};   //本机长结构地址
unsigned char cmd_num;          //命令号
unsigned char pre_num;         //前导符的个数
unsigned char status1,status2;  //状态位
unsigned char send_bytes;       //发送数据的字节数
unsigned char not_resp;         //表示从机不需要响应的标志，该变量为1时表示不响应
unsigned char pv_unit_code =0x39;
unsigned char v2_unit_code =0x27;
unsigned char v3_unit_code =0x20;
unsigned char v4_unit_code =0x39;

//--------------------------------------------------------------------------------------------------
//命令响应数据填充区
unsigned char resp_data_0[14] ={0xfe,0x16,0x41,0x07,0x05,0x01,0x1e,0x18,0x02,0x06,0x04,0xc8};
unsigned char resp_data_1[5];
unsigned char resp_data_13[20];
unsigned char resp_data_3[42];
unsigned char resp_data_2[8] ={0x7F,0xa0,0x00,0x00,0x7F,0xa0,0x00,0x00};

unsigned char tag[6] ={0x82,0x08,0x20,0x82,0x08,0x20}; //标签
unsigned char descriptor[12] ={0x82,0x08,0x20,0x82,0x08,0x20,0x82,0x08,0x20,0x82,0x08,0x20};  //描述符
unsigned char date[3] ={0x00,0x00,0x00};
unsigned char message[24] ={0x82,0x08,0x20,0x82,0x08,0x20,0x82,0x08,0x20,0x82,0x08,0x20,0x82,0x08,0x20,
                           0x82,0x08,0x20,0x82,0x08,0x20,0x82,0x08,0x20};
unsigned char status[25] ={0x00,0x00 ,0x00,0x00,0x8F,0x00,0x13,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00 ,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x10 };//48，读附加设备信息
unsigned char Actuator_inf[5] ={0x00 ,0x00 ,0x01 ,0x00 ,0x00};
unsigned char sensor[12]={0x0C,0x87,0x40,0x02,0x3C,0x00,0x00,0x00,0x00,0x0C,0x87,0x40};
unsigned char zero_point[1] ={0x01};  //141 
unsigned char switch_code[1] ={0x10}; //143
unsigned char counter_values[29]={0x00,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x00,
                                0x00,0x00,0x1C,0x02,0x00,0x00,0xA0,0x86,0x01,0x00,0x40,0x42,0x0F,0x00,0x32};  //146
unsigned char static_variables[19]={0x27,0x41,0xA0,0x00,0x00,0x40,0x80,0x00,0x00,0x00,0x3E,0x4C,0xCC,0xCC,0x00 
                                  ,0x00,0x09,0x00,0x00 }; //151 读静态变量
unsigned char alarm_range[8] ={0x00,0x00,0x00,0x00,0x42,0xC8,0x00,0x00};//153读报警范围值
unsigned char valve_range[4] ={0x00,0x00,0x10,0x27};//167 读阀门范围值
unsigned char setpoint_cutoff[2] ={0x64,0x00};      //169 读定位点截止值
unsigned char stoke_time[5] ={0x00,0x00,0x00,0x00,0x00}; //171 读往返行程时间
unsigned char travel[4] ={0x00,0x00,0x10,0x27};  // 173  读阀行程范围
unsigned char alarm_mark[1] ={0x00};
unsigned char readback_range[6] ={0x00,0x00,0x10,0x27,0x00,0x00};      //177 读回读范围
unsigned char control_data1[20] ={0x32,0x00,0x32,0x00,0xC8,0x00,0xC8,0x00,0x00,0x00,0x6B,0x6B,0x03,
                            0x00,0x00,0x00,0x00,0x00,0x00,0x00};       //180  读控制参数
unsigned char digital_setpoint[6] ={0x00,0x00,0x00,0x00,0x00,0x00 };     //189   
unsigned char setpoint_mode[4] ={0x00,0x00,0x00,0x00};    //191 获取定位点模式
unsigned char digital_input_mode[4] ={0x00,0x00,0x00,0x00};         //193 获取数字输入功能
unsigned char actuator[4] ={0x00,0x00,0x00,0x00};     //195  获取执行器类型
unsigned char measering_task[24] ={0x82,0x08,0x20,0x82,0x08,0x20,0x82,0x08,0x20,0x82,0x08,0x20,0x82,
                                   0x08,0x20,0x82,0x08,0x20,0x82,0x08,0x20,0x82,0x08,0x20};        //204 读应用描述

unsigned char final_ass_num[3] ={0x00,0x00,0x00};
unsigned char resp_num_13 =0x15;
unsigned char resp_num_3 =0x18;





unsigned char *p_pv_value ;
unsigned char *p_pv_current ;  //主变量电流值
unsigned char *p_pv_upper ;  //主变量上限值
unsigned char *p_pv_lower ;   //主变量下限值
unsigned char *p_v2_value ;
unsigned char *p_v3_value ;
unsigned char *p_v4_value ;
unsigned char *p_pv_sensor_upper ;
unsigned char *p_pv_sensor_lower ;
unsigned char *p_pv_min_span ;
unsigned char *p_pv_damping ;  //将指针和对应的数据值一一对应
        
        
float  pv_value ;
float  pv_current ;  //主变量电流值
float  pv_upper ;  //主变量上限值
float  pv_lower ;   //主变量下限值
float  v2_value ;
float  v3_value ;
float  v4_value ;
float  pv_sensor_upper ;                                      
float  pv_sensor_lower ;
float  pv_min_span ;
float  pv_damping_value ;  //将指针和对应的数据值一一对应

unsigned char pv_upper_754[4];    //主变量量程上限值IEEE754形式
unsigned char pv_lower_754[4];    //主变量量程下限值IEEE754形式
unsigned char pv_current_754[4];  //主变量电流值IEEE754形式
unsigned char pv_754[4];
unsigned char v2_754[4];
unsigned char v3_754[4];
unsigned char v4_754[4];

unsigned char cmd_data_35[9];      //35号命令的数据区
unsigned char e2prom_code;       //EEPROM控制代码

/*

un_DtformConver   DtformConver;
unsigned char i;
int il_Temp32;
float f_Temp;
//十进制float型数据转换成IEEE754标准

 for(i=0;i<4;i++)
 {
    il_Temp32  |= (int32)(DtformConver.uc_Buf[i]<<(i*8));
 }

//IEEE754标准转换成十进制float
il_Temp32 = 0x435C0000;        //实际十进制值是220
 memset((unsigned char*)&DtformConver.uc_Buf[0],0,4);
  for(i=0;i<4;i++)
 {
    DtformConver.uc_Buf[i] = (unsigned char)(il_Temp32>>(i*8));
 }
 f_Temp = DtformConver.ul_Temp;
 
*/

/*主程序运行后,程序一直在等待.当串口接收到一个完整的数据帧之后,received置1,主程序对接收到
的数据帧进行解析,若解析正确,则根据数据帧中的命令号执行相应的操作.*/

void hart_task (void)
{ 
  unsigned char i,j,t;
  unsigned char cs_start;
  unsigned char *r,*p,*b;
  float pv_range_span,f1;   //主变量量程跨度
  //WDTCTL = WDTPW + WDTHOLD;  // Stop WDT
  error = 0;               
  status1 = 0;       
  status2 = 0x40;
  
   //pv_value = (float)(DIS.CurrentBackDis - OP.Down_4_1)/(float)( OP.Up_4_2 - OP.Down_4_1 )*100;
  pv_value = DIS.DisTemp *0.1;  // pv_value = g_PIDServiceData.Cur_ValvePoint *0.1;  //20161022修改
  
  
  if(AD.CurrentDis >VAL.V_p8_4_MIN)
   {
       pv_current = 4.0+16.0/( VAL.V_p8_5_MAX - VAL.V_p8_4_MIN )*(AD.CurrentDis-VAL.V_p8_4_MIN);     //主变量电流值
   }  
   else
   {
       pv_current = 4.0-16.0/( VAL.V_p8_5_MAX - VAL.V_p8_4_MIN )*(VAL.V_p8_4_MIN -AD.CurrentDis);     //主变量电流值
   }
   pv_upper =20.0;                      //主变量电流上限值
   pv_lower = 0.0;                     //主变量电流下限值
   v2_value =0.0;                            //第二变量值
   v3_value =0.0;  
   //v4_value =(float)OP.Difference_Ex/OP.Range_M;;
   v4_value =(g_PIDServiceData.SetPoint- pv_value*10)/10.0;//v4_value =(g_PIDServiceData.SetPoint- g_PIDServiceData.Cur_ValvePoint)/10.0;//20161022修改
   
   //v4_value =(float)(OP.Meterage_L - OP.Expectation)/OP.Range_M;
   pv_sensor_upper =100.0;                    //传感器上限
   pv_sensor_lower =0.0;                      //传感器下限
  // pv_min_span   
  // pv_damping_value  
     
//  for (;;)                             
//  { 
//    while(CLK.received==1)  //当接收到数据时进行以下的处理,否则一直在这里等待
   
   
   
 // hart_resp(resp_data_0, resp_num_0, 0x00,cmd_num);
 // com_send(send,send_bytes);
  if(CLK.received==1) 
  { 
      not_resp=0;
      r=receive;
      for(j=1;j<=rec_bytes;++j)
      {
        if (strncmp((const char*)r,(const char*)start_short,3)==0) 
        {
            x1=1; break;
        }
        if (strncmp((const char*)r,(const char*)start_long,3)==0)
        {
            x1=2; break;
        }
        r++;
      }
      if(j>rec_bytes)          //当程序未找到0x02或0x82时 
      {
        ++error;// break;
      }
      pre_num = j+1;
      j +=2 ; r +=2;              //此时j，r 都指向起始字节
      cs_start = j;
      ++j; ++r;              //指向地址首字节
      if(rec_bytes-j<2)        //当剩余数据数少于3时
      {
        ++error; //break;
      }
      if (x1==1)               //地址为短结构
      {
        if(*r != short_addr)     //短结构地址与本从机短地址不同时
        {
          ++error;// break;
        }
        else
        {
          ++j; ++r;            //指向命令号
        }                 
      }
      if(x1==2)               //地址为长结构
      {
        if(strncmp((const char*)r,(const char*)long_addr,5)!=0)  //短结构地址与本从机短地址不同时
        {
          ++error; //break;
        }
        else
        {
          j += 5; r +=5;           //指向命令号
        }      
      }
      cmd_num = *r;
      j++; r++;               //指向数据长度字节
      byte_count = *r;
      if (byte_count==0) 
      {
        j++; r++;           //指向校验位 
      } 
      else
      { 
        r++;                //指向命令数据的第一个字节
        j += byte_count + 1;
        for(int i=0;i<byte_count;i++)
          cmd_data[i]=*r++;
      }
      r = &receive[cs_start-1];
      if ((Checksum(r, j-cs_start+1)) != 0) ++error;  
      if (error==0) 
      {
 
           p_pv_value = (unsigned char *)&pv_value;
           p_pv_current = (unsigned char *)&pv_current;  //主变量电流值
//           p_pv_upper = (unsigned char *)&pv_upper_value;  //主变量上限值
//           p_pv_lower = (unsigned char *)&pv_lower_value;   //主变量下限值
           p_v2_value = (unsigned char *)&v2_value;
           p_v3_value = (unsigned char *)&v3_value;
           p_v4_value = (unsigned char *)&v4_value;
//           p_pv_sensor_upper = (unsigned char *)&pv_sensor_upper;
//           p_pv_sensor_lower = (unsigned char *)&pv_sensor_lower;
//           p_pv_min_span = (unsigned char *)&pv_min_span;
//           p_pv_damping = (unsigned char *)&pv_damping_value;  //将指针和对应的数据值一一对应
         
           switch(cmd_num)
           {
              case   0: //0号命令组帧和其它命令不同
                          if(x1==1)
                          {
                             hart_resp(resp_data_0, resp_num_0, 0x00,cmd_num);
                          }
                          else
                          {
                             hart_resp(resp_data_0, resp_num_0,long_addr,cmd_num);
                          }                          
                         break;
                    
              case   1: /*将主变量单位代码和主变量值IEEE754形式填充到1号命令响应数据中中*/
                        resp_data_1[0] = pv_unit_code;
 //                     for(i=0;i<4;i++)
                        for(i=0;i<4;i++)
                        //resp_data_1[i+1] = pv_754[i] = *p_pv_value++;
                        resp_data_1[4-i] = pv_754[i] = *p_pv_value++;
                        hart_resp(resp_data_1, resp_num_1,long_addr,cmd_num);
                        break;
                   
              case   2: hart_resp(resp_data_2, 0x08,long_addr,cmd_num);
                        break;
                    
              case   3: /*按命令3响应数据的格式填充相关数据*/
                       for(i=0;i<4;i++)
                       //for(i=3;i>0;--i)
                       {
                         //resp_data_3[i] = v2_754[i] = *p_v2_value++;
                         //resp_data_3[i+5] = pv_754[i] = *p_pv_value++; 
                         //resp_data_3[i+10] = pv_current_754[i] = *p_pv_current++;
                         //resp_data_3[i+15] = v3_754[i] = *p_v3_value++;
                         // resp_data_3[i+20] = v4_754[i] = *p_v4_value++;
                        /* 
                         resp_data_3[4-i] = v2_754[i] = *p_v2_value++;
                         resp_data_3[8-i] = pv_754[i] = *p_pv_value++; 
                         resp_data_3[13-i] = pv_current_754[i] = *p_pv_current++;
                         resp_data_3[18-i] = v3_754[i] = *p_v3_value++;
                        */  
                         resp_data_3[3-i] = v2_754[i] = *p_v2_value++;
                         resp_data_3[8-i] = pv_754[i] = *p_pv_value++; 
                         resp_data_3[13-i] = pv_current_754[i] = *p_pv_current++;
                         resp_data_3[18-i] = v3_754[i] = *p_v3_value++;
                         resp_data_3[23-i] = v4_754[i] = *p_v4_value++;
                        }
                        resp_data_3[4] = pv_unit_code;
                        resp_data_3[9] = v2_unit_code;
                        resp_data_3[14] = v3_unit_code;
                        resp_data_3[19] = v4_unit_code;
                        hart_resp(resp_data_3, resp_num_3,long_addr,cmd_num);
                        break;
/*                    
              case   6: polling_addr = cmd_data[0];
                    short_addr = (0x80|polling_addr);
                    hart_resp(p_polling_addr, 1,long_addr,cmd_num);
                    break;
                    
              case  11: if(strncmp((const char*)cmd_data,(const char*)tag,6)==0)//当主机发来的设备标签和设备本身标签相同时
                      hart_resp(resp_data_0, resp_num_0,long_addr,cmd_num);
                    else
                      not_resp=1;  //当主机发来的设备标签和设备本身标签不相同时,不响应主机的命令
                    break;
*/                    
              case  12: hart_resp(message, 24,long_addr,cmd_num);
                    break;
                    
              case  13: /*将标签,描述符和日期填充到13号命令响应数据中中*/
                    for(i=0;i<6;i++)
                      resp_data_13[i]=tag[i];
                    for(i=6;i<18;i++)
                      resp_data_13[i]=descriptor[i-6];
                    for(i=18;i<21;i++)
                      resp_data_13[i]=date[i-18];
                    hart_resp(resp_data_13,resp_num_13,long_addr,cmd_num);
                    break;
                   /*将传感器序列号等内容填充到14号命令响应数据数组中*/
/*              case  14: for(i=0;i<3;i++)
                      resp_data_14[i] = pv_sensor_sn[i];     //填充主变量传感器序列号
                    resp_data_14[3] = pv_sensor_unit_code;  //填充主变量传感器单位代码
                    for(i=0;i<4;i++)
                    {
                      resp_data_14[i+4] = pv_sensor_upper_754[i] = *p_pv_sensor_upper++;
                      resp_data_14[i+8] = pv_sensor_lower_754[i] = *p_pv_sensor_lower++;
                      resp_data_14[i+12] = pv_min_span_754[i] = *p_pv_min_span++;
                    }           
                    hart_resp(resp_data_14, resp_num_14,long_addr,cmd_num);
                    break;
                    
               case  15: /*填充相关数据*/
/*                    resp_data_15[0] = pv_alarm_code;
                    resp_data_15[1] = pv_xfer_code;
                    resp_data_15[2] = pv_range_unit_code;                   
                    for(i=0;i<4;i++)
                    {
                      resp_data_15[i+3] = pv_upper_754[i] = *p_pv_upper++;                   
                      resp_data_15[i+7] = pv_lower_754[i] = *p_pv_lower++;                
                      resp_data_15[i+11] = pv_damping_754[i] = *p_pv_damping++;
                    }
                    resp_data_15[15] = write_protect_code;
                    resp_data_15[16] = label_distributor_code;           
                    hart_resp(resp_data_15, resp_num_15,long_addr,cmd_num);
                    break;
 */                   
              case  16: hart_resp(final_ass_num, 3,long_addr,cmd_num);
                    break;
/*                    
                case  17: for(i=0;i<byte_count;i++)
                      message[i]=cmd_data[i]; //将主机发来的设备信息写入设备
                    hart_resp(message,24,long_addr,cmd_num);
                    break; 
                    
               case  18: /*将主机发来的标签,描述符和日期写入设备*/
/*                    for(i=0;i<6;i++)
                      tag[i]=cmd_data[i];
                    for(i=6;i<18;i++)
                      descriptor[i-6]=cmd_data[i];
                    for(i=18;i<21;i++)
                      date[i-18]=cmd_data[i];
                    hart_resp(cmd_data,21,long_addr,cmd_num);
                    break;     
                    
/*               case  19: for(i=0;i<byte_count;i++)
                      final_ass_num[i]=cmd_data[i]; //将最终装配号写入设备
                    hart_resp(final_ass_num,3,long_addr,cmd_num);
                    break;
                    
                 case  33: if(strncmp((const char*)cmd_data,(const char*)xmtr_var_code,4)!=0)
                           not_resp++;
                           else
                             { /*将几个数据和数组按命令33响应的数据格式封装*/
/*                               for(i=0;i<4;i++)
                             {
                            resp_data_33[6*i] = xmtr_var_code[i];
                            resp_data_33[6*i+1] = unit_code_slot[i];
                            for(t=0;t<4;t++)                       
                            resp_data_33[6*i+2+t] = *p_xvd[i]++;                                            
                             }
                            p_xvd[0] = xmtr_var_data_0;
                            p_xvd[1] = xmtr_var_data_1;
                            p_xvd[2] = xmtr_var_data_2;
                            p_xvd[3] = xmtr_var_data_3;        //指针数组复位                      
                            hart_resp(resp_data_33,24,long_addr,cmd_num);
                             }
                           break;
                    
                case  34: hart_resp(pv_damping_754,4,long_addr,cmd_num);
                    for(i=0;i<4;i++)
                      *p_pv_damping++ = pv_damping_754[i] = cmd_data[i];  //将主变量阻尼值写入设备
                    break;   
                    
               case  35: /*将主变量单位代码,主变量上限值和下限值写入设备*/
/*                       pv_range_unit_code = cmd_data[0];          
                        for(i=0;i<4;i++)
                        {
                          *p_pv_upper++ = pv_upper_754[i] = cmd_data[i+1];
                          *p_pv_lower++ = pv_lower_754[i] = cmd_data[i+5];
                        }
                        hart_resp(cmd_data,9,long_addr,cmd_num);
                        break;
                    
               case  36: pv_upper_value = pv_value;     //将主变量当前值设为主变量上限值
                       for(i=0;i<4;i++)
                         pv_upper_754[i] = *p_pv_upper++; //更新主变量上限值的IEEE754形式
                       hart_resp(no_resp_data,0,long_addr,cmd_num);  
                       break;  
                    
               case  37: /*量程下限的改变将成比例的改变量程上限的值，以使跨度pv_range_span保持不变。
                    当这个改变把量程上限推到了传感器上限时，主变量量程上限饱和且响应代码为14。
                    主变量量程下限把量程上限推到超过传感器上限，将导致得到的跨度要小于主变量
                    最小精度pv_min_span，此时返回响应代码9-应用过程太高*/
 /*                   pv_range_span = pv_upper_value - pv_lower_value;
                    pv_lower_value = pv_value;                   
                    f1 = pv_lower_value + pv_range_span;  //f1为主变量上限的可能值
                    if(f1>pv_sensor_upper)      //超过传感器上限值时
                    {
                      pv_upper_value = pv_sensor_upper;
                      if((pv_sensor_upper-pv_lower_value)<pv_min_span)  //小于主变量最小跨度时
                        status1 = 9;
                      else
                        status1 = 14; 
                    }
                    else
                      pv_upper_value = f1;
                    for(i=0;i<4;i++)
                    {
                      pv_lower_754[i] = *p_pv_lower++;
                      pv_upper_754[i] = *p_pv_upper++;  //更新pv_lower_754和pv_upper_754数组中的内容
                    }
                    hart_resp(no_resp_data,0,long_addr,cmd_num);
                    break;
                    
                case  39: e2prom_code = 2;  //此变量原来初始值为0,烧写操作对其赋值为0,二者医混淆,故改其初始值
                        switch(cmd_data[0])
                         {
                          case 0: e2prom_code = 0;                             
                                break;
                              //待39号命令的响应发送出去后,进行烧写操作
                          case 1: e2prom_code = 1;
                              /*这一行执行恢复操作,将数据从flash传输到RAM,代码由后来编程者添加*/
/*                              break;
                          default: not_resp++;
                              break;
                         }
                         p_e2prom_code = &e2prom_code;
                         hart_resp(p_e2prom_code,1,long_addr,cmd_num);
                         break;
                    
                case  40: for(i=0;i<4;i++)
                          *p_pv_current++ = pv_current_754[i] = cmd_data[i];
                          if(pv_current !=0)
                           {
                             /*在这里添加将设备主变量电流改为接收到数据的代码*/  
/*                         }
                          else
                           {
                             /*这里添加代码使设备退出固定主变量电流模式*/  //主机发送0时,设备退出固定主变量电流模式
/*                         }
                           hart_resp(pv_current_754,4,long_addr,cmd_num); //上面代码添加后,本行函数第一个参数改为实际写入数模转换器的主变量电流数值的IEEE754形式
                           break;
*/                    
//                       case  41: /*在这里添加代码,使变送器自检*/
//                       break;
                    
//              case  42: /*在这里添加代码,使设备复位*/
//                       break;
                    
 //             case  44: pv_range_unit_code = pv_unit_code = cmd_data[0];
//                       p = &pv_unit_code;
//                       hart_resp(p,1,long_addr,cmd_num);
//                       break;
                case 48:  hart_resp(status,25,long_addr,cmd_num);
                         break;
                case 131:  hart_resp(Actuator_inf,5,long_addr,cmd_num);
                         break;
                case 133:  hart_resp(sensor,12,long_addr,cmd_num);
                         break;  
                case 141:  hart_resp(zero_point,1,long_addr,cmd_num);
                         break;
                case 143:  hart_resp(switch_code,1,long_addr,cmd_num);
                         break;
                case 146:  hart_resp(counter_values,29,long_addr,cmd_num);
                         break;         
                case 151:  hart_resp(static_variables,19,long_addr,cmd_num);
                         break;    
                case 153:  hart_resp(alarm_range,8,long_addr,cmd_num);
                         break; 
                case 167:  hart_resp(valve_range,4,long_addr,cmd_num);
                         break;                          
                case 169:  hart_resp(setpoint_cutoff,2,long_addr,cmd_num);
                         break;     
                case 171:  hart_resp(stoke_time,5,long_addr,cmd_num); 
                         break;                         
                case 173:  hart_resp(travel,4,long_addr,cmd_num); 
                         break;
                case 175:  hart_resp(alarm_mark,1,long_addr,cmd_num); 
                         break;                         
                case 177:  hart_resp(readback_range,6,long_addr,cmd_num);  
                         break;
                case 180:  hart_resp(control_data1,20,long_addr,cmd_num);  
                         break;                         
                case 189:  hart_resp(digital_setpoint,6,long_addr,cmd_num);  
                         break;
                case 191:  hart_resp(setpoint_mode,4,long_addr,cmd_num);  
                         break;
                case 193:  hart_resp(digital_input_mode,4,long_addr,cmd_num);  
                         break;                         
                case 195:  hart_resp(actuator,4,long_addr,cmd_num);  
                         break;
                case 204:  hart_resp(measering_task,24,long_addr,cmd_num);  
                         break;                         
                  
//              case 107: for(i=0;i<4;i++)
//                       xmtr_var_code[i] = cmd_data[i];   //将分配到Slot0-3的变送器变量代码写入设备
//                       hart_resp(xmtr_var_code,4,long_addr,cmd_num);
//                       break;
                    
                default: break;
             } 
            // if(not_resp==0)
            // com_send(send,send_bytes);
             if(e2prom_code==0)  //待39号命令的响应发送出去后,进行烧写操作
             {
                /*这一行执行烧写操作,将数据从RAM传输到flash,代码由后来编程者添加*/
              }
             //unsigned char receive[100]={0};
      }        
      CLK.received = 0;
      }
//  } 
}

/*该函数用于从指定的位置开始计算校验和,逐个数据进行异或运算.第一个参数指向要进行校验和计算的
第一个数据,第二个参数是参与计算的数据个数,返回值返回计算的结果.*/
unsigned char Checksum(unsigned char *s , unsigned char nbytes)
{
     unsigned char i,check;
     unsigned char *p;

     for (i=1, check=0, p=s; i<=nbytes; ++i) check ^= *p++;

     return(check);
}

/*通过串口发送数据,第一个参数指向需要发送数据的首地址,第二个参数说明需要发送数据的字节数*/
/*
void com_send(unsigned char *send, unsigned char send_bytes)
{
     unsigned char *send_ptr;
     send_ptr = send;
     P1OUT |=BIT4;  // 开MOS管
     P1OUT &= ~BIT1;
 //    delay(10);

     TXBUF0 =0x55;
     delay(1);
     delay(40);
     P1OUT &=~BIT4;  // 关MOS管
     P1OUT |= BIT1;
     //P2OUT &=~BIT2;  // 开MOS管
}
*/

/*通过串口发送数据,第一个参数指向需要发送数据的首地址,第二个参数说明需要发送数据的字节数*/

void com_send(unsigned char *send, unsigned char send_bytes)
{
     unsigned char *send_ptr;
     send_ptr = send;
     P1OUT |=BIT4;  // 开MOS管
     P1OUT &= ~BIT1;
 //    delay(10);
     for(int i=0;i<send_bytes;i++)
     {
       while ((IFG1 & UTXIFG0) == 0); //等待上一个字节发送完毕
       TXBUF0 = *send_ptr++;
     }
     delay(1);
     delay(40);
     P1OUT &=~BIT4;  // 关MOS管
     P1OUT |= BIT1;
     //P2OUT &=~BIT2;  // 开MOS管
}

/*该函数用于按HART协议的格式生成待发送的数据帧send[].第一个参数为指向响应数据区首字节的
指针,第二个参数为响应数据的字节数,第三个参数是指向本从机长结构地址,第四个参数是命令号*/
void hart_resp( unsigned char *resp_data, unsigned char resp_num, unsigned char *addr ,unsigned char cmd_num)
{
     unsigned char i,j;
     unsigned char *r_d;       
     unsigned char  *a;    
            
     r_d = resp_data ;
     a = addr;
     pre_num =0x07;   //前导符数
  
     for ( i=0; i<pre_num; ++i)  //pre_num是接收到数据帧中前导符的个数,响应时前导府数同主机发送给从机数据帧中前导符个数相同
     send[i] = 0xFF;
     switch(x1)  //判断是长结构还是短结构
     {
       case 0: return;
       case 1: send[i++] = 0x06;   //短结构时
               send[i++] = short_addr;  //使用短地址
               break;
       case 2: send[i++] = 0x86;   //长结构时           
               for(j=0; j<5; ++j) send[i++] = *a++; //使用长地址
               break;
      }
      send[i++] = cmd_num; //填充命令号
      send[i++] = resp_num + 2;//填充字节数
      send[i++] = status1 ; //填充状态位1
      send[i++] = status2 ; //填充状态位2
      for(j=1 ; j<=resp_num; ++j) send[i++] = *r_d++;
      send[i] = Checksum((send + pre_num), i-pre_num); //填充校验字节
      send_bytes = i+1;  //计算数据帧的总长度
      com_send(send,send_bytes);
}
  



//

unsigned int strncmp(const char *cs, const char *ct, unsigned char count)
{
        unsigned char __res = 0;

        while (count) {
                if ((__res = *cs - *ct++) != 0 || !*cs++)                  //比较到结束符/0，时，已经做了__res = *cs - *ct了，所以不等长度时，肯定返回不为0
                        break;
                count--;
        }
        return __res;
}

void HartMessage (void)
{
      rec_bytes = rec_ptr;       //接收的字节数
      rec_ptr = 0;               //清零以准备下一次数据帧的接收
      CLK.received = 1;              //程序运行到这里表示已接收到一个完整的数据帧,将received置位
      CLK.Hart_Flag =0;
}

/*关于用串口接收主机发来的数据帧,串口只能一个字节一个字节地接收数据,利用定时器定时30ms,当相邻的数据字节接收间隔时间不超过30ms时,便认定
二者是同一数据帧当中的数据,当超过30ms仍然没接收到下一个字节时,便认为数据帧接收已经结束.*/
#pragma vector = UART0RX_VECTOR
__interrupt void UART0 (void)
{       
  receive[rec_ptr++] = RXBUF0; // rec_ptr初始值为0,每接受一个数据便自增1
  CLK.Hart_Flag =1;
//  TACTL |= TACLR;              //定时器清零
//  TACTL |= MC0;                  //Timer A开始计时
}
/*
#pragma vector = UART0TX_VECTOR
__interrupt void UART0_TXISR(void)
{
      if(Tindex < SendLen)
      {
          TXBUF0 = *(SDat+Tindex);
          Tindex++;
      }
      else
      {
          Tindex=0;
          //LPM1;
      }
}
*/

/*
// Timer A0 interrupt service routine
#pragma vector = TIMERA0_VECTOR
__interrupt void Timer_A (void)
{ 
  rec_bytes = rec_ptr;       //接收的字节数
  rec_ptr = 0;               //清零以准备下一次数据帧的接收
  received = 1;              //程序运行到这里表示已接收到一个完整的数据帧,将received置位
  TACTL &= ~MC0;                //Timer A停止计时
}
*/