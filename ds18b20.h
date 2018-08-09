/* 
 * File:   ds18b20.h
 * Author: User
 *
 * Created on 10 ???????? 2015 ?., 12:42
 */

#ifndef DS18B20_H
#define	DS18B20_H

#define	_XTAL_FREQ 16000000
#define DQ_IN      TRISB3=1
#define DQ_OUT     TRISB3=0
#define S_DQ       RB3=1
#define C_DQ       RB3=0

#define DQ_IN_A0      TRISB7=1
#define DQ_OUT_A0     TRISB7=0
#define S_DQ_A0       RB7=1
#define C_DQ_A0       RB7=0

char resetWire();
void outByteWire(unsigned char data);
unsigned char readByteWire();
void searchRom();

char resetWire_A0();
void outByteWire_A0(unsigned char data);
unsigned char readByteWire_A0();



#endif	/* DS18B20_H */

