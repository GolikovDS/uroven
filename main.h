/* 
 * File:   main.h
 * Author: User
 *
 * Created on 26 ?????? 2015 ?., 15:51
 * 18/09/2015 - Add signed for termopar
 */

#ifndef MAIN_H
#define	MAIN_H


#define	_XTAL_FREQ 16000000

#define NORM                  0x00
#define BEFORE_LEAK           0x06
#define LEAK                  0x01
#define ERROR                 0x02
#define SETTING               0x03
#define SETTING_ZERO          0x04
#define NOT_STATIC            0x05
#define SENSOR_ERROR          0x07

#define UST                   RB0
#define LED_NORM              RB4
#define LED_LAEK              RB5
#define RELLE_NORM            RB2
#define RELLE_LEAK            RB1
#define GO_TO_SET_ZERO        RB6


#define ADC_ZERO_PRESSURE     25600
#define FREQUENCE_DIVIDER     80f
#define TEMPERATURE_AT_VOLT   0.125f //  range temperature -10...150 = 160 divided on max code 131071
#define OFFSET_TEMPERATURE    1000
#define LENGTH_DATA_OUTPUT    12

#define ANGLE_TO_25           40
#define ANGLE_AFTER_25        100
#define ANGLE_CRIT            3700 //delta 600, true value of crit 2100
#define ANGLE_CRIT_CONTROL    2800 //
#define LEAK_VALUE            1200
#define TEMP_ANGLE_LEAK       240
#define TEMP_ANGLE_LEAK_CRIT  265
#define TEMP_ANGLE_NORM       215
#define LOWER_TEMPERATURE_THRESHOLD 50

#define RS485_OUT             RC5=1
#define RS485_IN              RC5=0

struct WorkData{
  unsigned int capacitanceValue;
  unsigned int averageValue;
  unsigned int frequencyValue;
  unsigned int outTempValue;
  unsigned int correctionValue;  
  unsigned int tempValue;
  unsigned int inductanceValue;
  unsigned char flagStaticTrue;//  // // // // // // // //timerOk//CapacitanceOk//
};
unsigned int counterLeak;
unsigned char counterWD;
unsigned int debug;

/////////////////////////////////////////////
//timer dinamic
/////////////////////////////////////////////
unsigned int memberCapacitance[10];
unsigned char counterOfTrue;
unsigned int memberPressure[10];
char counterMember;

//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
//
//Capacitance -> 0x02 & 0x03
//inductance -> 0x04
//in EEPROM

unsigned int referenceValueOfCapacitance;
unsigned int referenceValueOfCapacitanceNow;
//////////////////////////////////////////////

struct WorkData realData;
unsigned char state;
//couters//
unsigned int mSecCounter; // 10mSec
unsigned char secCounter;
unsigned long minCounter;

unsigned int counterOnTime1;
unsigned long average;
unsigned char counnterSetting;

//delay //
                         ///////////////////////////////
unsigned char flagTimer; //1sec// 5sec //  // // // // //
                         ///////////////////////////////

//usart//
unsigned char dataUsart[8];
unsigned char flagUsart;                           //|||||||||All byte from usart is read|flag addressDivise OK|
unsigned char counterByteFromUsart;



unsigned int addressDevise;
void init();
unsigned int getTemp(); //from ds18d20
void intToCharArrayOutData(unsigned long value, unsigned int length, unsigned int indexOf);
void getTemperature();
void outStartBytes();
void outDataFun ();
unsigned int getCorrection(unsigned int temp);
unsigned int getCapacitance(struct WorkData * data);
unsigned long getVoltValueFromADC();
unsigned int getPressureValue();

long getTempValue();
void staticData (int * member, char counter, char to);

#endif	/* MAIN_H */

