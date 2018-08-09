#include <htc.h>
#include "ds18b20.h"


char resetWire(){
  int rez = 0;
  DQ_OUT;
  C_DQ;
  __delay_us(800);
  DQ_IN;
  S_DQ;
  RB5 = RB5;
  while (RB5);
  __delay_us(240);
  __delay_us(350);

  return 0;
}


void outByteWire(unsigned char data){
  for (int i = 0; i < 8; i++){
    DQ_OUT;
    C_DQ;
    if (data & 0x01){
      DQ_IN;
    }else{
      DQ_OUT;
      C_DQ;
    }
    __delay_us(50);
    data >> = 0x01;
    DQ_IN;
  }
}

void outBitWire(unsigned char data){
 
  DQ_OUT;
  C_DQ;
  if (data & 0x01){
    DQ_IN;
  }else{
    DQ_OUT;
    C_DQ;
  }
   __delay_us(62);
   DQ_IN;
 
}

unsigned char readByteWire(){
  unsigned char t;
  for(int i = 0; i < 8; i++){
     DQ_OUT;
     C_DQ;
     __delay_us(1);
    t >>= 0x01;
    DQ_IN;
    __delay_us(10);
    RB3 = RB3;
    if(RB3){
      t |= 0x80;
    }
    __delay_us(100);
  }
  return t;
}

unsigned char readBitWire(){
  unsigned char t = 0x00;
  DQ_OUT;
  C_DQ;
  __delay_us(1);    
  DQ_IN;
  __delay_us(10);
  RB3 = RB3;
  if(RB3){
    t = 0x01;
  }
  __delay_us(62);
  
  return t;
}


unsigned char readByteWire_A0(){
  unsigned char t;
  for(int i = 0; i < 8; i++){
     DQ_OUT_A0;
     C_DQ_A0;
     __delay_us(1);
    t >>= 0x01;
    DQ_IN_A0;
    __delay_us(10);
    RB7 = RB7;
    if(RB7){
      t |= 0x80;
    }
    __delay_us(100);
  }
  return t;
}

char resetWire_A0(){
  int rez = 0;
  DQ_OUT_A0;
  C_DQ_A0;
  __delay_us(800);
  DQ_IN_A0;
  S_DQ_A0;
  RB7 = RB7;
  while (RB7); 
  __delay_us(240);
  __delay_us(350);
  return 0;
}

void outByteWire_A0(unsigned char data){
  for (int i = 0; i < 8; i++){
    DQ_OUT_A0;
    C_DQ_A0;
    if (data & 0x01){
      DQ_IN_A0;
    }else{
      DQ_OUT_A0;
      C_DQ_A0;
    }
    __delay_us(50);
    data >> = 0x01;
    DQ_IN_A0;
  }
}

//for 2 devise//

void searchRom() {
  char counter = 0;
  long romDevise[2] = {0x00, 0x00};
  char bitA = 0;
  char bitB = 0;

  resetWire();
  outByteWire(0xF0);
  while (0x01) {
    counter++;
    bitA = readBitWire();
    bitB = readBitWire();
    romDevise[0] |= bitA;
    romDevise[0] << = 0x01;
    if (bitA == bitB)          //01 or 10
      break;    
    outBitWire(bitA);
  }
  for (int i = 0; i < 0x40 - counter; i++){
   bitA |= readBitWire();  
   
   outBitWire(bitA);
  }
}
