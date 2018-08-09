/* 
 * File:   main.c
 * Author: Golikov D.S.
 * Version 2.16
 */

#include <stdio.h>
#include <stdlib.h>
#include <htc.h>
#include "main.h"
#include "usart.h"
#include "i2cappa.h"
#include "ds18b20.h"
#include "modbus.h"
#include "eeprom.h"

//#pragma config CONFIG1H = 0x22
__CONFIG(1, OSC_HS & OSCS_OFF);
//#pragma config CONFIG2L = 0xF
__CONFIG(2, PWRT_OFF & BOR_ON & BORV_20);
//#pragma config CONFIG2H = 0xE
__CONFIG(3, WDT_OFF & WDTPS_128);
//#pragma config CONFIG3H = 0x1
__CONFIG(4, CCP2MUX_ON);
//#pragma config CONFIG4L = 0x85
__CONFIG(5, STVR_ON & LVP_OFF);
//#pragma config CONFIG5L = 0xF
__CONFIG(6, CP0_ON & CP1_ON & CP2_ON & CP3_ON);
//#pragma config CONFIG5H = 0xC0
__CONFIG(7, CPB_OFF & CPD_OFF);
//#pragma config CONFIG6L = 0xF
__CONFIG(8, WRT0_OFF & WRT1_OFF & WRT2_OFF & WRT3_OFF);
//#pragma config CONFIG6H = 0xE0
__CONFIG(9, WRTC_OFF & WRTB_OFF & WRTD_OFF);
//#pragma config CONFIG7L = 0xF
__CONFIG(10, EBTR0_OFF & EBTR1_OFF & EBTR2_OFF & EBTR3_OFF);
//#pragma config CONFIG7H = 0x40
__CONFIG(11, EBTRB_OFF);


void init(){
  ///////////////////////////////////////////
  //PORT 
  ///////////////////////////////////////////
 PORTA = 0b00000000;
 PORTB = 0b00000000;
 PORTC = 0b01000000;

 TRISA = 0b11111111;
 TRISB = 0b11001001;
 TRISC = 0b00011001;

 LATB = 0b00000000;

 ADCON0 = 0b00000000;
 ADCON1 = 0b00000110;
 CCP1CON = 0b00000000;

 ///////////////////////////////
 //I2C
 ///////////////////////////////

 //i2c//
 SSPADD = 44;                            //4 M?? ????????? ? ???????? ?2? 9600
 //SSPIF = 0;
 SSPSTAT = 0b10100000;
 SSPCON1 = 0b00111000;
 SSPCON2 = 0b00000000; 

 /////////////////////////
 //timers
 /////////////////////////
 T0CON = 0b10000000;
 T1CON = 0b00000011;                       //0-8bit 1-16bit//prescale//prescale//Timer1 Oscillator Enable//Timer1 External Clock Input Synchronization Select bit//Timer1 Clock Source Select bit//Timer1 On bit
 /////////////////////////
 //interrupt
 /////////////////////////
 INTCON = 0b11100000;
 INTCON2 = 0b00000000;

 /////////////////////////
 //USART
 /////////////////////////
 RCIE = 1;               //interrupt on

 //////////////////////////////////////////////
 //value
 //////////////////////////////////////////////

 counterOnTime1 = 0;
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 realData.inductanceValue = 14500;  ///////////////////////////////////////////////////////////////////////////////////////////////***********//
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}


void outStartBytes(){
  for(unsigned short i = 0x96; i < 0x99; i++){
    putch(i);
  }
}

unsigned int getTemp(){
  unsigned int temp[2];
  if (resetWire())
    state = ERROR;
  outByteWire(0xCC);
  outByteWire(0x44);
  if (resetWire())
    state = ERROR;
  outByteWire(0xCC);
  outByteWire(0xBE);
  temp[0] = readByteWire();
  temp[1] = readByteWire();
  return (unsigned int)(( temp[1] << 8) | temp[0])/1.6;
}

unsigned int getTemp_A0(){
  unsigned int temp[2];
  if (resetWire_A0())
    state = ERROR;
  outByteWire_A0(0xCC);
  outByteWire_A0(0x44);
  if (resetWire_A0())
    state = ERROR;
  outByteWire_A0(0xCC);
  outByteWire_A0(0xBE);
  temp[0] = readByteWire_A0();
  temp[1] = readByteWire_A0();
  return (unsigned int)(( temp[1] << 8) | temp[0])/1.6;
}
/*
 unsigned int getCorrection(unsigned int temp){
   double rezult;
   if(temp > 0x0800){ //-
     rezult = temp / 0.201f;
   }else{ //+
     rezult = temp / 0.228f;
   }   
   return (int)rezult;
}
*/
unsigned int getCapacitance(struct WorkData * data){
  unsigned int rezult;
  ////////////////////////////////////
  //ERROR KONDEN
  ////////////////////////////////
     if(counterOnTime1 < 250){
         state  = ERROR;
     }
  double capacitance = counterOnTime1 * (100.0 / FREQUENCE_DIVIDER);
  data->frequencyValue = (int)(capacitance + 0.5);
  if (data->tempValue < 0x3E8)
    data->correctionValue = data->tempValue /  0.228f;                           //getCorrection(data->tempValue);
     
  capacitance = 1000000000000000.0/(capacitance * capacitance * 39.4784f * data->inductanceValue/100000) + 0.5f;  
  counnterSetting++;  
  
//  if(data->tempValue > 0x0800){
    rezult = (int)(capacitance - data->correctionValue + 0.5f);
//  }else{
//    rezult = (int)(capacitance - data->correctionValue + 0.5f);
//  }
  
  average += rezult;  
  if(counnterSetting == 0x20){
    average >>= 5;
    data->averageValue = average;
    average = 0;
    counnterSetting = 0;
  }
  return rezult;
}

unsigned long getVoltValueFromADC(){
  unsigned long pressureVoltValue;
  unsigned char data[3];
  i2cStart();
  i2cAddress (WRITE, ADDRESS);
  i2cPutByte(I2CCONFIG);
  i2cStop();
  i2cStart();
  i2cAddress (READ, ADDRESS);
  data[0] = i2cGetByte(MORE);
  data[1] = i2cGetByte(MORE);
  data[2] = i2cGetByte(MORE);
  i2cGetByte(LAST); 
  i2cStop();
  pressureVoltValue = ((data[0] << 8) | data[1]);
  pressureVoltValue = ((pressureVoltValue << 8) | data[2]) + 1;
  return pressureVoltValue;
}

unsigned int getPressureValue(){
  /////////////////////////
  //0...60 mPa//4...20 mA//
  /////////////////////////
  return (unsigned int)((getVoltValueFromADC() - ADC_ZERO_PRESSURE) * 0.585772);
}

long getTempValue(){
  //////////////////////////////
  //-10...+150 0C // 0...20 mA//
  //////////////////////////////
  unsigned long rezult;  
  rezult = ((TEMPERATURE_AT_VOLT * (getVoltValueFromADC() + 1)) + 0.5f);
  if(rezult < OFFSET_TEMPERATURE){ //-
    rezult = OFFSET_TEMPERATURE - rezult;
  }else{                           //+
    rezult -= OFFSET_TEMPERATURE;
  }
  return rezult;
}

void setFlagStaticTrue(char to){

  switch (to){
    case 0x01:
      realData.flagStaticTrue |= 0x02;
      break;
    case 0x02:
      realData.flagStaticTrue |= 0x01;
      break;
    case 0x04:
      realData.flagStaticTrue |= 0x04;
      break;
    default: break;
  }
}

void breakFlagSStaticTrue(char to){

  switch (to){
    case 0x01:
      realData.flagStaticTrue &= 0xFD;
      break;
    case 0x02:
      realData.flagStaticTrue &= 0xFE;
      break;
    case 0x04:
      realData.flagStaticTrue &= 0xFB;
      break;
    default: break;

  }
}

void staticData (int * member, char counter, char to){
char koef;
char counterMember10;
char counterMemberBefore;

if(to == 0x01)
  koef = 40;
else
  koef = 4;
  ///////////////////////////////////////////////////////////////////////////////////////////////////

counterMemberBefore = counterMember - 1;
if(counterMemberBefore > 10)
  counterMemberBefore = 9;

counterMember10 = counterMember + 1;
if(counterMember10 == 10)
  counterMember10 = 0;
//////////////////////////////////////////////////////////////////////////////////////////////////

if(member[counter] < member[counterMember10]){
  if(member[counterMember10] - member[counter] < koef)
    setFlagStaticTrue(to);
  else
    breakFlagSStaticTrue(to);   
}else{
  if (member[counter] - member[counterMember10] > koef)
    breakFlagSStaticTrue(to);
  else
    setFlagStaticTrue(to);
}

if(member[counter] < member[counterMemberBefore]){
  if(member[counterMemberBefore] - member[counter] > koef)
    breakFlagSStaticTrue(to);
}else{
  if (member[counter] - member[counterMemberBefore] > koef)
    breakFlagSStaticTrue(to);
}
}

int main(int argc, char** argv) {  
  init();
  init_comms();  
  state = NOT_STATIC;

  if(!UST && RB6 == 0){
    state = SETTING;
    while(!UST);
    counterOfTrue = 3;
  }
  
  referenceValueOfCapacitance = (readMemori (0x02) << 8) | readMemori (0x03);
  if ((referenceValueOfCapacitance == 0xFFFF) & state != SETTING)
    state = ERROR;

  while (1) {

    if(flagTimer & 0x40){

      ////////////////////////////////////////
      //Get temperature 1 in 10 sec         //
      ////////////////////////////////////////
      if(flagTimer & 0x20){
        realData.outTempValue = getTemp_A0();
        realData.tempValue = getTemp();
        if((realData.outTempValue > 0x3E8 || realData.tempValue > 0x3E8) && realData.flagStaticTrue != 0x04) {
          breakFlagSStaticTrue(0x04);
        }else{
          setFlagStaticTrue(0x04);
        }
      }
      /////////////////////////////////////////
      /////////////////////////////////////////
      /////////////////////////////////////////

      realData.capacitanceValue = getCapacitance(&realData);    //in future appand realData.averageValue
      debug = state;
      // debug = getTempValue();

      /////////////////////////////////////////
      // *                               *   //
      /////////////////****////////////////////
      //*************************************//
      /////////////////////////////////////////


      if(flagTimer & 0x80 && (state != ERROR && state != SENSOR_ERROR)){

        memberPressure[counterMember] = realData.outTempValue;                  
        memberCapacitance[counterMember] = realData.capacitanceValue;
        
               
        ///////////////////////////////////////////////////////////////////
        ////////////////STATIK START///////////////////////////////////////
        //***************************************************************//
        staticData(memberCapacitance, counterMember, 1);
        staticData(memberPressure, counterMember, 2);
        
        ///////////////////////////////////////////////////////////////////
        //realData.flagStaticTrue = 0x07;       //forever static warning //
        ///////////////////////////////////////////////////////////////////
        
        ///////////////////////////////////////////////////////////////////
        ////////////////STATIK END/////////////////////////////////////////
        //***************************************************************//
        
        
        counterMember++;
        if(counterMember == 10)
          counterMember = 0;

        if(state != SETTING){
            
            
        ///////////////////////////////////////////////////////////////////
        //////////////STARTUP_MGP//////////////////////////////////////////
        //***************************************************************//
        if (realData.outTempValue > TEMP_ANGLE_NORM){
          if(realData.capacitanceValue < referenceValueOfCapacitance - 4000)
            state = LEAK;
        } else {
            if(realData.capacitanceValue < (referenceValueOfCapacitance - (TEMP_ANGLE_LEAK - realData.outTempValue) * ANGLE_TO_25))
              state = LEAK;         
        }
        ///////////////////////////////////////////////////////////////////
        ////////////END STARTUP////////////////////////////////////////////
        //***************************************************************//
            
            
          if(realData.flagStaticTrue == 0x07 && state != LEAK){
            counterOfTrue--;
            if(counterOfTrue == 0){
              counterOfTrue = 1;
                          
              if (realData.outTempValue < TEMP_ANGLE_LEAK){

                referenceValueOfCapacitanceNow = referenceValueOfCapacitance - (TEMP_ANGLE_LEAK - realData.outTempValue) * ANGLE_TO_25;
                //////////////////////////////////////////////////////////////////////////
                if(realData.capacitanceValue < referenceValueOfCapacitanceNow){
                  if (counterLeak > 180 || state == LEAK){
                    state = LEAK;
                  }else{
                    state = BEFORE_LEAK;
                  }
                }else{
                  state = NORM;
                }
              ///////////////////////////////////////////////////////////////////////////
              }else{
                if(realData.outTempValue < (TEMP_ANGLE_LEAK_CRIT)){
                  referenceValueOfCapacitanceNow = referenceValueOfCapacitance + (realData.outTempValue - TEMP_ANGLE_LEAK) * ANGLE_AFTER_25;
                  ///////////////////////////////////////
                  if(realData.capacitanceValue < referenceValueOfCapacitanceNow){
                    if (counterLeak > 180 || state == LEAK){
                      state = LEAK;
                    }else{
                      state = BEFORE_LEAK;
                    }
                  }else{
                    state = NORM;
                  }
                  //////////////////////////////////////
                }else{
                ///critc///
                  referenceValueOfCapacitanceNow = referenceValueOfCapacitance + ANGLE_CRIT_CONTROL;
                /////////////////////////////////////////////////////
                  if(realData.capacitanceValue < referenceValueOfCapacitanceNow){
                    if (counterLeak > 180 || state == LEAK){
                      state = LEAK;
                    }else{
                      state = BEFORE_LEAK;
                    }
                  }else{
                    state = NORM;
                  }
                  /////////////////////////////////////////////////////
                }
              }
            }
          }else{
            if (state != LEAK)
              state = NOT_STATIC;
            counterOfTrue = 3;
          }
        }else{

          if(realData.flagStaticTrue == 0x07){
            counterOfTrue--;
            if(counterOfTrue == 0){
             counterOfTrue = 3;
             referenceValueOfCapacitance = realData.capacitanceValue;

              if(realData.outTempValue < TEMP_ANGLE_NORM){
                referenceValueOfCapacitance += (TEMP_ANGLE_NORM - realData.outTempValue) * ANGLE_TO_25;
                referenceValueOfCapacitance -= LEAK_VALUE;
              }else{
                 if(realData.outTempValue < TEMP_ANGLE_LEAK){
                   referenceValueOfCapacitance -= (realData.outTempValue - TEMP_ANGLE_NORM) * ANGLE_AFTER_25;
                   referenceValueOfCapacitance -= LEAK_VALUE;
                 }else{
                   referenceValueOfCapacitance -= ANGLE_CRIT;
                 }
              }
              saveMemori (0x02, referenceValueOfCapacitance >> 8);
              saveMemori (0x03, referenceValueOfCapacitance);
              state = NORM;
            }
          }else{
            counterOfTrue = 3;
          }
        }
      }
      counterWD = 0;
      flagTimer &= 0x1F;
      RS485_OUT;
      outPutDataFromUsart(addressDevise, &realData, 0, 19);
      RS485_IN;
    }
  }
  return (EXIT_SUCCESS);
}
