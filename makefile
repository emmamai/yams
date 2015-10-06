CFLAGS= -Wall -g
DEFINES=

all: yams

yams: yams.c
	gcc -L./ -o yams yams.c -lxcb $(CFLAGS)

clean:
	rm *.o yams
