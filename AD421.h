#ifndef __AD421_H__
#define __AD421_H__
//-------------------------
# include <msp430x16x.h>
# include "delay.h"
//-------------------------
//  Òý½Å¶¨Òå
//-------------------------
# define AD421_DIR    P1DIR 
# define AD421_SEL    P1SEL  
# define AD421_PORT   P1OUT


# define LATCH_AD421  BIT7
//# define CLOCK_AD421  BIT4
# define CLOCK_AD421  BIT6
# define DATA_AD421   BIT5
//-------------------------
extern void AD421_Initial(void);
extern void SEND_AD421(void);
extern void SEND_AD421_Test( unsigned int data_temp );
#endif
