
#include <htc.h>
#include "main.h"
#include "i2cappa.h"

void interrupt clock (void){
  //////////////////////////
  if(TMR0IF){
    mSecCounter++;
     if(mSecCounter == FREQUENCE_DIVIDER){
       counterOnTime1 = (TMR1H << 8) | TMR1L;       
     }
    TMR0IF = 0;
    TMR0 = 0xB1F0;

    if(mSecCounter == 100){      
      secCounter++;
      if(secCounter == 60){
        if (minCounter % 3 == 0){
          flagTimer |= 0x80;
        }
        counterWD++;
        if (counterWD > 3){
          state = SENSOR_ERROR;
          counterWD = 3;
        }
        if(state == BEFORE_LEAK){
          counterLeak++;
        }else{
          if(state == LEAK)
            counterLeak = 185;
          else
            counterLeak = 0;
        }

        minCounter++;
        secCounter = 0;        
      }      
      if (secCounter % 2 == 0){
        flagTimer |= 0x40;
      }
      if(secCounter % 10 == 0){
        flagTimer |= 0x20;        
      }      
      mSecCounter = 0;
      TMR1H = TMR1L = 0;
    }

    switch (state){
    case NORM:
      if(mSecCounter < 5){
        LED_NORM = 1;
        LED_LAEK = 0;
      }else{
        LED_NORM = 0;
        LED_LAEK = 0;
      }
      RELLE_NORM = 1;
      RELLE_LEAK = 0;
      break;

    case NOT_STATIC:
      if(mSecCounter < 5){
        LED_NORM = 1;
        LED_LAEK = 1;
      }else{
        LED_NORM = 0;
        LED_LAEK = 0;
      }
      break;

    case SETTING:
      if(mSecCounter < 5 || (mSecCounter > 10 && mSecCounter < 15) || (mSecCounter > 20 && mSecCounter < 25)){
        LED_NORM = 1;
        LED_LAEK = 0;
      }else{
        LED_NORM = 0;
        LED_LAEK = 0;
      }
      RELLE_NORM = 0;
      RELLE_LEAK = 0;
      break;

    case ERROR:
      if(mSecCounter < 5 || (mSecCounter > 10 && mSecCounter < 15) || (mSecCounter > 20 && mSecCounter < 25)){
        LED_NORM = 0;
        LED_LAEK = 1;
      }else{
        LED_LAEK = 0;
        LED_NORM = 0;
      }
      RELLE_NORM = 0;
      RELLE_LEAK = 0;
      break;

      case SENSOR_ERROR:
      if(mSecCounter < 5 || (mSecCounter > 10 && mSecCounter < 15) || (mSecCounter > 20 && mSecCounter < 25)){
        LED_NORM = 0;
        LED_LAEK = 1;
      }else{
        LED_LAEK = 0;
        LED_NORM = 1;
      }
      RELLE_NORM = 0;
      RELLE_LEAK = 0;
      break;

    case LEAK:
      if(mSecCounter < 5){
        LED_NORM = 0;
        LED_LAEK = 1;
      }else{
        LED_NORM = 0;
        LED_LAEK = 0;
      }
      RELLE_NORM = 0;
      RELLE_LEAK = 1;
      break;

      case BEFORE_LEAK:
      if(mSecCounter < 5){
        LED_NORM = 0;
        LED_LAEK = 1;
      }else{
        LED_NORM = 0;
        LED_LAEK = 0;
      }
      RELLE_NORM = 1;
      RELLE_LEAK = 0;
      break;

      default: break;
    }
  }
////////////////////////////////////
  //ERROR KONDEN
  ////////////////////////////////
  if(TMR1IF){
      //state = ERROR;
      TMR1IF = 0;
  }
  
  if(RCIF){
    if((addressDevise == RCREG || addressDevise == 0xFF) && flagUsart != 0x01){
       counterByteFromUsart = 0;
       flagUsart = 0x01;
    }
    if(flagUsart & 0x01){
      dataUsart[counterByteFromUsart] =  RCREG - 0x30;
      counterByteFromUsart++;
      if(counterByteFromUsart == 8){
        flagUsart = 0x02;
      }
    }
  }

}
