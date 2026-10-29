# include "Device.h"

unsigned char  Send_frame[30] ;    //发送缓冲区
unsigned char  Rece_frame[10] ;    //接收缓冲区

void Uart_Initial( void )
{
        WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
        U0CTL  = 0x00;
        U0TCTL = 0x00;
        P1SEL &= ~BIT4;;    //P6.7普通IO口
        P1DIR |= BIT4;      //P6.7置为输出
        P1OUT &=~BIT4;   //关MOS管
        //P1OUT |=BIT4;   //开MOS管
        P1SEL &= 0xfc;   // p1.0   p1.1置为普通IO口
        P1DIR |= BIT1;  //置输出
        P1DIR &= ~ BIT0;
        P1OUT |= BIT1;
//      FLL_CTL0 |= XCAP18PF;                     // Configure load caps
        P3SEL |= 0x30;                            // P3.4,5 = USART0 TXD/RXD
        P3DIR |= BIT4;    //p3.4为输出脚
        ME1 |= UTXE0 + URXE0;                     // Enable USART0 TXD/RXD
        U0CTL |= CHAR +PENA ;                            // 8-bit character
        //U0TCTL |= SSEL1;                           // UCLK = ACLK
        //UBR00 = 0x63;    //32/9600  0x0c  0x59                
        //UBR10 = 0x01;
        //UMCTL0 = 0x00;      //0x00                      // Modulation   
        U0TCTL &= ~SSEL1;                           
        U0TCTL |= SSEL0; //
        //U0TCTL |= SSEL1;
        UBR00 = 0x1A;    //32/9600  0x0c  0x59                
        UBR10 = 0x00;
        UMCTL0 = 0x6B;      //0x00                      // Modulation   
        
        
        U0CTL &= ~SWRST;                          // Initialize USART state machine
        IE1 |= URXIE0;                             // Enable USART0 RX interrupt
        //IE1 |= UTXIE0;
}  


unsigned char Verify(unsigned char *data,unsigned int longth)                 //校验
{
	static unsigned int i;
	unsigned char temp ;
	temp = *data;
	for(i=1;i<longth;i++)
	{
		temp = temp ^ (*(data+i));
	}
	temp = ~temp;
	return temp;
}

void framing(void)     //组帧 暂且发送数据
{
 //     	static unsigned int i;
        unsigned int data1,data2,data3;
        data1 =AD.CurrentIn;   //指令
        //data2 =AD.CurrentBack; //位置
        data2 =DIS.DisTemp;
       // data3 =PID.PreU;
        
	Send_frame[0] = 0x68;     //帐头
	Send_frame[1] = 0x04;     //数据长度
        Send_frame[2] =  (data1 & 0x00ff);    //指令低8位
        Send_frame[3] =  (data1 & 0xff00)>>8; //指令高8位
        Send_frame[4] =  (data2 & 0x00ff);    //位置低8位
        Send_frame[5] =  (data2 & 0xff00)>>8; //位置高8位
       // Send_frame[6] = Verify(Send_frame,6); //校验
        Send_frame[6] = (data3 & 0x00ff); 
        Send_frame[7] = (data3 & 0xff00)>>8;         
        Send_frame[8] = 0x00; 
        Send_frame[9] = 0x86; 
       
 	Send_frame[0] = 0xFF;     //帐头
	Send_frame[1] = 0xFF;     //数据长度
        Send_frame[2] =  0xFF;
        Send_frame[3] =  0xFF;
        Send_frame[4] =  0xFF;
        Send_frame[5] =  0x02;
       // Send_frame[6] = Verify(Send_frame,6); //校验
        Send_frame[6] = 0x00;
        Send_frame[7] = 0x00;      
        Send_frame[8] = 0x00; 
        Send_frame[9] = 0x82;       

}

void frame_send(unsigned char *data,unsigned char longth)
{
	static unsigned int j = 0;
        framing();
        P1OUT |=BIT4;  // 开MOS管
       // delay(10);
        P1OUT &= ~BIT1;
        delay(1);
	while(j<longth)
	{
//		while(!U1STAbits.TRMT);	
	//	U1TXREG = *(data+j);
                while (!(IFG1 & UTXIFG0));              // USART0 TX buffer ready?
                TXBUF0 = *(data+j) ; 
		j++;
                        
	}
	
	if(j == (longth))
	{
		j = 0;
	}
        delay(50);
        P1OUT &=~BIT4;  // 关MOS管
        P1OUT |= BIT1;
}

/*

#pragma vector=UART0RX_VECTOR
__interrupt void Uart0_Rx (void)
{
     _BIC_SR_IRQ(LPM3_bits);                   // Clear LPM3 bits from 0(SR)
  
}

*/
//void Command_Deal( void )
//{
  
//}





/*

void Uart_Send( unsigned char val )
{
    while (!(IFG1 & UTXIFG0));              // USART0 TX buffer ready?
    //TXBUF0 = RXBUF0;                        // RXBUF0 to TXBUF0
    TXBUF0 = val; 
}


unsigned char Verify(unsigned char *data,unsigned int longth)                 //校验
{
	static unsigned int i;
	unsigned char temp ;
	temp = *data;
	for(i=1;i<longth;i++)
	{
	    temp = temp ^ (*(data+i));
	}
	temp = ~temp;
	return temp;
}


void framing(unsigned char *data,unsigned char type,unsigned char longth)     //组帧
{
	static unsigned int i;
	Send_frame[0]   = 0x68;
	Send_frame[1]	= longth;

	if(type == 1)  //命令
	{
		Send_frame[2] = Send_frame[2] | 0xc0;
		for(i=0;i<longth;i++)
		{
			Send_frame[i+3] = *(data+i);
		}

		Send_frame[longth+3] = Verify(data,longth);
		Send_frame[longth+4] = 0x16;

		frame = Send_frame ;

	}

	else
	if (type == 0) //数据
	{
		Send_frame[2] = Send_frame[2] | 0x80;	
		//frame = data + 1024;
                frame = &(*(data - 3));

		for(i=0; i<3; i++)
		{
			*(frame+i) = Send_frame[i];
		}
			
		*(frame+longth+3) = Verify(data,longth);
		*(frame+longth+4) = 0x16;
		frame = &(*(data - 3));
	}

}


void frame_send(unsigned char *data,unsigned char type,unsigned int longth)   //发送
{
	static unsigned int j = 0;
	static unsigned int k=1500;

	framing(data,type,longth);

	while(j<longth+5)
	{
		while(!U1STAbits.TRMT);	
		U1TXREG = *(frame+j);
		j++;
	}
	while(k--);

	if(j == (longth+5))
	{
		j = 0;
	}
}

*/



