#ifndef __Flash_RW_H__
#define __Flash_RW_H__
//-------------------------------------------------------------------------------------------------
//
# include  <msp430x16x.h>
# define  SegmentA_address 0x1000
# define  SegmentB_address 0x1080
# define  WRITE_FACTORY                      0               // 如果是第一次写入工厂设置时就要 改为1  写入之后下一次就要写为0 0

extern void Factory_Start(void);
extern void Read_SegA (void);
//-------------------------------------------------------------------------------------------------
#endif
