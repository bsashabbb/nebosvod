CC = clang

CFLAGS = -Wall -Wextra
DEBUG_FLAGS = -O0 -g -fsanitize=address -fno-omit-frame-pointer
RELEASE_FLAGS = -O3

LDFLAGS = -lraylib -lm 
DEBUG_LDFLAGS = -fsanitize=address

TARGET = build/main

SRC := $(wildcard \
	*.c \
	core/*.c \
	physics/*.c \
	physics/gravity/*.c \
	physics/collision/*.c \
	physics/spatial/*.c \
	render/*.c \
	spawn/*.c \
	util/*.c \
	ui/*.c)

OBJ := $(SRC:.c=.o)

all: release

release: CFLAGS += $(RELEASE_FLAGS)
release: $(TARGET)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: LDFLAGS += $(DEBUG_LDFLAGS)
debug: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)