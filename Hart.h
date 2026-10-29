//*****************************************************************************
//                 Hart interface program 
//------------------------------------------------------------------------------
//  Project Name     :  HT2015   and  AD 421
//  Project Number   :  20071005
//  Author           :  stefanie
//  Date             :  2008-02-24
//  Version          :  current revision of the HART protocol specification 5.3
//-----------------------------------------------------------------------------
//                    All Copyrights are reserved
//******************************************************************************

//------------------------------------------------------------------------------
// 接口定义：   AD421      LATCH    CLOCK    DATA    GND
//                         P1.5     P1.6     P1.7    GND
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// 接口定义：  Ht2015      ORXD     OCD    ITXD   INRTS   +3V   GND
//                          RXD    P5.5    TXD    P1.1     
//------------------------------------------------------------------------------

//---------------------------------------------
//   层号    层名      OSI层次       HART层次 
//    7    应用层    格式化数据      HART命令 
//    6    表示层    转换数据 无 
//    5    会话层    控制会话 无 
//    4    传输层    确保信息完整 无 
//    3    网络层    路由传送 无 
//    2  数据链路层  差错处理 协议规则 
//    1    物理层    连接设备 BELL 202 
//----------------------------------------------
// HART协议包括物理层，数据链路层和应用层，及DLL语言这四部分。
//-----------------------------------------------------------

//------------------------------------------------------------------------------
//  HART协议数据链路层规范    从设备      burst模式设备     主设备
//  主-从型帧，从-主型帧   长型帧，短型帧  
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  the structure of a HART message 
//------------------------------------------------------------------------------
//   PRFAMBLE   START   ADDR  COMM   BCNT   (STATUS)    DATA    CHK
//    5-20        1     1-5     1      1       2       0 -25     1
//------------------------------------------------------------------------------
/*
    The preamble, of between 5 and 20 bytes of hex FF (all 1"s), helps the receiver
        to synchronise to the character stream.
　　
　　The start character may have one of several values, indicating the type of message:
        master to slave, slave to master, or burst message from slave; also the address format: 
        short frame or long frame.
　　
　　The address field includes both the master address (a single bit: 1 for a primary master, 
        0 for a secondary master) and the slave address. In the short frame format,
        the slave address is 4 bits containing the "polling address" (0 to 15). 
        In the long frame format, it is 38 bits containing a "unique identifier" for that particular device.
        (One bit is also used to indicate if a slave is in burst mode.)
　　
　　The command byte contains the HART command for this message. Universal commands 
        are in the range 0 to 30; common practice commands are in the range 32 to 126;
        device-specific commands are in the range 128 to 253.
　　
　　The byte count byte contains the number of bytes to follow in the status and data bytes.
        The receiver uses this to know when the message is complete. 
        (There is no special "end of message" character.)
　　
　　The status field (also known as the "response code") is two bytes, only present in the
        response message from a slave. It contains information about communication errors in
        the outgoing message, the status of the received command, and the status of the device itself.
　　
　　The data field may or may not be present, depending on the particular command.
        A maximum length of 25 bytes is recommended, to keep the overall message duration reasonable.
        (But some devices have device-specific commands using longer data fields.)
        See also the HART data field.
　　
　　Finally, the checksum byte contains an "exclusive-or" or "longitudinal parity" of all previous 
        bytes (from the start character onwards). Together with the parity bit attached to each byte, 
        this is used to detect communication errors.
//----------------------------------------------------------------------------------------------------
//   standard uart 
*/
//------------------------------------------------------------------------------
// 针对 芯片 HT2015 引脚定义
//

//#ifndef __Hart_H__
//#define __Hart_H__

# include <msp430x16x.h>

//extern unsigned char rec_ptr;

# define  HART_MODE   1     // 1   PIN MODE   O  UART MODE

# define  OCD_N   BIT0
# define  OCDDIR  P1DIR
# define  OCDSEL  P1SEL
# define  OCDIES  P1IES  
# define  OCDIFG  P1IFG
# define  OCDIN   P1IN
# define  OCDIE   P1IE
# define  INRTS   BIT1
//# define  OCD     
//----------------------------------------------------先导字符
# define PREAMBLES           0xFF
//----------------------------------------------------定界符
# define LONG_BURST_FRAME    0X81     //  1000  0  001
# define LONG_STX_FRAME      0X82     //  1000  0  001
# define LONG_ACK_FRAME      0X86     //  1000  0  001
# define SHORT_BURST_FRAME   0X01     //  1000  0  001
# define SHORT_STX_FRAME     0X02     //  1000  0  001
# define SHORT_ACK_FRAME     0X06     //  1000  0  001
//----------------------------------------------------编址
# define ADD_MASTER_UNBURST  0X80    
# define ADD_SLAVE_UNBURST   0X00 
# define ADD_MASTER_BURST    0XC0        
# define ADD_SLAVE_BURST     0X40 
//******************************************************************************
//----------------------------------------------------命令域     ( 通用命令域)
# define READ_ID       0   // 读标识码  
# define READ_PV       1   // 读主变量（PV）
# define READ_P_A      2   // 读主变量电流值和百分比
# define READ_MP_A     3   // 读动态变量和主变量电流
# define WRITE_POLL    6   // 写POLLING地址
# define READ_IN_TAG   11  // 用设备的Tag读设备的标识
# define READ_MESSAGE  12  // 读消息（Message）
# define READ_TAG_T    13  // 读标签Tag，描述符Description和日期Date
# define READ_IN_S     14  // 读主变量传感器信息
# define READ_IN_O     15  // 读主变量输出信息
# define READ_F_ID     16  // 读最终装配号
# define WRITE_MESSAGE 17  // 写消息
# define WRITE_TAG     18  // 写标签、描述符和日期
# define WRITE_F_ID    19  // 写最后装配号
//----------------------------------------------------命令域     ( 一般命令域)
# define READ_B_PV     33  // 读变送器变量
# define WRITE_M_R     34  // 写主变量阻尼值
# define WRITE_M_M     35  // 写主变量量程值 
# define SET_M_TOP     36  // 设置主变量量程上限
# define SET_M_LOW     37  // 设置主变量量程下限
# define RESET_FLAG    38  // 复位组态改变标志
# define SET_EE        39  // EE控制

# define MOD_M_A       40  // 进入/退出固定主变量电流模式

# define CHECK_SELF    41  // 执行变送器自检

# define RESET_IN      42  // 执行设备复位  
# define SET_M_ZERO    43  // 设置主变量零点
# define WRITE_M_D     44  // 写主变量单位
# define MODIFY_M_DAC  45  // 调整主变量电流DAC零点
# define MODIFY_M_INC  46  // 调整主变量电流DAC增益
# define WRITE_M_FUN   47  // 写主变量传递功能
# define READ_A_IN     48  // 读附加的变送器状态
# define WRITE_M_C_ID  49  // 写主变量传感器序列号
# define READ_D_CONFIG 50  // 读动态变量配置
# define WRITE_D_CONFIG 51 // 写动态变量配置
# define SET_IN_ZERO    52 // 设置变送器变量零点
# define WRITE_IN_D     53 // 写变送器变量单位
# define READ_IN_INFOR  54 // 读变送器变量信息
# define WRITE_IN_Z     55 // 写变送器变量阻尼值
# define WRITE_IN_ID    56 // 写变送器变量传感器序列号
# define READ_UNIT_TAG  57 // 读UNIT设备标签、描述、日期
# define WRITE_IN_TAG   58 // 写UNIT设备标签、描述、日期
# define WRITE_Q_NUM    59 // 写响应前导符的个数
# define READ_A0_P      60 // 读模拟输出AO和量程的百分比
# define READ_D_PV      61 // 读动态变量和PV模拟输出
# define READ_AS        62 // 读多个模拟输出
# define READ_A_INFOR   63 // 读模拟输出信息
# define WRITE_AS_Z     64 // 写模拟输出附加阻尼值
# define WRITE_AS_M     65 // 写模拟输出量程值
# define SET_IO_AS      66 // 进入/退出固定模拟输出模式
# define MODIFY_ZERO_AS 67 // 修正模拟输出零点

# define MODIFY_ZY_AS   68 // 修正模拟输出增益
# define WRITE_TRAN     69 // 写模拟输出Transfer功能
# define READ_AD_T      70 // 读模拟输出端点值
# define WRITE_BURST    107 // 写Burst变送器变量
# define WRITE_BURST_ID 108 // 写Burst模式命令号
# define MOD_BURST      109 // Burst模式控制
# define READ_ALL       110 // 读所有动态变量
# define CON_TRAN       111 // Transfer服务控制 

# define  resp_num_0   12
# define  resp_num_1    5
//---------------------------------------------------特殊命令
//   KONG
//------------------------------------------------------------------------------

typedef struct HartFrame
{
    unsigned char preambles[7];  // 启动引导   
    unsigned char delimiter;  // 定界符
    union
    {
        unsigned char ShortAddr;
        unsigned char LongAddr[5];
    }Address;                 //  地址
    unsigned char command;    //  命令域
    unsigned char counter;    // 数据字节记数域
    unsigned char data[24];   // 数据域
    unsigned char check;      // 径向校验  垂直校验
    unsigned char respond[2]; // 响应码字段  主长帧中
}HARTframe;


typedef union                                        
{
   float ul_Temp;
   unsigned char  uc_Buf[4];
}un_DtformConver;




//HARTframe receive;
//receive.preambles = PREAMBLES;
/*
#if  HART_MODE 

void HT2015_intitial(void)
{
    OCDSEL &=~(OCD_N);   // function of pin（IO）
    OCDDIR &=~(OCD_N);   // derection
    OCDIES &=~(OCD_N);   // raising edge intterupt
    OCDIFG &=~(OCD_N);   // clear the flag of intterupt                                                                  
    OCDIE  |= (OCD_N);   // enable pin intterupt  
}

#endif










void  datum_send(void)
{
   //   Transfer.request(address,sequence no,[data])：//本原语由主设备HART协议用户使用向给定地址上的从设备实体请求传输信息。
   //   Transfer.indicate(address,sequence no,[data])：// 本原语由数据链路层激发，通知从设备HART协议用户从对等协议实体中收到了一条合法的传送消息。 
   //   Transfer.response(status,sequence,[data])：// 本原语由从设备用户执行，响应进入的transfer.indicate。
   //   Transfer.confirm(local status,response code,sequence no,[data])：// 本原语向主设备协议用户返回先前的transfer.request的执行结果。

}
*/