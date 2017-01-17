#include <stdio.h>
#include <stdlib.h>
#include "yams.h"


unsigned int ramSize;
unsigned char *ram;
unsigned char *rom;
unsigned char memMode = 0;
int changeMapAfterAccess = 0;

unsigned int MEM_GetByte( unsigned int addr ) {
	if( addr >= 0xD00000 ) {
		if ( addr < 0xE00000 ) {
			printf( "WARNING: IWM read unsupported, 0x%x\n", addr );
			return 0x1f;
		} else if ( addr < 0xE80000 ) { //no device
			return 0;
		}
		if ( addr < 0xF00000 ) {
			//printf( "WARNING: VIA access unsupported\n" );
			return rand();
		}
		if ( addr < 0xF80000 ) { //no device
			return 0;
		}
		if ( addr < 0xFA0000 ) {
			return 0;
		}
		if ( addr < 0xFFFFF0 ) { //no device
			return 0;
		}
		printf( "WARNING: Autovector read unsupported, 0x%x\n", addr );
		return 0;
	} else {	
		//Everything below 0x800000 depends on which mode the address map is in
		if ( addr < 0x800000 ) {
			if ( memMode == 1 ) { //normal map
				if ( addr < 0x400000 ) {
					return ram[addr % ramSize];
				}
				if ( addr < 0x500000 ) {
					return rom[(addr - 0x400000) % currentSystem->romSize];
				}
				if ( addr < 0x520000 ) {
					printf( "WARNING: Access to reserved memory area at 0x%x", addr );
					return rand();
				}
				if ( addr < 0x600000 ) {
					if ( addr % 2 == 0 ) {
						printf( "WARNING: SCSI read - unsupported\n" );
						return 0;
					} else {
						printf( "WARNING: SCSI write - unsupported\n" );
						return 0;
					}
				}
				//no device
				return 0;
			} else { //power-up map
				if ( addr < 0x100000 ) {
					return rom[addr % currentSystem->romSize];
				}
				if ( addr < 0x400000 ) {
					return 0;
				}
				if ( addr < 0x500000 ) {
					//changeMapAfterAccess = 1;
					return rom[( addr - 0x400000 ) % currentSystem->romSize];
				}
				if ( addr < 0x580000 ) {
					//changeMapAfterAccess = 1;
					return 0;
				}
				if ( addr < 0x600000 ) {
					//changeMapAfterAccess = 1;
					if ( addr % 2 == 0 ) {
						printf( "WARNING: SCSI read - unsupported\n" );
						return 0;
					} else {
						printf( "WARNING: SCSI write - unsupported\n" );
						return 0;
					}
				}
				if ( addr < 0x680000 ) {
					return ram[(addr - 0x600000) % ramSize];
				}
			}
		}
		//beyond this point, accesses are the same regardless of memory mode
		//printf( "WARNING: Upper memory hardware access unsupported\n" );
		return 0;

	}
}

void MEM_SetByte( unsigned int addr, unsigned char val ) {
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
	if ( changeMapAfterAccess == 1 ) {
		memMode = memMode ^ 1;
		changeMapAfterAccess = 0;
	}
	return retVal;
}

unsigned int m68k_read_memory_16( unsigned int addr ) {
	unsigned int retVal = 	( MEM_GetByte( addr ) << 8 ) + 
							( MEM_GetByte( addr + 1) );
							
	#ifdef DPRINT_MEM_ACCESS
	printf( "read16 a:%x v:%x\n", addr, retVal );
	#endif
	if ( changeMapAfterAccess == 1 ) {
		memMode = memMode ^ 1;
		changeMapAfterAccess = 0;
	}
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
	if ( changeMapAfterAccess == 1 ) {
		memMode = memMode ^ 1;
		changeMapAfterAccess = 0;
	}
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
	MEM_SetByte( addr, val );
	MEM_SetByte( addr + 1, val >> 8 );
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