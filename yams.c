#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "yams.h"
#include "m68k.h"

#define NUM_SUPPORTED_SYSTEMS 4

systemInfo_t mac128 = {
	"mac128",
	"mac128.rom",
	65536,
	{ 131072, 524288 }
};

systemInfo_t mac128ke = {
	"mac128ke",
	"mac128ke.rom",
	131072,
	{ 131072, 524288 }
};

systemInfo_t macplus = {
	"macplus",
	"macplus.rom",
	131072,
	{ 1048576, 2097152, 2621440, 4194304 }
};

systemInfo_t macse = {
	"macse",
	"macse.rom",
	262144,
	{ 1048576, 2097152, 2621440, 4194304 }
};

systemInfo_t* sysInfo[NUM_SUPPORTED_SYSTEMS] = {
	&mac128,
	&mac128ke,
	&macplus,
	&macse
};

systemInfo_t *currentSystem;

unsigned char vrambuf[342*64];

int main( int argc, char *argv[] ) {
	FILE *romFile;
	char buf[64];
	int fileSize = 0, i, j, pc;

	printf( "yams %sb%s\n", YAMS_VER, YAMS_BUILD );

	currentSystem = &mac128ke;

	for( i = 1; i < argc; i++ ) {
		printf( "checking for -m - %s\n", argv[i] );
		if ( strncmp( argv[i], "-m", 2 ) == 0 ) {
			strncpy( &buf, argv[++i], 64 );
			for( j = 0; j < NUM_SUPPORTED_SYSTEMS; j++ ) {
				if ( strncmp( buf, sysInfo[j]->name, 64 ) == 0 ) {
					currentSystem = sysInfo[j];
				}
			}
		}
	}

	ramSize = currentSystem->validRamSizes[0];
	ram = malloc( ramSize );
	int tmp = ramSize;
	memset( ram, 0xF0, ramSize );
	memset( vrambuf, 0, 342*64 );

	printf( "Emulating system \"%s\" with %dkb RAM\n", currentSystem->name, ramSize/1024 );

	romFile = fopen( currentSystem->romName, "rb" );

	if ( !romFile ) {
		printf( "FATAL: Cannot find %s\n", currentSystem->romName );
		Quit();
	}
	fseek( romFile, 0L, SEEK_END);
	fileSize = ftell( romFile );
	if ( fileSize != currentSystem->romSize ) {
		printf( "FATAL: %s is %d bytes, should be %d\n", currentSystem->romName, fileSize, currentSystem->romSize );
		free( romFile );
		Quit();
	}
	rewind( romFile );
	rom = malloc( currentSystem->romSize );
	fread( rom, 1, currentSystem->romSize, romFile );
	fclose( romFile );

	if ( VID_Init() != 0 ) {
		printf( "Video failure, exiting\n" );
		Quit();
	} 

	memMode = 0;
	m68k_init();
	m68k_set_cpu_type( M68K_CPU_TYPE_68000 );
	m68k_pulse_reset();

	int x, y;
	int n = 0;
	int run = 0;

	#ifdef DPRINT_ASM
	char *daBuf = malloc( 256 );
	#endif

	#define VID_BASE (ramSize-0x5900)
	//#define VID_BASE (0x3fa700)

	printf( "ScrnBase: 0x%06X\n", VID_BASE );

	sleep( 2 );

	while( run == 0) {
		#ifdef DPRINT_ASM
		pc = m68k_get_reg( NULL, M68K_REG_PC );
		m68k_disassemble( daBuf, m68k_get_reg( NULL, M68K_REG_PC ), M68K_CPU_TYPE_68000 );
		printf( "%6x : %s\n", pc, daBuf );
		if ( pc > 0x08000000 ) {
			printf( "PC out of bounds - exiting" );
			run = 1;
		}
		m68k_execute( 1 );
		#else
		m68k_execute( 133333 );
		#endif
		for ( y = 0; y < 342; y++ ) {
			for ( x = 0; x < 64; x++ ) {
				unsigned int offset = (y*64)+(x);
				unsigned char b = ram[(VID_BASE+offset)%ramSize];
				//unsigned char b = MEM_GetByte( VID_BASE + offset );
				//printf( "offset 0x%06X vbuf %d ram %d\n", offset, vrambuf[offset], b );
				if ( vrambuf[offset] != b ) {
					VID_SetPixel( ( x * 8 ) + 0 , y, b & ( 0x80 >> 0 ) );
					VID_SetPixel( ( x * 8 ) + 1 , y, b & ( 0x80 >> 1 ) );
					VID_SetPixel( ( x * 8 ) + 2 , y, b & ( 0x80 >> 2 ) );
					VID_SetPixel( ( x * 8 ) + 3 , y, b & ( 0x80 >> 3 ) );
					VID_SetPixel( ( x * 8 ) + 4 , y, b & ( 0x80 >> 4 ) );
					VID_SetPixel( ( x * 8 ) + 5 , y, b & ( 0x80 >> 5 ) );
					VID_SetPixel( ( x * 8 ) + 6 , y, b & ( 0x80 >> 6 ) );
					VID_SetPixel( ( x * 8 ) + 7 , y, b & ( 0x80 >> 7 ) );
					vrambuf[offset] = b;
					VID_Flush();
				}
			}
		}
		if ( n % 60 == 0 ) {
			printf( "================================ %d ================================\n", n );
		}
		n++;
	}
	#ifdef DPRINT_ASM
	free( daBuf );
	#endif
	

	Quit();
}

void Quit() {
	VID_Cleanup();
	free( ram );
	free( rom );
	exit( 0 );
}