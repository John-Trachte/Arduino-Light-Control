# Usage:
# make			compile to binary
# make clean	remove binary

.PHONY = all clean

CC = gcc
LINKERFLAGS = -lGL -lGLEW -lglfw

all: Control.c
	${CC} -o test Control.c ${LINKERFLAGS}

clean:
	rm -rvf test