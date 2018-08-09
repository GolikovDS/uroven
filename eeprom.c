
#include <htc.h>


unsigned char
readMemori ( char add )
{
 unsigned short memdata;
 GIE = 0;
 EEADR = add;
 EEPGD = CFGS = 0x00;
 RD = 0x01;
 memdata = EEDATA;
 GIE = 1;
 return memdata;
}

void
saveMemori (char add, char data)
{
 EEADR = add;
 EEDATA = data;
 EEPGD = CFGS = 0x00;
 WREN = 1;
 GIE = 0;
 EECON2 = 0x55;
 EECON2 = 0xAA;
 WR = 1; 
 while ( WR );
 WREN = 0;
 GIE = 1;
}