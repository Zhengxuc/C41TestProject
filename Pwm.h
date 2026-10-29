#ifndef __Pwm_H__
#define __Pwm_H__
//-------------------------------------------------------------------------------------------------

typedef struct 
{
    unsigned int FREQUENCY;
    
}PWM_REG;

extern void Control_Initial( void );
extern void OutPut_Positive( unsigned int m );
extern void OutPut_Negetive( unsigned int n );
//-------------------------------------------------------------------------------------------------
#endif
