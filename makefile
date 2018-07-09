# Flagship Engine makefile (windows only at the moment)

CC = clang
CFLAGS = -std=c11 -m64 -Wall -Wno-deprecated
TARGET = flagship.exe
export

all:
	@ $(MAKE) -s -C ./bin

clean:
	@ $(MAKE) -s -C ./bin clean

run: all
	@ flagship.exe