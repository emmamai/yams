#include <stdio.h>
#include <stdlib.h>
#include "yams.h"

unsigned char via_vBufA = 0xff;
unsigned char via_vBufB = 0xff;
unsigned char via_vDirA = 0xff;
unsigned char via_vDirB = 0xff;

unsigned char via_vT1C =  0xff;
unsigned char via_vT1CH = 0xff;
unsigned char via_vT1L =  0xff;
unsigned char via_vT1LH = 0xff;
unsigned char via_vT2C =  0xff;
unsigned char via_vT2CH = 0xff;

unsigned char via_vPCR =  0xff;
unsigned char via_vACR =  0xff;
unsigned char via_vIFR =  0xff;
unsigned char via_vIER =  0xff;
unsigned char via_vSR  =  0xff;

enum VIA_ADDR {
	vBase = 0xEFE1FE,
	vBufB = 512*0,
	vDirB = 512*2,
	vDirA = 512*3,
	vT1C  = 512*4,
	vT1CH = 512*5,
	vT1L  = 512*6,
	vT1LH = 512*7,
	vT2C  = 512*8,
	vT2CH = 512*9,
	vSR   = 512*10,
	vACR  = 512*11,
	vPCR  = 512*12,
	vIFR  = 512*13,
	vIER  = 512*14,
	vBufA = 512*15
};

unsigned char VIA_Read( const unsigned int addr ) {
	if ( addr & 0x000001 ) {
		printf( "Warning: odd VIA read\n" );
		return rand();
	}
	switch ( addr & 0x1E00 ) {
		case vBufA:
			return via_vBufA;
		case vDirA:
			return via_vDirA;
		case vBufB:
			return via_vBufB;
		case vDirB:
			return via_vDirB;
		default:
			printf( "Warning: VIA_Read: unknown register offset %04x\n", addr & 0x1E00 );
			return rand();
	}
}

void VIA_Write( const unsigned int addr, const unsigned char val ) {
	if ( addr & 0x000001 ) {
		printf( "Warning: odd VIA write (addr odd)\n" );
		return;
	}
	switch ( addr & 0x1E00 ) {
		case vBufA:
			via_vBufA = val & via_vDirA;
			return;
		case vDirA:
			via_vDirA = val;
			return;
		case vBufB:
			via_vBufB = val & via_vDirB;
			return;
		case vDirB:
			via_vDirB = val;
			return;
		default:
			printf( "Warning: VIA_Write: unknown register offset %04x\n", addr & 0x1E00 );
			return;
	}
}