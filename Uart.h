#ifndef __Uart_H__
#define __Uart_H__
//-------------------------------------------------------------------------------------------------
# include <msp430x16x.h>
typedef struct 
{
    unsigned int DSF; 
}UART_REG;

//extern void Uart_Send( unsigned char val );
extern void Uart_Initial( void );
extern void frame_send(unsigned char *data,unsigned char longth);
extern unsigned char Verify(unsigned char *data,unsigned int longth) ;
extern void framing(void);
extern unsigned char Send_frame[30];


//-------------------------------------------------------------------------------------------------
#endif

