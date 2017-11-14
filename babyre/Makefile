CC=gcc
CFLAGS=-O2
LD=-O2 
OBJ=main.o rsa.o numCalc.o
.PHONY: clean

all: babyre 

babyre: ${OBJ}
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
