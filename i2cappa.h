#ifndef I2CAPPA_H
#define I2CAPPA_H

#define	_XTAL_FREQ 16000000

#define ADDRESS      0b1101000
#define ADDRESSMON   0b00111000
#define MORE         0
#define LAST         1
#define WRITE        0
#define READ         1
#define I2CCONFIG    0b10011100       //return for pressure value 0b10011100 - PGA x1
#define I2CRESET     0x06
#define I2CERROR     27
#define I2C_DELAY    20
#define BREAKTOLOOP  60000
#define NORM                0x00
#define LEAK                0x01
#define ERROR               0x02
#define SETTING             0x03


char i2cStart ( void );
void CheckSspif ( void );
char i2cStop ( void );
char i2cAddress (  char , char   );
char i2cPutByte ( char );
char i2cGetByte ( char );
char i2cReStart( void );
char breakToLoop ( void );

unsigned int counti2cloop = 0;
#endif