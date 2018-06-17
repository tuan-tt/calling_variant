# Declaration of variables
CC = gcc

LIBS = -pthread

CFLAGS = -Wfatal-errors -Wall -O2 -Wno-unused-result -Wno-char-subscripts

SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:.c=.o)

EXEC = cvar

all: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) $(LIBS)

# To obtain object files
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXEC)