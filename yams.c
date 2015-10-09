#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "yams.h"
#include "m68k.h"


systemInfo_t mac128 = {
	"mac128",
	"Mac128.ROM",
	65536,
	{ 131072, 524288 }
};

systemInfo_t macse = {
	"macse",
	"MacSE.ROM",
	262144,
	{ 524288, 1048576, 2097152, 2621440, 4194304 }
};

systemInfo_t *currentSystem;

int main( int argc, char *argv[] ) {
	FILE *romFile;
	int fileSize = 0;

	printf( "yams %sb%s\n", YAMS_VER, YAMS_BUILD );

	//todo: read command line params
	currentSystem = &mac128;

	ramSize = currentSystem->validRamSizes[0];
	ram = malloc( ramSize );
	int tmp = ramSize;
	memset( ram, 1, ramSize );

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

	int i;
	int x, y;
	int n = 0;
	int run = 0;

	#ifdef DPRINT_ASM
	char *daBuf = malloc( 256 );
	#endif

	#define VID_BASE 0x1A700

	while( run == 0) {
		#ifdef DPRINT_ASM
		m68k_disassemble( daBuf, m68k_get_reg( NULL, M68K_REG_PC ), M68K_CPU_TYPE_68000 );
		printf( "%s\n", daBuf );
		m68k_execute( 1 );
		#else
		m68k_execute( 133333 );
		#endif
		for ( y = 0; y < 342; y++ ) {
			for ( x = 0; x < 64; x++ ) {
				VID_SetPixel( ( x * 8 ) + 0 , y, ( ram[(y*64)+(x) + VID_BASE] ) & ( 0x80 >> 0 ) );
				VID_SetPixel( ( x * 8 ) + 1 , y, ( ram[(y*64)+(x) + VID_BASE] ) & ( 0x80 >> 1 ) );
				VID_SetPixel( ( x * 8 ) + 2 , y, ( ram[(y*64)+(x) + VID_BASE] ) & ( 0x80 >> 2 ) );
				VID_SetPixel( ( x * 8 ) + 3 , y, ( ram[(y*64)+(x) + VID_BASE] ) & ( 0x80 >> 3 ) );
				VID_SetPixel( ( x * 8 ) + 4 , y, ( ram[(y*64)+(x) + VID_BASE] ) & ( 0x80 >> 4 ) );
				VID_SetPixel( ( x * 8 ) + 5 , y, ( ram[(y*64)+(x) + VID_BASE] ) & ( 0x80 >> 5 ) );
				VID_SetPixel( ( x * 8 ) + 6 , y, ( ram[(y*64)+(x) + VID_BASE] ) & ( 0x80 >> 6 ) );
				VID_SetPixel( ( x * 8 ) + 7 , y, ( ram[(y*64)+(x) + VID_BASE] ) & ( 0x80 >> 7 ) );
			}
		}
		VID_Flush();
		if ( n % 60 == 0 ) {
			printf( "%d\n", n );
		}
		
		if ( n++ > 300 ) {
			run = 1;
		}
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