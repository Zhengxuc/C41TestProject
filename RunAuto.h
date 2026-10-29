#ifndef __RunAuto_H__
#define __RunAuto_H__
//-------------------------------------------------------------------------------------------------
#include "Pwm.h"
extern int g_vl;
extern void Auto_IP( unsigned int tmp_I );
extern void PIDcontrol_H(void);
extern void PIDcontrol(void);
extern void refreshdata(void);
extern long PIDcalculate(void);
extern unsigned int ValveRodSpeed(void);
extern void Boundary_Deal (void);
extern void startpwm (void);
extern void ValveRodSpeedNEW(unsigned char state);

//-------------------------------------------------------------------------------------------------
#endif
