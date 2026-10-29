#ifndef __MessageCenter_H__
#define __MessageCenter_H__
# include "KEYBOARD.H"
# include "HT1621.H"
//**********************************************************************************************
//                                  重要参数设定              （ 机械行程占传感器行程的百分比 ）
//**********************************************************************************************
# define  MCH_SEN_PER  0.95                                      // 这个数据必须要设定为浮点类型
//**********************************************************************************************
//**********************************************************************************************


typedef  enum 
{
  main_menu=0,
  mode1_menu,
  mode2_menu,
  valve_menu,
}Menu_Name;

typedef  enum 
{
  stop_measure=0,
  adc_measure,
  time_measure,
  auto_measure,
}Measure_course;

typedef struct 
{
  unsigned char mode:1;
  unsigned char start:1;    //
  unsigned char stop:1;    //
  unsigned char empty:1;  //清空计数
  unsigned char valve_sort:1;  //阀门种类
  unsigned char mode1_measure:2; //测量过程
  unsigned char flash_refresh:1;//是否写flash
}_Control_Flag; 

extern unsigned char MENU;
extern _Control_Flag Control_Flag;
//------------------------------------------------------------------------------
# define AUTO      0X00   // 自适应控制  工作界面
//------------------------------------------------------------------------------
# define MAN       0X01   // 手动（0－100％） 工作界面
//------------------------------------------------------------------------------
# define QUICK     0X02   // 快速配置
# define CONFIG    0X03   // 组态方式   工作界面


# define TimeStep   100    //调节响应时间
//------------------------------------------------------------------------------
// 键盘宏定义 识别判断码  与硬件关联不可以随意改动
//------------------------------------------------------------------------------
# define INC          0X02   
# define DEC          0X08 
# define MODE         0X04   
# define CONFIRM      0X01
# define INC_CONFIRM  0X03 
# define DEC_CONFIRM  0X09
# define INC_DEC      0x0a
//------------------------
# define INC_MODE     0X06 
# define DEC_MODE     0X0C
//------------------------
# define MODE_CONFIRM 0X05

# define INC_F        0X20   
# define DEC_F        0X80 
# define MODE_F       0X40   
# define CONFIRM_F    0X10
//-------------------------
# define INC_MODE_F   0X60 
# define DEC_MODE_F   0XC0
//-------------------------
# define INC_CONFIRM_F  0X30 
# define DEC_CONFIRM_F  0X90 
# define MODE_CONFIRM_F 0X50
//------------------------------------------------------------------------------
//--------------符号显示控制---------------------
# define  MODE_AUTO   0x8000       // 自适应模式图标
# define  MODE_HAND   0x4000       // 手动模式图标     //左边的    
# define  MODE_       0x2000       //  "-"    图标
//------------------------------------------------
# define  MODE_PERC   0x4000       //  "%"    图标
# define  MODE_BAR    0x2000       //  "bar"  图标
# define  MODE_DEGR   0x0400       //   摄氏度图标
# define  MODE_S      0x0100       //  "S"    图标      //右边的
# define  MODE_LOCK   0x0080       //     自锁图标                                                                                                                                                    
# define  MODE_BOOK   0x0040       //     书本图标
# define  MODE_VOTA   0x0020       //  "V"    图标
# define  MODE_mA     0x0010       // ”mA“  图标
//------------------------------------------------

//-------------------组态编号宏定义
# define   PG1        0
# define   P1_1_RCR   1
# define   P2_1_MIN   1
# define   P3_1_MIN   1
# define   P4_1_MIN   1
# define   P5_1FU     1
# define   P6_1__LK   1
# define   P7_1_MIN   1 
# define   P8_1_TRG   1

//---------------------
# define   PG2        1
# define   P1_2_RDJ   2
# define   P2_2_MAX   2
# define   P3_2_MAX   2
# define   P4_2_MAX   2
# define   P5_2FD     2
# define   P6_2__SV   2
# define   P7_2_MAX   2
# define   P8_2_POS   2
 
//---------------------
# define   PG3        2
# define   P1_3BRND   3
# define   P2_3_CHR   3
# define   P3_3_RCT   3
# define   P4_3_VRL   3
# define   P5_3FP     3
# define   P6_3__ZO   3
# define   P7_3_ACT   1//3
# define   P8_3_FRC   3

//---------------------
# define   PG4        3
# define   P1_4STEP   4
# define   P2_4_ACT   4
# define   P3_4SAVE   4
# define   P4_4SAVE   4
# define   P5_4FB     4
# define   P6_4__VR   4
# define   P7_4TEST   4
# define   P8_4_MIN   4

//---------------------
# define   PG5        4
# define   P1_5TEST   5
# define   P2_5_QOF   5
# define   P5_5BT     5
# define   P6_5__TO   5
# define   P7_5SAVE   2//5
# define   P8_5_MAX   5

//---------------------
# define   PG6        5
# define   P2_6_TUP   6
# define   P5_6SU     6
# define   P1_6SAVE   5//6
# define   P8_6_POP   6
# define   P6_6_MIN   6

//---------------------
# define   PG7        6
# define   P2_7_TDN   7
# define   P5_7SD     7
# define   P8_7_RDJ   7
# define   P6_7_MAX   7

//---------------------
# define   PG8        7
# define   P2_8_CUS   8
# define   P5_8TF     8
# define   P8_8SAVE   6//8
# define   P6_8SAVE   8

//---------------------
#define    PG9        8
# define   P2_9TEST   9
# define   P5_9TB     5//9

//---------------------
# define   P2_10SAV   9//10
# define   P5_10T     6//10

//---------------------
# define   P5_11RST   7//11//
# define   P9_11      11
//---------------------
# define   P5_12SAV   8//12

//---------------------
# define   P1_6_NULL  0
# define   P2_10_NULL 0
# define   P3_4_NULL  0
# define   P4_4_NULL  0
# define   P5_12_NULL 0
# define   P6_8_NULL  0
# define   P7_5_NULL  0
# define   P8_8_NULL  0


//------------------------------------------------------------------------------
//--------------------- 小屏幕显示    是和显示里的顺序一致的
//------------------------------------------------------------------------------
# define   AUTO_RDP  0
# define   AUTO_PID  1
# define   AUTO_MIX  2
# define   MAN_PENS  3
# define   MAN_SENS  4   
# define   CONFIG_D  5

# define   p1_1_RCR  6
# define   p1_2_RDJ  7
# define   p1_3BRND  8     
# define   p1_4STEP  9
# define   p1_5TEST  10 
# define   p1_6SAVE  11
//-----------------------
   
# define   p2_1_MIN  12
# define   p2_2_MAX  13
# define   p2_3_CHR  14    
# define   p2_4_ACT  15
# define   p2_5_QOF  16
# define   p2_6_TUP  17
# define   p2_7_TDN  18
# define   p2_8_CUS  19
# define   p2_9TEST  20
# define   p2_10SAV  21
//-----------------------
# define   p3_1_MIN  22
# define   p3_2_MAX  23   
# define   p3_3_RCT  24
# define   p3_4SAVE  25   
//-----------------------                             
# define   p4_1_MIN  26
# define   p4_2_MAX  27   
# define   p4_3_VRL  28
# define   p4_4SAVE  29  
//----------------------- 
# define   p5_1FU    30
# define   p5_2FD    31
# define   p5_3FP    32      
# define   p5_4FB    33
# define   p5_5BT    34
# define   p5_6SU    35
# define   p5_7SD    36
# define   p5_8TF    37    
# define   p5_9TB    38
# define   p5_10T    39
# define   p5_11RST  40
# define   p5_12SAV  41 
//----------------------
# define   p6_1__LK  42
# define   p6_2__SV  43
# define   p6_3__ZO  44    
# define   p6_4__VR  45
# define   p6_5__TO  46
# define   p6_6_MIN  47
# define   p6_7_MAX  48
# define   p6_8SAVE  49   //对6组态进行保存
                                        
# define   p7_1_MIN  50
# define   p7_2_MAX  51
# define   p7_3_ACT  52    
# define   p7_4TEST  53
# define   p7_5SAVE  54   //对7组态进行保存
                                        
# define   p8_1_TRG  55
# define   p8_2_POS  56
# define   p8_3_FRC  57   
# define   p8_4_MIN  58
# define   p8_5_MAX  59 
# define   p8_6_POP  60  
# define   p8_7_RDJ  61   
# define   p8_8SAVE  62   //对8组态进行保存
//-----------------------------------------                                        
# define   LERK_ERR  63   
# define   INSV_ERR  64
                                        
# define   INSV_HIG  65
# define   INSV_LOW  66
                                        
# define   ZERO_ERR  67
# define   VRLV_ERR  68           
# define   TIME_ERR  69
# define   ELEC_ERR  70
# define   SENS_ERR  71
# define   ACT__ERR  72
                                       
# define   CALC_ERR  73    //测试过程中的问题
# define   BREAK_AU  74
# define   OUT__RNG  75
# define   NO_SCALE  76
# define   RNG__ERR  77
# define   TIME_OUT  78
# define   SPR__ERR  79

# define   RNG__E10  80
# define   ERR_ERR1  81
# define   ERR_ERR2  82
                                     
# define   AUTO__OK  83    //测试过程中同步显示
# define   QUICKRDJ  84 
# define   NULL_SMA  85
# define   AUTO_     86

# define   SET_10_1  87
# define   SET_10_2  88
# define   SET_10_3  89
# define   SET_10_4  90
# define   SET_10_5  91
# define   SET_10_6  92
# define   SET_10_7  93
# define   SET_10_8  94
# define   SET_10_9  95
# define   SET10_10  96
# define   SET10_11  97
# define   SET10_12  98
# define   SET10_13  99
# define   SET10_14  100
# define   SET10_15  101
# define   SET10_16  102
# define   SET10_17  103
# define   QUK_SAVE  104

# define   ALARM_S   105



//------------------------------------------------------------------------------------------
//---------------------快速配置页面的定义
# define   Q_0_COVE  0 
# define   Q_1_P1_1  1 
# define   Q_2_P4_3  2
# define   Q_3_P1_4  3
# define   Q_4_P1_2  4
# define   Q_5_OVER  5
//------------------------------------------------------------------------------------------
//---------------------大屏幕显示定义
# define   P1_6    0
# define   P2_10   1
# define   P3_4    2  
# define   P4_4    3
# define   P5_12   4
# define   P6_6    5
# define   P7_5    6 
# define   P8_8    7
# define   LINE    8           
# define   ROTR    9                                       
# define   DIRC    10         
# define   REVR    11                                            
# define   CLOK    12                  
# define   CTCL    13  
//# define   OFF     14 
//# define   OPEN    15  
//-----------------------
# define   OFF     15 
# define   OPEN    14  
//----------------------以上两行是为了修改显示字符，将open和off显示颠倒
# define   NO      16 
# define   YES     17          
# define   NORC    18           
# define   ACTV    19                                                                          
# define   FCO    20     //替换       
# define   FL    21     //替换          
# define   LT   22      //替换    
# define   M1_25   23       
# define   M1_50   24  
# define   M25_1   25  
# define   M50_1   26
# define   CUST    27
# define   RUN     28   
# define   M___M   29        
# define   M__OK   30 
# define   M__4M   31
# define   M__3M   32
# define   M__2M   33
# define   M__1M   34
# define   M__0M   35
# define   NULL_B  36
# define   _S1_    37
# define   _S2_    38
# define   _S3_    39
# define   _S4_    40
# define   _S5_    41
# define   _S6_    42
# define   _S7_    43
# define   _S8_    44
# define   _S9_    45
# define   _OK_    46
//----------------------------------
# define  MK_1   47  //  防止机械卡死
# define  UP_3   48  // 上边界
# define  DN_4   49  // 下边界
# define  MD_5   50  // 走到中间
# define  SD_6   51  // 减速   
# define  SM_7   52  // 寻找最小 步进
//--------------------------------------等待
# define  WT_6   53 //  6秒等待  
# define  WT_5   54 //  5秒等待     
# define  WT_4   55 //  4秒等待     
# define  WT_3   56 //  3秒等待     
# define  WT_2   57 //  2秒等待   
# define  WT_1   58 //  1秒等待     
# define  WT_0   59 //  0秒等待  
//--------------------------------------走值
# define  t_0   60 // 走0    
# define  t_25  61 // 走25     
# define  t_50  62 // 走50    
# define  t_75  63 // 走75   
# define  t_100 64 // 走100
//--------------------------------------采样
# define  SAMP   65  // sample
//--------------------------------------
# define  REDY   66 //  READY
//--------------------------------------
# define  SAVE   67  //  SAVE
//--------------------------------------
# define  COMP   68 //  COPLETE 
//--------------------------------------
# define  AT_R   69  // AUTO_RDJ 
# define  AT_P   70  // AUTO_PID 
# define  AT_M   71  // AUTO_MIX
# define  MA_P   72  // MAN_PEN
# define  MA_S   73  // MAN_SEN
//--------------------------------------
# define  SCLE   74  // SCALE
# define  MIDD   75  // MIDDLE 
# define  MISP   76  // MIDDLE_SPEED
# define  TINY   77  // TINY_SPEED
# define  ALLL   78  // ALL
# define  M_ER_M 79
# define  CACU   80
# define  DR_2   81
# define  TM_8   82
# define  PASS   83
# define  FAIL   84
//----------------------------------这里以后会添加新的内容 
# define  DIGITAL 85 
# define  ALARM   86
# define  P9_24   87

// 添加自整定标志
# define  ADJ10    88
# define  ADJ20    89
# define  ADJ30    90
# define  ADJ40    91
# define  ADJ50    92
# define  ADJ60    93
# define  ADJ70    94
# define  ADJ80    95
# define  ADJ90    96
# define  ADJ100   97
# define  ADJ110   98
# define  ADJ120   99
# define  ADJ130   100

# define  LS   102
# define  VL   103
# define  VH   104
//

//---------------------组态功能宏定义
# define  P1_2F   1
# define  P1_4F   2
# define  P1_5F   3
# define  P1_6F   4
# define  P2_8F   5
# define  P2_9F   6
# define  P2_10F  7  
# define  P3_1F   8
# define  P3_2F   9
# define  P3_4F   10
# define  P4_4F   11
# define  P5_AF   12
# define  P5_11F  14
# define  P5_12F  15
# define  P6_8F   16
# define  P7_4F   17
# define  P7_5F   18
# define  P8_3F   19
# define  P8_4F   20
# define  P8_5F   21
# define  P8_6F   22
# define  P8_7F   23
# define  P8_8F   24
# define  PQ_KF   25
# define  SAVEF   26
# define  NULLF   0
//---------------------运行步骤宏定义
# define  STP_0   0
# define  STP_1   1
# define  STP_2   2
# define  STP_3   3
# define  STP_4   4
# define  STP_5   5
# define  STP_6   6
# define  STP_7   7
# define  STP_8   8
# define  STP_9   9
# define  SP_10   10
# define  SP_11   11
# define  SP_12   12
# define  SP_13   13
# define  SP_14   14
# define  SP_15   15
# define  SP_16   16
# define  SP_17   17
# define  SP_18   18
# define  SP_19   19
# define  SP_20   20
# define  SP_21   21
# define  SP_OK   22
//------------------用来确定在flash中的存储顺序的
# define   v_p1_1_RCR  0 // 0-1
# define   v_p1_3BRND  1 // 3-100

# define   v_p2_1_MIN  2 // 38-184
# define   v_p2_2_MAX  3 // 56-210
# define   v_p2_3_CHR  4 // 0-4
# define   v_p2_4_ACT  5 // 
# define   v_p2_5_QOF  6 // 0-1000
# define   v_p2_6_TUP  7 // 0-1000
# define   v_p2_7_TDN  8 // 0-1000
   
# define   v_p2_8_ST1  9   //0-1000       10
# define   v_p2_8_ST2  10  //0-1000       11
# define   v_p2_8_ST3  11  //0-1000       12
# define   v_p2_8_ST4  12  //0-1000       13
# define   v_p2_8_ST5  13  //0-1000       14
# define   v_p2_8_ST6  14  //0-1000       15
# define   v_p2_8_ST7  15  //0-1000       16
# define   v_p2_8_ST8  16  //0-1000       17
# define   v_p2_8_ST9  17  //0-1000       18
# define   v_p2_8_ST10 18  //0-1000       19
# define   v_p2_8_ST11 19  //0-1000      15
# define   v_p2_8_ST12 20  //0-1000      16
# define   v_p2_8_ST13 21  //0-1000      17
# define   v_p2_8_ST14 22  //0-1000      18
# define   v_p2_8_ST15 23  //0-1000      19
# define   v_p2_8_ST16 24
# define   v_p2_8_ST17 25

# define   v_p3_1_MIN  26 //0-1000
# define   v_p3_2_MAX  27 //0-1000
# define   v_p3_3_RCT  28 
# define   v_p4_1_MIN  29 //0-1000
# define   v_p4_2_MAX  30 //0-1000
# define   v_p4_3_VRL  31

# define   v_p5_1__K1  32 //0-5000
# define   v_p5_2__K2  33 //0-5000  
# define   v_p5_3__K3  34 //0-5000
# define   v_p5_4__K4  35 //0-5000 
# define   v_p5_5__K5  36 //0-5000
# define   v_p5_6__K6  37 //0-5000 
# define   v_p5_7__K7  38 //0-5000 
# define   v_p5_8__K8  39 //0-5000
# define   v_p5_9__K9  40 //0-5000 
# define   v_p5_10_K0  41 //0-5000 
   
# define   v_p6_1__LK  42  
# define   v_p6_2__SV  43    
# define   v_p6_3__ZO  44 
# define   v_p6_4__VR  45  
# define   v_p6_5__TO  46 
# define   v_p6_6_MIN  47  
# define   v_p6_7_MAX  48
  
# define   v_p7_1_MIN  49 //40-100
# define   v_p7_2_MAX  50 //100-200
# define   v_p7_3_ACT  51 
   
# define   v_p8_1_TRG  52 //10-2000  
# define   v_p8_2_POS  53 //0-2
# define   v_p8_4_MIN  54 //10-2000  
# define   v_p8_5_MAX  55 //0-2

# define   v_p8_6_POP  56   //0-7          48
# define   v_p8_7_RDJ  57   //0-7          49
   
# define   v_Up_Limited    58   
# define   v_Down_Limited  59 
# define   v_Middle_Valve  60  

# define NULL_BIG 61    //这个是哪里来的？

# define  Pid_Kd  61


//--------------------------------------------这个是输入后的值
# define V_DIRC 0
# define V_REVR 1

# define V_OFF  0
# define V_OPEN 1

# define V_LT  2
# define V_LINE  0
# define V_1_25  1
# define V_1_50  2
# define V_25_1  3
# define V_50_1  4
# define V_CUST  5

# define V_ROTR  1

# define V_CLOK  0
# define V_CTCL  1

# define V_NOCR  0
# define V_ACTV  1

# define V_IN   1
# define V_OUT  2

# define  V_AT_R   0  // AUTO_RDJ 
# define  V_AT_P   1  // AUTO_PID 
# define  V_AT_M   2  // AUTO_MIX
# define  V_MA_P   3  // MAN_PEN
# define  V_MA_S   4  // MAN_SEN
//--------------------------------------
# define  V_SCLE   0  // SCALE
# define  V_MIDD   1  // MIDDLE 
# define  V_MISP   2  // MIDDLE_SPEED
# define  V_TINY   3  // TINY_SPEED
# define  V_ALLL   4  // ALL
//----------------------------BREAK _错误的具体解释
# define  E_8_5_UP     0
# define  E_8_5_DN     1
# define  E_8_4_UP     2
# define  E_8_4_DN     3
# define  E_3_1_D_3_2  4
# define  E_4_1_D_4_2  5
# define  E_2_1_D_2_2  6
# define  E_7_1_D_7_2  7
# define  E_M_DN_D_M_UP  8
# define  E_M_DN_D_S_DN  9
# define  E_M_UP_D_S_UP  10
# define  E_CUSTOM_CURVE 11
# define  E_MACHINE_10   12
# define  E_CONTROL_10   13
//------------------------------------------------------------------页面参数控制
struct ALARM_BITS
{
   unsigned int  Leak_ERR  : 1; //0     泄露报警 
   unsigned int  Insv_ERR  : 1; //1     输入超范围
   unsigned int  Insv_HIG  : 1; //2     输入超范围
   unsigned int  Insv_LOW  : 1; //3     输入超范围
   unsigned int  Zero_ERR  : 1; //4     零位超差
   unsigned int  Vrlv_ERR  : 1; //5     阀超范围      
   unsigned int  Time_ERR  : 1; //6     上下动作超时
   unsigned int  Elec_ERR  : 1; //7     电路故障     输入电流 存在严重错误
   unsigned int  Sens_ERR  : 1; //8     传感器 反馈错误
   unsigned int  Act__ERR  : 1; //9     驱动单元故障
   unsigned int  Calc_ERR  : 1; //10    不明确检查    
   unsigned int  Break_AU  : 1; //11    参数设定存在严重错误 ----------
   unsigned int  Out__RNG  : 1; //12    传感器超范围
   unsigned int  No_SCALE  : 1; //13    极限范围无法确定   
   unsigned int  Rng__ERR  : 1; //14    只有10%的传感器被使用     
   unsigned int  Time_OUT  : 1; //15    运动超时
   unsigned int  Spr__ERR  : 1; //16    方向错误
   unsigned int  Keep_ERR  : 1; //17    
   //-------------------------------
   unsigned int  KB__FLAG  : 1; //18
   unsigned int  Delay     : 13; //0
};

typedef union 
{
   unsigned long      all;
   struct ALARM_BITS  bit;
   
}ALARM_REG;

struct SYS_BITS
{
   unsigned int PageStateL  : 2;   // 0-1
   unsigned int PageStateF  : 2;   // 2-3
   unsigned int PageS1      : 4;   // 4-6//之前是3位 140213修改
   unsigned int PageS2      : 5;   // 7-9//之前是4位 140213修改
   unsigned int Step        : 5;   // 10-13
   unsigned int Keep_err    : 1;   // 14-17
   unsigned int Dida        : 1;   // 14-17
};

typedef  union 
{
   unsigned long    all;
   struct SYS_BITS  bit;
   
}SYS_REG;


//struct SYS_BITS  // 从SYS分离两位出来
//{
//    unsigned int Keep_err    : 1;
//    unsigned int Dida        : 1; 
//};

//typedef union
//{
 //   unsigned long     all;
//    struct SYS_BITS  bit;
//}SYS_REG;

//------------------------------------------------------------------------------
//----------------------------------------------------------------组态配置值存储
typedef  struct 
{
   unsigned int  V_p1_1_RCR : 2;   // 0-2         1
   unsigned char V_p1_3BRND : 7;   // 3-100       2
   unsigned char V_p2_1_MIN : 8;   // 38-184      3
   unsigned char V_p2_2_MAX : 8;   // 56-210      4
   unsigned char V_p2_3_CHR : 3;   // 0-4         5
   unsigned char V_p2_4_ACT : 1;   //             6
   unsigned int  V_p2_5_QOF : 10;  //0-1000       7
   unsigned int  V_p2_6_TUP : 10;  //0-1000       8
   unsigned int  V_p2_7_TDN : 10;  //0-1000       9
   
   unsigned int  V_p2_8_ST1 : 10;  //0-1000       10
   unsigned int  V_p2_8_ST2 : 10;  //0-1000       11
   unsigned int  V_p2_8_ST3 : 10;  //0-1000       12
   unsigned int  V_p2_8_ST4 : 10;  //0-1000       13
   unsigned int  V_p2_8_ST5 : 10;  //0-1000       14
   unsigned int  V_p2_8_ST6 : 10;  //0-1000       15
   unsigned int  V_p2_8_ST7 : 10;  //0-1000       16
   unsigned int  V_p2_8_ST8 : 10;  //0-1000       17
   unsigned int  V_p2_8_ST9 : 10;  //0-1000       18
   unsigned int  V_p2_8_ST10 : 10;  //0-1000      19
   unsigned int  V_p2_8_ST11 : 10;  //0-1000      15
   unsigned int  V_p2_8_ST12 : 10;  //0-1000      16
   unsigned int  V_p2_8_ST13 : 10;  //0-1000      17
   unsigned int  V_p2_8_ST14 : 10;  //0-1000      18
   unsigned int  V_p2_8_ST15 : 10;  //0-1000      19
   unsigned int  V_p2_8_ST16 : 10;  //0-1000      19
   unsigned int  V_p2_8_ST17 : 10;  //0-1000      19
    
   unsigned int  V_p3_1_MIN : 13;  //0-1000       20
   unsigned int  V_p3_2_MAX : 13;  //0-1000       21
   unsigned char V_p3_3_RCT : 1;   //             22  
   
   unsigned int  V_p4_1_MIN : 10;  //0-1000       23
   unsigned int  V_p4_2_MAX : 10;  //0-1000       24
   unsigned char V_p4_3_VRL : 1;   //             25
      
   unsigned int  V_p5_1__K1 : 13;  //0-5000       26
   unsigned int  V_p5_2__K2 : 13;  //0-5000       27
   unsigned int  V_p5_3__K3 : 13;  //0-5000       28
   unsigned int  V_p5_4__K4 : 13;  //0-5000       29
   unsigned int  V_p5_5__K5 : 13;  //0-5000       30
   unsigned int  V_p5_6__K6 : 13;  //0-5000       31  
   unsigned int  V_p5_7__K7 : 13;  //0-5000       32
   unsigned int  V_p5_8__K8 : 13;  //0-5000       33
   unsigned int  V_p5_9__K9 : 13;  //0-5000       34
   unsigned int  V_p5_10_K0 : 13;  //0-5000       35

   unsigned int  V_p6_1__LK : 1;   //             36   
   unsigned int  V_p6_2__SV : 12;   //             37   
   unsigned int  V_p6_3__ZO : 7;   //             38
   unsigned int  V_p6_4__VR : 1;   // 1           39
   unsigned int  V_p6_5__TO : 6;   // 1;            40
   unsigned int  V_p6_6_MIN : 6;   // 8;            41
   unsigned int  V_p6_7_MAX : 6;   //            42
  
   unsigned char V_p7_1_MIN : 7;  //40-100        43
   unsigned char V_p7_2_MAX : 8;  //100-200       44
   unsigned char V_p7_3_ACT : 1;  //              45
   
   unsigned int  V_p8_1_TRG : 11;  //10-2000      46
   unsigned int  V_p8_2_POS : 1;   //0-1          47
   
   unsigned int  V_p8_4_MIN : 13;  //10-2000      48
   unsigned int  V_p8_5_MAX : 13;  //10-2000      49
   
   unsigned int  V_p8_6_POP : 3;   //0-7          50
   unsigned int  V_p8_7_RDJ : 3;   //0-7          51
   
   unsigned int  Fit_c[ 18 ];      //             52 
   unsigned int  Cust_c[ 17 ];     //             53 
}VAL_REG;    
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//---------------------控制相关  20150619
typedef struct  
{
  int SetPoint;         // 设定位置
  int SetPointDis;      //显示设定值
  int Cur_ValvePoint;   //当前反馈位置
  int LastValvePoint;   //上次反馈位置
  int Prev_ValvePoint;  //上上次反馈位置
  int Cur_Error;        //当前偏差
  int LastError;        //上次偏差  
  int Prev_Error;       //上上次偏差 
  long SumDeltaU;       //增量输出累加
  long Transform;        //转换值
  int Kp;  //unsigned int Kp;               //比例常数
  int Ki; //unsigned int Ki;               //积分常数
  int Kd; //unsigned int Kd;               //微分常数
  
}PIDServiceData;  

//------------------------------------------------------------------------------
//    灰色预测相关参数   2019 11 19
//------------------------------------------------------------------------------
/*
typedef struct
{
      int  counts;          //运行次数
      int  Rec_Value;       //追值
      int  For_Value;       //预测值
      int  Out_Value;       //输出值
      int  K;
      int  HS_Flag;         //灰色控制状态标志
      int  Oderref;       //指令更新
      int  cur_Value;     //更新时的
      int  form; 
      

}HSYCData;*/

/*  
typedef struct 
{
  unsigned int Fast_Gasfilled;     //快调充气的PWM占空比
  unsigned int Fast_Outgassing;    //快调放气的PWM占空比
  unsigned int Slow_Gasfilled;     //慢调充气的PWM占空比
  unsigned int Slow_Outgassing;    //慢调放气的PWM占空比
  unsigned int Weak_Gasfilled;     //微调充气的PWM占空比
  unsigned int Weak_Outgassing;    //微调放气的PWM占空比 
}PWM_Duty;  
*/
typedef struct 
{
  unsigned int ValveRodSpeed1;    //
  unsigned int ValveRodSpeed2;    //
  unsigned int FastSector_Lower;  //快速区的下线
  unsigned int SlowSector_Lower;  //慢速区的下线
  unsigned int DeadSevtion;       //
  
}DeviceFeature;  
 
extern PIDServiceData g_PIDServiceData;
//extern PWM_Duty g_PWM_Duty;
extern DeviceFeature g_DeviceFeature;

extern KEYBORAD_REG KB;  //键盘消息结构
extern DIS_REG DIS;  //键盘消息结构
//------------------------------------------------------------------------------
extern void MessageCenter( void );
extern void Key_Deal(void);
extern void flash_write_data(unsigned int addr,char *value, unsigned char length);
extern void flash_read_data(unsigned int addr,char *data, unsigned char len);

#endif
