CC = clang
CFLAGS = -g -Wall
LFLAGS = -lSDL2 -lSDL2_image
SRC = src
OBJ = obj
SRCS = $(wildcard $(SRC)/*.c)
OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
BIN = bin
BINARY = $(BIN)/tunel-survivors

DIR_GUARD = @mkdir -p $(@D)

all: $(BINARY)

release: CFLAGS=-Wall -O2 -DNDEBUG
release: clean
release: $(BINARY)

$(BINARY): $(OBJS)
	$(DIR_GUARD)
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(DIR_GUARD)
	$(CC) $(CFLAGS) $(LFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BIN)/* $(OBJ)/*
