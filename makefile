CFLAGS= -Wall -g
SOURCES= yams.c mem.c m68kcpu.c m68kdasm.c m68kops.c m68kopac.c m68kopdm.c m68kopnz.c vid.c
DEFINES=

all: yams

yams: $(SOURCES)
	gcc -L./ -o yams $(SOURCES) $(CFLAGS) -lxcb

clean:
	rm *.o yams
