#include <htc.h>
#include <stdio.h>
#include "usart.h"
#include "modbus.h"
#include "main.h"


unsigned int getCRC16(unsigned char *data, unsigned char length)
{
  unsigned int CRC = 0xFFFF;
 
  while (length--)
  {
    CRC ^= *data++;
    for (unsigned char i = 8; i > 0; i--){
      if (CRC & 1) {
        CRC >>= 1;
        CRC ^= POLINOM;
      } else {
        CRC >>= 1;
      }
    }
  }
  return CRC;
}


void outPutDataFromUsart(unsigned int address, struct WorkData * outData, unsigned char seek, unsigned char length){
  unsigned int CRC16;
  unsigned char data[22];
  if(length <= 19){
    data[0] = address;
    data[1] = 0x04;
    data[2] = length - seek;
    data[3] = outData->capacitanceValue >> 8;
    data[4] = outData->capacitanceValue;
    data[5] = outData->averageValue >> 8;
    data[6] = outData->averageValue;
    data[7] = outData->tempValue >> 8;  ///////////////
    data[8] = outData->tempValue;
    data[9] = outData->frequencyValue >> 8;
    data[10] = outData->frequencyValue;
    data[11] = outData->outTempValue >> 8;
    data[12] = outData->outTempValue;
    data[13] = outData->correctionValue >> 8;
    data[14] = outData->correctionValue;
    data[15] = outData->flagStaticTrue;
    data[16] = referenceValueOfCapacitance >> 8;
    data[17] = referenceValueOfCapacitance;
    data[18] = referenceValueOfCapacitanceNow >> 8;
    data[19] = referenceValueOfCapacitanceNow;
    data[20] = debug >> 8;
    data[21] = debug;

    length += 3;
    seek *= 2;
    seek += 3;

    for (int i = 3; i < (length - seek) + 3; i++){
      data[i] = data[seek + (i - 3)];
    }
    CRC16 = getCRC16(data, (length - seek) + 3);

    for (int i = 0; i < (length - seek) + 3; i++){
      putch(data[i]);
    }
    putch(CRC16 >> 8);
    putch(CRC16);
  }else{
    errorModBus(address, 0x04);
  }

}

unsigned char * getDataFromUsart(unsigned char addressDevise){
  unsigned char data[12];
  data[0] = getch();

  return data[0];
}

void errorModBus(unsigned int address, unsigned char fun){
  unsigned char data[5];
  unsigned int CRC16;
  data[0] = address;
  data[1] = fun;
  data[2] = 0x01;
  data[3] = 0xFF;
  data[4] = 0xFF;
  CRC16 = getCRC16(data, 5);
  putch(CRC16 >> 8);
  putch(CRC16);
}