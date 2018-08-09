
#include <htc.h>
#include "i2cappa.h"


///////////////////////////////////////////
// I2C ���������� ����������
//////////////////////////////////////////

char
breakToLoop ( void ) 
{
 if ( counti2cloop == BREAKTOLOOP )
   return I2CERROR;
  return 1;
}
//////////////////////////////////////////
//�����
//////////////////////////////////////////
char 
i2cStart( void )
{
 counti2cloop = 0;
 SEN = 1;
 while( SEN ) {
   counti2cloop++;
   if ( breakToLoop () == I2CERROR ) {
     counti2cloop = 0;
     return I2CERROR;
   }
 }
 CheckSspif();
 __delay_us ( I2C_DELAY );
 return 1;
}

////////////////////////////////////////////
///�������
////////////////////////////////////////////
/**
char 
i2cReStart( void )
{
 RSEN = 1;
 while( RSEN ); 
 CheckSspif();

 __delay_ms ( I2C_DELAY );

}
*/
//////////////////////////////////////////
//����
//////////////////////////////////////////

char 
i2cStop( void )
{
 counti2cloop = 0;
 PEN = 1;
 while( PEN ) {
   counti2cloop++;
   if ( breakToLoop () == I2CERROR ) {
     counti2cloop = 0;
     return I2CERROR;
   }
 } 
 CheckSspif();
 __delay_us ( I2C_DELAY );
 return 1;
}

///////////////////////////////////////////
//����� ����� SSPOV
///////////////////////////////////////////

void
CheckSspif ( void )
{ 
 SSPOV = SSPIF = 0;
}

//////////////////////////////////////////////
//WRITE-������, READ-������, MORE-������ 
//������������ (�� ��������� ���� � ��������)
//LAST-��������� ����������� ����
//������:
//
//  i2cStart();     
//  status = i2cAddress(WRITE);
//  status = i2cPutByte (123);
//  i2cStop();
// 
//  i2cStart();
//  status = i2cAddress(READ);
//  char i = i2cGetByte (MORE);
//  char s = i2cGetByte (LAST);
//  i2cStop();
//
//////////////////////////////////////////////

//////////////////////////////////////////////
//������� ������ ���������� �� ������ �
//��������� ��������� �� ������ ��� ������ 
//(WRITE, READ)
//////////////////////////////////////////////

char 
i2cAddress ( char w_r, char address )
{
 counti2cloop = 0;
 if ( w_r == WRITE ) { 
   w_r = 0;
 } else {
   w_r = 1;
 }
 
 
 SSPBUF = ( address << 1 ) + w_r;
 
 while( BF ) {
   counti2cloop++;
   if ( breakToLoop () == I2CERROR ){
     counti2cloop = 0;
     return I2CERROR;
   }
 }  
 CheckSspif(); 
  __delay_us ( I2C_DELAY );
 if ( ACKSTAT == 0 ){ 
   return NORM; 
 }
   
 return I2CERROR ;
}

///////////////////////////////////////////////
//������� �������� �����
///////////////////////////////////////////////

char 
i2cPutByte ( char byte )
{ 
 counti2cloop = 0;
 SSPBUF = byte;
 while( BF ) {
   counti2cloop++;
   if ( breakToLoop () == I2CERROR ){
     counti2cloop = 0;
     return I2CERROR;
   }
 }   
 CheckSspif();
 __delay_us ( I2C_DELAY );
 if ( ACKSTAT == 0 ) {   
   return NORM;
	}
 return ERROR; 
}

//////////////////////////////////////////////////
//������� ��������� �����, � ��������� �� �� ���
//���������� ���� �������� ��������� (LAST) ���
//���� �� ��������� (MORE)
//////////////////////////////////////////////////

char 
i2cGetByte ( char ack )
{
 counti2cloop = 0;
 char data;
 RCEN = 1;
 while( RCEN ) {
   counti2cloop++;
   if ( breakToLoop () == I2CERROR ){
     counti2cloop = 0;
     return I2CERROR;
   }
 }   
 data = SSPBUF;
 while( BF ) {
   counti2cloop++;
   if ( breakToLoop () == I2CERROR ) {
     counti2cloop = 0;
     return I2CERROR;
   }
 }   
 //CheckSspif();
 if( !ack ) {
   ACKDT = 0;
   ACKEN = 1;
 } else {
   ACKDT = 1;
   ACKEN = 1;
 } 
 __delay_us ( I2C_DELAY );

 return data; 
}