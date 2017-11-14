CC=gcc
CFLAGS=-m32
LD=-m32
OBJ=main.o 
.PHONY: clean

all: pwn2

pwn2: ${OBJ}
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
