#include "stdio.h"
#include "yams.h"


unsigned int ramSize;
unsigned char *ram;
unsigned char *rom;
unsigned char memMode = 0;
int changeMapAfterAccess = 0;

unsigned int MEM_GetByte( unsigned int addr ) {
	if( addr >= 0xE00000 ) {
		//VPA
		if ( addr < 0xE80000 ) { //no device
			return 0;
		}
		if ( addr < 0xF00000 ) {
			//printf( "WARNING: VIA access unsupported\n" );
		}
		if ( addr < 0xF80000 ) { //no device
			return 0;
		}
		if ( addr < 0xFA0000 ) {
			printf( "WARNING: Access to reserved area at 0x%x\n", addr );
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
					return 0;
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
					changeMapAfterAccess = 1;
					return rom[( addr - 0x400000 ) % currentSystem->romSize];
				}
				if ( addr < 0x580000 ) {
					changeMapAfterAccess = 1;
					return 0;
				}
				if ( addr < 0x600000 ) {
					changeMapAfterAccess = 1;
					if ( addr % 2 == 0 ) {
						printf( "WARNING: SCSI read - unsupported\n" );
						return 0;
					} else {
						printf( "WARNING: SCSI write - unsupported\n" );
						return 0;
					}
				}
				return ram[(addr - 0x600000)];
			}
		}

		//beyond this point, accesses are the same regardless of memory mode
		printf( "WARNING: Upper memory hardware access unsupported\n" );
		return 0;

	}
}

unsigned int m68k_read_memory_8( unsigned int addr ) {
	unsigned int retVal = MEM_GetByte( addr );
	//printf("read8 a:%x v:0x%x\n", addr, retVal );
	if ( changeMapAfterAccess == 1 ) {
		memMode = memMode ^ 1;
		changeMapAfterAccess = 0;
	}
	return retVal;
}

unsigned int m68k_read_memory_16( unsigned int addr ) {
	unsigned int retVal = 	( MEM_GetByte( addr ) << 8 ) + 
							( MEM_GetByte( addr + 1) );
							
	//printf( "read16 a:%x v:%x\n", addr, retVal );
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

	//printf( "read32 a:%x v:%x\n", addr, retVal );
	if ( changeMapAfterAccess == 1 ) {
		memMode = memMode ^ 1;
		changeMapAfterAccess = 0;
	}
	return retVal;
}

void m68k_write_memory_8( unsigned int addr, unsigned int val ) {
	printf( "write8 a:%x v:%x\n", addr, val );
}

void m68k_write_memory_16( unsigned int addr, unsigned int val ) {
	printf( "write16 a:%x v:%x\n", addr, val );
}

void m68k_write_memory_32( unsigned int addr, unsigned int val ) {
	printf( "write32 a:%x v:%x\n", addr, val );
}