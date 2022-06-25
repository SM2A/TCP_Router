SRC_DIR=Source

COMPILER = g++
VERSION = -std=c++11
CC = ${COMPILER} ${VERSION}

SRC_FILES_R := $(wildcard $(SRC_DIR)/Router/*.cpp, wildcard $(SRC_DIR)/Router/*.h)
SRC_FILES_S := $(wildcard $(SRC_DIR)/Sender/*.cpp, wildcard $(SRC_DIR)/Sender/*.h)
SRC_FILES_C := $(wildcard $(SRC_DIR)/Receiver/*.cpp, wildcard $(SRC_DIR)/Receiver/*.h)

all: clean router.out receiver.out sender.out

router.out: $(SRC_FILES_R)
	${CC} ${SRC_DIR}/Router/*.cpp -o router.out

sender.out: $(SRC_FILES_S)
	${CC} ${SRC_DIR}/Sender/*.cpp -o sender.out

receiver.out: $(SRC_FILES_C)
	${CC} ${SRC_DIR}/Receiver/*.cpp -o receiver.out

clean:
	rm -f *.out