
#define YAMS_VER "0.1"
#define YAMS_BUILD "2"

#define DPRINT_ASM
#define DPRINT_MEM_ACCESS
//#define NO_MEM_WARNINGS

extern unsigned int ramSize;
extern unsigned char *ram;
extern unsigned char *rom;

extern unsigned char memMode;

typedef struct {
	char name[16];
	char romName[16];
	unsigned int romSize;
	unsigned int validRamSizes[16];
} systemInfo_t;

extern systemInfo_t *currentSystem;

void Quit();

int VID_Init();
void VID_Cleanup();
void VID_SetPixel( int x, int y, char color );