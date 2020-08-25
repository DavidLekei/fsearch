CC := mpicc

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
INC_DIR := ./include

EXE := $(BIN_DIR)/fsearch
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CFLAGS   := -Wall -I$(INC_DIR)
LDFLAGS  := -Llib
LDLIBS   := -lm -lpthread

MPIFLAGS := -Wall -I$(INC_DIR) -D USE_MPI

.PHONY: all clean

all: $(EXE)

mpi: $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(MPIFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)