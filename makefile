SRC_DIR=Source

COMPILER = g++
VERSION = -std=c++11
CC = ${COMPILER} ${VERSION}

SRC_FILES_S := $(wildcard $(SRC_DIR)/Router/*.cpp, wildcard $(SRC_DIR)/Router/*.h)
SRC_FILES_C := $(wildcard $(SRC_DIR)/Receiver/*.cpp, wildcard $(SRC_DIR)/Receiver/*.h)

all: clean router.out client.out

server.out: $(SRC_FILES_S)
	${CC} ${SRC_DIR}/Router/*.cpp -o router.out

Receiver.out: $(SRC_FILES_C)
	${CC} ${SRC_DIR}/Client/*.cpp -o client.out

clean:
	rm -f *.out