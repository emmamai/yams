#include <stdlib.h>
#include <stdio.h>

#include "yams.h"
#include "m68k.h"

systemInfo_t macse = {
	"macse",
	"MacSE.ROM",
	262144,
	{ 524288, 1048576, 2097152, 2621440, 4194304 },
	MMU_BBU
};

systemInfo_t *currentSystem;

int main( int argc, char *argv[] ) {
	FILE *romFile;
	int fileSize = 0;

	printf( "yams %sb%s\n", YAMS_VER, YAMS_BUILD );

	//todo: read command line params
	currentSystem = &macse;

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

	//todo: actually emulate something
	memMode = 0;
	m68k_init();
	m68k_set_cpu_type( M68K_CPU_TYPE_68000 );
	m68k_pulse_reset();
	m68k_execute( 512 );

	Quit();
}

void Quit() {
	free( ram );
	free( rom );
	exit( 0 );
}