# include <msp430x16x.h>

void delay(unsigned int y)
{
    unsigned char j;
    while(y--) 
    {  
       for(j=250;j>0;j--);
    }   
}

void delay_ns(unsigned int y)
{
    while(y--) 
    {  
       _NOP();
    }   
}



