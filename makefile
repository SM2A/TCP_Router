SRC_DIR=Source

COMPILER = g++
VERSION = -std=c++11
CC = ${COMPILER} ${VERSION}

SRC_FILES_S := $(wildcard $(SRC_DIR)/Router/*.cpp, wildcard $(SRC_DIR)/Router/*.h, wildcard $(SRC_DIR)/Packet/*.cpp, wildcard $(SRC_DIR)/Packet/*.h)
SRC_FILES_C := $(wildcard $(SRC_DIR)/Client/*.cpp, wildcard $(SRC_DIR)/Client/*.h, wildcard $(SRC_DIR)/Packet/*.cpp, wildcard $(SRC_DIR)/Packet/*.h)

all: clean router.out client.out

router.out: $(SRC_FILES_S)
	${CC} ${SRC_DIR}/Router/*.cpp -pthread -o router.out

client.out: $(SRC_FILES_C)
	${CC} ${SRC_DIR}/Client/*.cpp -o client.out

clean:
	rm -f *.out