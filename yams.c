#include <stdlib.h>
#include <stdio.h>

#include "yams.h"
#include "m68k.h"

systemInfo_t mac128 = {
	"mac128",
	"Mac128.ROM",
	65536,
	{ 131072 }
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
	char *daBuf = malloc( 256 );
	int n = 0;
	int run = 0;

	while( run == 0) {
		for ( i = 0; i < 133333; i++ ) {
			m68k_disassemble( daBuf, m68k_get_reg( NULL, M68K_REG_PC ), M68K_CPU_TYPE_68000 );
			//printf( "%s\n", daBuf );
			m68k_execute( 1 );
		}
		for ( i = 0; i < ( 512 * 384 ); i++ ) {
			VID_SetPixel( i % 512, i / 512, ram[ 0x01A700 + i ] );
		}
		if ( n++ > 300 ) {
			run = 1;
		}
	}

	free( daBuf );

	

	Quit();
}

void Quit() {
	VID_Cleanup();
	free( ram );
	free( rom );
	exit( 0 );
}