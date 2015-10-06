
#define YAMS_VER "0.1"
#define YAMS_BUILD "1"

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