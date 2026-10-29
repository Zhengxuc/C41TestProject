# include "Device.h"

#if WRITE_FACTORY 
const unsigned int reset_datum[64] = { 
                                        0,    // v_p1_1_RCR  0 // 0-1
                                        3,    // v_p1_3BRND  1 // 3-100
                                        
                                        40,   // v_p2_1_MIN  2 // 38-184
                                        200,  // v_p2_2_MAX  3 // 56-210
                                        0,    // v_p2_3_CHR  4 // 0-4
                                        0,    // v_p2_4_ACT  5
                                        10,    // v_p2_5_QOF  6 //0-1000
                                        30,   // v_p2_6_TUP  7 //0-1000//  10
                                        30,   // v_p2_7_TDN  8 //0-1000//  10
                                        
                                        0,     // v_p2_8_1  9 // 38-184
                                        63,    // v_p2_8_2  10 // 56-210
                                        125,   // v_p2_8_3  11 // 0-4
                                        188,   // v_p2_8_4  12 //
                                        250,   // v_p2_8_5  13 //0-1000
                                        313,   // v_p2_8_6  14 //0-1000
                                        375,   // v_p2_8_7  15 //0-1000
                                        438,   // v_p2_8_8  16 //0-1000
                                        500,   // v_p2_8_9  17 //0-1000
                                        563,   // v_p2_8_10  18 //0-1000
                                        625,   // v_p2_8_11 19 //0-1000
                                        688,   // v_p2_8_12 20 //0-1000
                                        750,   // v_p2_8_13 21 //0-1000
                                        813,   // v_p2_8_14 22 //0-1000
                                        875,   // v_p2_8_15 23 //0-1000
                                        938,   // v_p2_8_15 24 //0-1000
                                        1000,  // v_p2_8_15 25 //0-1000
                                        
                                        200,    // v_p3_1_MIN  26 //0-1000   204 5%
                                        3890,   // v_p3_2_MAX  27 //0-1000   3890 95%
                                        0,      // v_p3_3_RCT  28 
                                        
                                        0,     // v_p4_1_MIN  29 //0-1000
                                        1000,  // v_p4_2_MAX  30 //0-1000
                                        0,     // v_p4_3_VRL  31
                                        
                                        963,  // v_p5_1__K0  32 //0-5000  
                                        963, // v_p5_2__K1  33 //0-5000  
                                        350,  // v_p5_3__K2  34 //0-5000
                                        350,  // v_p5_4__K3  35 //0-5000 
                                        5,  // v_p5_5__K4  36 //0-5000 fast
                                        5,  // v_p5_6__K5  37 //0-5000  Kp
                                        5,   // v_p5_7__K6  38 //0-5000  Ki
                                        5,   // v_p5_8__K6  38 //0-5000  form
                                        50,   // v_p5_9__K6  38 //0-5000   speed1
                                        50,   // v_p5_10__K6  38 //0-5000  speed2
                                        
                                        0,    // v_p6_1__LK  39  
                                        0,    // v_p6_2__SV  40    
                                        20,    // v_p6_3__ZO  41 
                                        0,    // v_p6_4__VR  42  
                                        10,    // v_p6_5__TO  43 
                                        2,   // v_p6_6_MIN  44
                                        2,  // v_p6_7_MAX  45
                                        
                                        40,   // v_p7_1_MIN  46 //40-100
                                        200,  // v_p7_2_MAX  47 //100-200
                                        0,    // v_p7_3_ACT  48
                                        300,  // v_p8_1_TRG  49 //10-2000  
                                        0,    // v_p8_2_POS  50 //0-2
                                        728,  // v_p8_4_MIN  51 //10-2000  
                                        3641, // v_p8_5_MAX  52 //0-2
                                        
                                        0,    // AUTO_RDP     //53 v_p8_6_POP  53 //10-2000  
                                        4,    // v_p8_7_RDJ  54 //0-2
                                        
                                        3013, //v_Up_Limited    55   
                                        803,  //v_Down_Limited  56 
                                        1908,  //v_Middle_Valve  57  
                                          
                                        16,   //Kd
                                     };

#endif

void Read_SegA (void)
{
      WDTCTL = WDTPW + WDTHOLD;   //关闭看门狗
      
      int *Flash_ptrA;                         // Segment A pointer
      unsigned int  temp_pointer[64]; 
      unsigned int i;
      Flash_ptrA = (int *) 0x1000;              // Initialize Flash segment A pointer
      for (i=0; i<64; i++)
      {
          temp_pointer[i] = *Flash_ptrA++;      // Copy value segment A to segment B
      }
      FCTL1 = FWKEY;                           // Clear WRT bit
         
      WDTCTL = WDT_MRST_32;    //放狗,并选时钟源为SMCLK,即定时时间为SMCLK/32768
      //------------------------------------------------------

    
      
      
      VAL.V_p1_1_RCR = temp_pointer[v_p1_1_RCR];   
      VAL.V_p1_3BRND = temp_pointer[v_p1_3BRND];  
      VAL.V_p2_1_MIN = temp_pointer[v_p2_1_MIN];   
      VAL.V_p2_2_MAX = temp_pointer[v_p2_2_MAX];   
      VAL.V_p2_3_CHR = temp_pointer[v_p2_3_CHR];   
      VAL.V_p2_4_ACT = temp_pointer[v_p2_4_ACT];
      VAL.V_p2_5_QOF = temp_pointer[v_p2_5_QOF];  
      VAL.V_p2_6_TUP = temp_pointer[v_p2_6_TUP];  
      VAL.V_p2_7_TDN = temp_pointer[v_p2_7_TDN]; 
   
      VAL.V_p2_8_ST1 = temp_pointer[v_p2_8_ST1];
      VAL.V_p2_8_ST2 = temp_pointer[v_p2_8_ST2]; 
      VAL.V_p2_8_ST3 = temp_pointer[v_p2_8_ST3]; 
      VAL.V_p2_8_ST4 = temp_pointer[v_p2_8_ST4]; 
      VAL.V_p2_8_ST5 = temp_pointer[v_p2_8_ST5]; 
      VAL.V_p2_8_ST6 = temp_pointer[v_p2_8_ST6]; 
      VAL.V_p2_8_ST7 = temp_pointer[v_p2_8_ST7]; 
      VAL.V_p2_8_ST8 = temp_pointer[v_p2_8_ST8]; 
      VAL.V_p2_8_ST9 = temp_pointer[v_p2_8_ST9]; 
      VAL.V_p2_8_ST10 = temp_pointer[v_p2_8_ST10];
      VAL.V_p2_8_ST11 = temp_pointer[v_p2_8_ST11]; 
      VAL.V_p2_8_ST12 = temp_pointer[v_p2_8_ST12]; 
      VAL.V_p2_8_ST13 = temp_pointer[v_p2_8_ST13]; 
      VAL.V_p2_8_ST14 = temp_pointer[v_p2_8_ST14]; 
      VAL.V_p2_8_ST15 = temp_pointer[v_p2_8_ST15];  
      VAL.V_p2_8_ST16 = temp_pointer[v_p2_8_ST16];
      VAL.V_p2_8_ST17 = temp_pointer[v_p2_8_ST17];
      
      VAL.V_p3_1_MIN = temp_pointer[v_p3_1_MIN]; 
      VAL.V_p3_2_MAX = temp_pointer[v_p3_2_MAX]; 
      VAL.V_p3_3_RCT = temp_pointer[v_p3_3_RCT];  
   
      VAL.V_p4_1_MIN = temp_pointer[v_p4_1_MIN];  
      VAL.V_p4_2_MAX = temp_pointer[v_p4_2_MAX];  
      VAL.V_p4_3_VRL = temp_pointer[v_p4_3_VRL];  

      VAL.V_p5_1__K1 = temp_pointer[v_p5_1__K1];  
      VAL.V_p5_2__K2 = temp_pointer[v_p5_2__K2];    
      VAL.V_p5_3__K3 = temp_pointer[v_p5_3__K3]; 
      VAL.V_p5_4__K4 = temp_pointer[v_p5_4__K4]; 
      VAL.V_p5_5__K5 = temp_pointer[v_p5_5__K5];  
      VAL.V_p5_6__K6 = temp_pointer[v_p5_6__K6]; 
      VAL.V_p5_7__K7 = temp_pointer[v_p5_7__K7]; 
      VAL.V_p5_8__K8 = temp_pointer[v_p5_8__K8];  
      VAL.V_p5_9__K9 = temp_pointer[v_p5_9__K9]; 
      VAL.V_p5_10_K0 = temp_pointer[v_p5_10_K0]; 
      
      VAL.V_p6_1__LK = temp_pointer[v_p6_1__LK];   
      VAL.V_p6_2__SV = temp_pointer[v_p6_2__SV];     
      VAL.V_p6_3__ZO = temp_pointer[v_p6_3__ZO];  
      VAL.V_p6_4__VR = temp_pointer[v_p6_4__VR];   
      VAL.V_p6_5__TO = temp_pointer[v_p6_5__TO];
      VAL.V_p6_6_MIN = temp_pointer[v_p6_6_MIN];   
      VAL.V_p6_7_MAX = temp_pointer[v_p6_7_MAX]; 
  
      VAL.V_p7_1_MIN = temp_pointer[v_p7_1_MIN];  
      VAL.V_p7_2_MAX = temp_pointer[v_p7_2_MAX];  
      VAL.V_p7_3_ACT = temp_pointer[v_p7_3_ACT];
   
      VAL.V_p8_1_TRG = temp_pointer[v_p8_1_TRG];  
      VAL.V_p8_2_POS = temp_pointer[v_p8_2_POS];  
      VAL.V_p8_4_MIN = temp_pointer[v_p8_4_MIN];  
      VAL.V_p8_5_MAX = temp_pointer[v_p8_5_MAX];  
      
      VAL.V_p8_6_POP = temp_pointer[v_p8_6_POP];       
      VAL.V_p8_7_RDJ = temp_pointer[v_p8_7_RDJ];
    
      OP.Up_Limited = temp_pointer[v_Up_Limited];       
      OP.Down_Limited = temp_pointer[v_Down_Limited]; 
      OP.Middle_Valve = temp_pointer[v_Middle_Valve];   
      
      g_PIDServiceData.Kd = temp_pointer[Pid_Kd];
/*                 
      g_DeviceFeature.FastSector_Lower= VAL.V_p5_5__K5;  //30   50
      g_PIDServiceData.Kp =VAL.V_p5_6__K6; 
      g_PIDServiceData.Ki =VAL.V_p5_7__K7;
      g_PIDServiceData.Transform =  VAL.V_p5_8__K8;   //20  1222 10
      g_DeviceFeature.ValveRodSpeed1 = VAL.V_p5_9__K9; //1222 3   50  4 
      g_DeviceFeature.ValveRodSpeed2 = VAL.V_p5_10_K0; //1222 2   50  3
      
 */     

}


#if  WRITE_FACTORY
void  Factory_Start(void)
{
     _DINT();                    // disable all interrupts　
      WDTCTL = WDTPW + WDTHOLD;   //关闭看门狗
      unsigned int *Flash_ptr;                          // Flash pointer
      unsigned int const *temp_pointer = &reset_datum[0];
      unsigned int temp_pointer2[64];
      unsigned int i;
      
      Flash_ptr = (unsigned int *) SegmentB_address;  // Initialize Flash pointer  
      
      WDTCTL = WDTPW + WDTHOLD;                        // Stop watchdog timer
      FCTL2 = FWKEY + FSSEL0 + FN0;                    // MCLK/2 for Flash Timing Generator
  
      FCTL1 = FWKEY + ERASE;                    // Set Erase bit
      FCTL3 = FWKEY;                            // Clear Lock bit
      *Flash_ptr = 0;                           // Dummy write to erase Flash segment
      FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation
      for (i=0; i<64; i++)
      {
          *(Flash_ptr++) = *(temp_pointer++);   // Write value to flash
          while(FCTL3 & BUSY);
      }
      
      FCTL1 = FWKEY;                            // Clear WRT bit
      while(FCTL3 & BUSY);
      FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
//-------------------------------------------------------------------------------
      Flash_ptr = (unsigned int *) SegmentA_address;  // Initialize Flash pointer
      temp_pointer = (unsigned int *) SegmentB_address;  //
      
      WDTCTL = WDTPW + WDTHOLD;                        // Stop watchdog timer
      FCTL2 = FWKEY + FSSEL0 + FN0;                    // MCLK/2 for Flash Timing Generator
  
      FCTL1 = FWKEY + ERASE;                    // Set Erase bit
      FCTL3 = FWKEY;                            // Clear Lock bit
      *Flash_ptr = 0;                           // Dummy write to erase Flash segment
      FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation
      
      for (i=0; i<64; i++)
      {
         temp_pointer2[i] = *(Flash_ptr++) = *(temp_pointer++);   // Write value to flash
         while(FCTL3 & BUSY);
      }
       //------------------------------------------------------
      FCTL1 = FWKEY;                            // Clear WRT bit
      while(FCTL3 & BUSY);
      FCTL3 = FWKEY + LOCK;
      _EINT();   
      WDTCTL = WDT_MRST_32;    //放狗,并选时钟源为SMCLK,即定时时间为SMCLK/32768
      
      testbbb[0] = temp_pointer2[v_p1_1_RCR];   
      testbbb[1] = temp_pointer2[v_p1_3BRND];  
      testbbb[2] = temp_pointer2[v_p2_1_MIN];
      
      
      VAL.V_p1_1_RCR = temp_pointer2[v_p1_1_RCR];   
      VAL.V_p1_3BRND = temp_pointer2[v_p1_3BRND];  
      VAL.V_p2_1_MIN = temp_pointer2[v_p2_1_MIN];   
      VAL.V_p2_2_MAX = temp_pointer2[v_p2_2_MAX];   
      VAL.V_p2_3_CHR = temp_pointer2[v_p2_3_CHR];   
      VAL.V_p2_4_ACT = temp_pointer2[v_p2_4_ACT];
      VAL.V_p2_5_QOF = temp_pointer2[v_p2_5_QOF];  
      VAL.V_p2_6_TUP = temp_pointer2[v_p2_6_TUP];  
      VAL.V_p2_7_TDN = temp_pointer2[v_p2_7_TDN]; 
   
      VAL.V_p2_8_ST1 = temp_pointer2[v_p2_8_ST1];
      VAL.V_p2_8_ST2 = temp_pointer2[v_p2_8_ST2]; 
      VAL.V_p2_8_ST3 = temp_pointer2[v_p2_8_ST3]; 
      VAL.V_p2_8_ST4 = temp_pointer2[v_p2_8_ST4]; 
      VAL.V_p2_8_ST5 = temp_pointer2[v_p2_8_ST5]; 
      VAL.V_p2_8_ST6 = temp_pointer2[v_p2_8_ST6]; 
      VAL.V_p2_8_ST7 = temp_pointer2[v_p2_8_ST7]; 
      VAL.V_p2_8_ST8 = temp_pointer2[v_p2_8_ST8]; 
      VAL.V_p2_8_ST9 = temp_pointer2[v_p2_8_ST9]; 
      VAL.V_p2_8_ST10 = temp_pointer2[v_p2_8_ST10]; 
      VAL.V_p2_8_ST11 = temp_pointer2[v_p2_8_ST11]; 
      VAL.V_p2_8_ST12 = temp_pointer2[v_p2_8_ST12]; 
      VAL.V_p2_8_ST13 = temp_pointer2[v_p2_8_ST13]; 
      VAL.V_p2_8_ST14 = temp_pointer2[v_p2_8_ST14]; 
      VAL.V_p2_8_ST15 = temp_pointer2[v_p2_8_ST15];  
      VAL.V_p2_8_ST16 = temp_pointer2[v_p2_8_ST16];
      VAL.V_p2_8_ST17 = temp_pointer2[v_p2_8_ST17];
      
      VAL.V_p3_1_MIN = temp_pointer2[v_p3_1_MIN]; 
      VAL.V_p3_2_MAX = temp_pointer2[v_p3_2_MAX]; 
      VAL.V_p3_3_RCT = temp_pointer2[v_p3_3_RCT];  
   
      VAL.V_p4_1_MIN = temp_pointer2[v_p4_1_MIN];  
      VAL.V_p4_2_MAX = temp_pointer2[v_p4_2_MAX];  
      VAL.V_p4_3_VRL = temp_pointer2[v_p4_3_VRL];  

      VAL.V_p5_1__K1 = temp_pointer2[v_p5_1__K1];  
      VAL.V_p5_2__K2 = temp_pointer2[v_p5_2__K2];    
      VAL.V_p5_3__K3 = temp_pointer2[v_p5_3__K3]; 
      VAL.V_p5_4__K4 = temp_pointer2[v_p5_4__K4]; 
      VAL.V_p5_5__K5 = temp_pointer2[v_p5_5__K5];  
      VAL.V_p5_6__K6 = temp_pointer2[v_p5_6__K6]; 
      VAL.V_p5_7__K7 = temp_pointer2[v_p5_7__K7];
      VAL.V_p5_8__K8 = temp_pointer2[v_p5_8__K8];  
      VAL.V_p5_9__K9 = temp_pointer2[v_p5_9__K9]; 
      VAL.V_p5_10_K0 = temp_pointer2[v_p5_10_K0];

      VAL.V_p6_1__LK = temp_pointer2[v_p6_1__LK];   
      VAL.V_p6_2__SV = temp_pointer2[v_p6_2__SV];     
      VAL.V_p6_3__ZO = temp_pointer2[v_p6_3__ZO];  
      VAL.V_p6_4__VR = temp_pointer2[v_p6_4__VR];   
      VAL.V_p6_5__TO = temp_pointer2[v_p6_5__TO]; 
      VAL.V_p6_6_MIN = temp_pointer2[v_p6_6_MIN];   
      VAL.V_p6_7_MAX = temp_pointer2[v_p6_7_MAX]; 
  
      VAL.V_p7_1_MIN = temp_pointer2[v_p7_1_MIN];  
      VAL.V_p7_2_MAX = temp_pointer2[v_p7_2_MAX];  
      VAL.V_p7_3_ACT = temp_pointer2[v_p7_3_ACT];
   
      VAL.V_p8_1_TRG = temp_pointer2[v_p8_1_TRG];  
      VAL.V_p8_2_POS = temp_pointer2[v_p8_2_POS];  
      VAL.V_p8_4_MIN = temp_pointer2[v_p8_4_MIN];  
      VAL.V_p8_5_MAX = temp_pointer2[v_p8_5_MAX];
      
      VAL.V_p8_6_POP = temp_pointer[v_p8_6_POP];       
      VAL.V_p8_7_RDJ = temp_pointer[v_p8_7_RDJ];
      
      OP.Up_Limited = temp_pointer2[v_Up_Limited];       
      OP.Down_Limited = temp_pointer2[v_Down_Limited]; 
      OP.Middle_Valve = temp_pointer2[v_Middle_Valve];  
      
      g_PIDServiceData.Kd = temp_pointer2[Pid_Kd];
/*      
      g_DeviceFeature.FastSector_Lower= VAL.V_p5_5__K5;  //30   50
      g_PIDServiceData.Kp =VAL.V_p5_6__K6; 
      g_PIDServiceData.Ki =VAL.V_p5_7__K7;
      g_PIDServiceData.Transform =  VAL.V_p5_8__K8;   //20  1222 10
      g_DeviceFeature.ValveRodSpeed1 = VAL.V_p5_9__K9; //1222 3   50  4 
      g_DeviceFeature.ValveRodSpeed2 = VAL.V_p5_10_K0; //1222 2   50  3
*/      
       
 
}
#endif


