#include <htc.h>
#include <stdio.h>
#include "usart.h"


////////////////////////////////////////////////////
//?????????? ???????? ??????? ??????? ?????? ??????
////////////////////////////////////////////////////

void
putch(unsigned char byte)
{
	/* output one byte */
	while(!TXIF)	/* set when register is empty */
		continue;
	TXREG = byte;
}

unsigned char
getch() {
	/* retrieve one byte */
	while(!RCIF)	/* set when register is not empty */
		continue;
	return RCREG;
}


