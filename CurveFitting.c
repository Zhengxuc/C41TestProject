# include "FittingData.H"
# include "Device.h" 

//------------------------------------------------------------------------------

/*
unsigned int Formula( unsigned int current )
{
    unsigned int tmp;
    unsigned char i = 0;
    for( i = 0; i<17 ; i++ )
    {
      if( current < VAL.Fit_c[i] ) break;
    } 
    if( i==0 ) 
    {
      tmp = OP.Down_4_1;
    }
    else if( i==17 )
    {
      tmp = OP.Up_4_2;
    }
    else
    {  
      if( VAL.V_p2_3_CHR < V_CUST )
      {  
         tmp = ((float)( Cv_Fit[ VAL.V_p2_3_CHR ][ i ]  - Cv_Fit[ VAL.V_p2_3_CHR ][i - 1])/(float)( VAL.Fit_c[17] )*( current - VAL.Fit_c[i - 1] ) + Cv_Fit[ VAL.V_p2_3_CHR ][i - 1])/1000.0*OP.Range_D + OP.Down_4_1;//OP.Down_Limited;
      }
      else
      {  
         tmp = ((float)( VAL.Cust_c[ i ]  - VAL.Cust_c[i - 1])/(float)( VAL.Fit_c[17] )*( current - VAL.Fit_c[i - 1] ) + VAL.Cust_c[i - 1])/1000.0*OP.Range_D + OP.Down_4_1;//OP.Down_Limited;
      }
    }  
    return tmp;
}
*/

unsigned int Formula( unsigned int current )
{
    unsigned int tmp;
    unsigned char i = 0;
    for( i = 0; i<17 ; i++ )
    {
      if( current < VAL.Fit_c[i] ) break;
    } 
    if( i==0 ) 
    {
      tmp = OP.Down_4_1;
    }
    else if( i==17 )
    {
      tmp = OP.Up_4_2;
    }
    else
    {  
      if( VAL.V_p2_3_CHR < V_CUST )
      {  
         tmp = ((float)( Cv_Fit[ VAL.V_p2_3_CHR ][ i ]  - Cv_Fit[ VAL.V_p2_3_CHR ][i - 1])/(float)( VAL.Fit_c[17] )*( current - VAL.Fit_c[i - 1] ) + Cv_Fit[ VAL.V_p2_3_CHR ][i - 1]); ///1000.0*OP.Range_D + OP.Down_4_1;//OP.Down_Limited;
      }
      else
      {  
         tmp = ((float)( VAL.Cust_c[ i ]  - VAL.Cust_c[i - 1])/(float)( VAL.Fit_c[17] )*( current - VAL.Fit_c[i - 1] ) + VAL.Cust_c[i - 1]); //  /1000.0*OP.Range_D + OP.Down_4_1;//OP.Down_Limited;
      }
    }  
    return tmp;
}
