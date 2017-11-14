CC=gcc
CFLAGS=-masm=intel -fno-stack-protector
LD=-lseccomp
OBJ=main.o 
.PHONY: clean

all: babystack

babystack: ${OBJ}
	${CC}  ${OBJ} -o $@ ${LD}
	strip -s $@

${OBJ}: %.o: %.c
	${CC} -c $< -o $@ ${CFLAGS}


clean:
	-rm *.o
	-rm ./.gdb_history
	-rm ./*.idb
	-rm */peda-session-bin.txt
	-rm peda-session-bin.txt
	-rm */*~
	-rm *~
