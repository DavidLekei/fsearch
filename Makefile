CC := gcc

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
INC_DIR := ./include
NUKLEAR := ./lib/Nuklear

EXE := $(BIN_DIR)/fsearch
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

MPIFLAGS := -Wall -I$(INC_DIR) -D USE_MPI
CFLAGS   := -Wall -I$(INC_DIR)
DEBUGFLAGS := -Wall -I$(INC_DIR) -O0 -ggdb -D DEBUG
LDFLAGS  := -Llib
LDLIBS   := -lm -lpthread


all: objs exe

mpi: mpi_objs exe

debug: debug_objs exe

mpi_objs: $(OBJ_DIR) $(SRC_DIR)/fsearch.c
	$(CC) $(MPIFLAGS) -c $(SRC_DIR)/fsearch.c -o $(OBJ_DIR)/fsearch.o

debug_objs: $(OBJ_DIR) $(SRC_DIR)/fsearch.c
	$(CC) $(DEBUGFLAGS) -c $(SRC_DIR)/fsearch.c -o $(OBJ_DIR)/fsearch.o

objs: $(OBJ_DIR) $(SRC_DIR)/fsearch.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/fsearch.c -o $(OBJ_DIR)/fsearch.o
	$(CC) $(CFLAGS) -c $(SRC_DIR)/gui.c -o $(OBJ_DIR)/gui.o

exe: $(BIN_DIR) $(OBJ_DIR)/fsearch.o $(OBJ_DIR)/gui.o
	$(CC) $(LDFLAGS) $(OBJ_DIR)/fsearch.o  $(OBJ_DIR)/gui.o $(LDLIBS) -o $(BIN_DIR)/fsearch

$(BIN_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)