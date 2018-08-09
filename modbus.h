/* 
 * File:   modbus.h
 * Author: User
 *
 * Created on 21 ???????? 2015 ?., 14:12
 */

#ifndef MODBUS_H
#define	MODBUS_H


#define POLINOM 0xA001


unsigned int getCRC16(unsigned char *data, unsigned char length);
void outPutDataFromUsart(unsigned int address, struct WorkData * outData, unsigned char seek, unsigned char length);
unsigned char * getDataFromUsart(unsigned char addressDevise);
void errorModBus(unsigned int address, unsigned char fun);
#endif	/* MODBUS_H */

