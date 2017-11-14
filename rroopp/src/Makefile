CC=gcc
CFLAGS=-fpic -fno-stack-protector -masm=intel -O2
LD=-fvisibility=hidden -shared
OBJ=library.o ip.o
.PHONY: clean

all: launcher.so

launcher.so: ${OBJ}
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
