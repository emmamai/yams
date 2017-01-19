#include <stdio.h>
#include <stdlib.h>
#include "yams.h"

extern unsigned char via_vBufA;
unsigned int ramSize;
unsigned char *ram;
unsigned char *rom;
unsigned char memMode = 0;
int changeMapAfterAccess = 0;

unsigned char MEM_GetByte( const unsigned int addr ) {
	switch ( addr & 0xf00000 ) {
		case 0x000000:
		case 0x100000:
		case 0x200000:
		case 0x300000:
			if ( ( via_vBufA & 0x10 ) == 0x10 )
				return rom[addr % currentSystem->romSize];
			return ram[addr % ramSize];
		case 0x400000:
			if ( ( via_vBufA & 0x10 ) == 0x10 )
				changeMapAfterAccess = 1;
			return rom[( addr - 0x400000 ) % currentSystem->romSize];
		case 0x600000:
		case 0x700000:
			if ( ( via_vBufA & 0x10 ) == 0x10 )
				return ram[( addr - 0x600000) % ramSize];
			return 0;
		case 0x900000:
		case 0xB00000:
			fprintf( stderr, "MEM: Access to SCC unimplemented (0x%06X)\n", addr );
			return 0;
		case 0xD00000:
			fprintf( stderr, "MEM: Access to IWM unimplemented (0x%06X)\n", addr );
			return 0x1f; //dummy value prevents hang
		case 0xE00000:
			if ( addr & 0x080000 ) { //VIA
				return VIA_Read( addr );
			} else {
				fprintf( stderr, "MEM: Access to reserved region at 0x%06X\n", addr );
				return 0;
			}
		case 0xF00000:
			if ( addr & 0x080000 ) {
				//TODO: add support for loading a file as a test rom?
				//fprintf( stderr, "MEM: Access to test sofware at 0x%06X\n", addr );
				return 0;
			}
			fprintf( stderr, "MEM: Access to phase read unimplemented (0x%06X)\n", addr ); 
			return 0;
		case 0x500000: //bad accesses
		case 0xA00000:
		case 0xC00000:
			fprintf( stderr, "MEM: Access to reserved region at 0x%06X\n", addr );
			return 0;
		default:
		case 0x800000:
			fprintf( stderr, "MEM: Access to unassigned region at 0x%06X\n", addr );
			return 0;
	}
}

void MEM_SetByte( const unsigned int addr, const unsigned char val ) {
	switch ( addr & 0xf00000 ) {
		case 0x000000:
		case 0x100000:
		case 0x200000:
		case 0x300000:
			if ( ( via_vBufA & 0x10 ) == 0x00 )
				ram[addr % ramSize] = val;
			return;
		case 0x400000:
			return;
		case 0x600000:
		case 0x700000:
			if ( ( via_vBufA & 0x10 ) == 0x10 )
				ram[( addr - 0x600000) % ramSize] = val;
			return;
		case 0x900000:
		case 0xB00000:
			fprintf( stderr, "MEM: Access to SCC unimplemented (0x%06X)\n", addr );
			return;
		case 0xD00000:
			fprintf( stderr, "MEM: Access to IWM unimplemented (0x%06X)\n", addr );
			return;
		case 0xE00000:
			if ( addr & 0x080000 ) //VIA
				VIA_Write( addr );
			else
				fprintf( stderr, "MEM: Access to reserved region at 0x%06X\n", addr );
			return;
		case 0xF00000:
			if ( addr & 0x080000 )
				fprintf( stderr, "MEM: Access to test sofware at 0x%06X\n", addr );
			else
				fprintf( stderr, "MEM: Access to phase read unimplemented (0x%06X)\n", addr ); 
			return;
		case 0x500000: //bad accesses
		case 0xA00000:
		case 0xC00000:
			fprintf( stderr, "MEM: Access to reserved region at 0x%06X\n", addr );
			return;
		default:
		case 0x800000:
			fprintf( stderr, "MEM: Access to unassigned region at 0x%06X\n", addr );
			return;
	}
}

void MEM_SetByteold( unsigned int addr, unsigned char val ) {
	if ( memMode == 1 ) { //normal map
		if ( addr < 0x400000 ) {
			ram[addr % ramSize] = val;
		}
	} else { //boot map
		if ( addr < 0x600000 ) {
			return;
		}
		if ( addr < 0x800000 ) {
			ram[( addr - 0x600000 ) % ramSize] = val;
		}
	}
}

unsigned int m68k_read_memory_8( unsigned int addr ) {
	unsigned int retVal = MEM_GetByte( addr );
	#ifdef DPRINT_MEM_ACCESS
	printf("read8 a:%x v:0x%x\n", addr, retVal );
	#endif
	memMode = memMode | changeMapAfterAccess;
	return retVal;
}

unsigned int m68k_read_memory_16( unsigned int addr ) {
	unsigned int retVal = 	( MEM_GetByte( addr ) << 8 ) + 
							( MEM_GetByte( addr + 1) );
							
	#ifdef DPRINT_MEM_ACCESS
	printf( "read16 a:%x v:%x\n", addr, retVal );
	#endif
	memMode = memMode | changeMapAfterAccess;
	return retVal;
}

unsigned int m68k_read_memory_32( unsigned int addr ) {
	unsigned int retVal = 	( MEM_GetByte( addr ) << 24 ) + 
							( MEM_GetByte( addr + 1) << 16 ) +
							( MEM_GetByte( addr + 2) << 8 ) +
							( MEM_GetByte( addr + 3) );

	#ifdef DPRINT_MEM_ACCESS
	printf( "read32 a:%x v:%x\n", addr, retVal );
	#endif
	memMode = memMode | changeMapAfterAccess;
	return retVal;
}

void m68k_write_memory_8( unsigned int addr, unsigned int val ) {
	#ifdef DPRINT_MEM_ACCESS
	printf( "write8 a:%x v:%x\n", addr, val );
	#endif
	MEM_SetByte( addr, val );
}

void m68k_write_memory_16( unsigned int addr, unsigned int val ) {
	#ifdef DPRINT_MEM_ACCESS
	printf( "write16 a:%x v:%x\n", addr, val );
	#endif
	MEM_SetByte( addr + 1, val );
	MEM_SetByte( addr + 0, val >> 8 );
}

void m68k_write_memory_32( unsigned int addr, unsigned int val ) {
	#ifdef DPRINT_MEM_ACCESS
	printf( "write32 a:%x v:%x\n", addr, val );
	#endif
	MEM_SetByte( addr + 3, val );
	MEM_SetByte( addr + 2, val >> 8 );
	MEM_SetByte( addr + 1, val >> 16 );
	MEM_SetByte( addr + 0, val >> 24 );
}