SRC_DIR=Source

COMPILER = g++
VERSION = -std=c++11
CC = ${COMPILER} ${VERSION}

SRC_FILES_S := $(wildcard $(SRC_DIR)/Router/*.cpp, wildcard $(SRC_DIR)/Router/*.h, wildcard $(SRC_DIR)/Packet/*.cpp, wildcard $(SRC_DIR)/Packet/*.h)
SRC_FILES_C := $(wildcard $(SRC_DIR)/Client/*.cpp, wildcard $(SRC_DIR)/Client/*.h, wildcard $(SRC_DIR)/Packet/*.cpp, wildcard $(SRC_DIR)/Packet/*.h)

all: clean router.out receiver.out

router.out: $(SRC_FILES_S)
	${CC} ${SRC_DIR}/Router/*.cpp -pthread -o router.out

receiver.out: $(SRC_FILES_C)
	${CC} ${SRC_DIR}/Receiver/*.cpp -o receiver.out

clean:
	rm -f *.out