CFLAGS= -Wall -g -O2
OBJS= yams.o mem.o vid.o m68kcpu.o m68kdasm.o m68kops.o m68kopac.o m68kopdm.o m68kopnz.o
LIBS= -lxcb
DEFINES=

all: yams

yams: $(OBJS)
	gcc -L./ -o yams $(OBJS) $(CFLAGS) $(LIBS)

clean:
	rm *.o yams
