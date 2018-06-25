# Declaration of variables
CC = gcc

LIBS = -pthread -Llocal/lib -lz -Ilocal/include -lm

CFLAGS = -Wfatal-errors -Wall -O2 -Wno-unused-result -Wno-char-subscripts

HTSLIB = local/lib/libhts.a

SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:.c=.o)

EXEC = cvar

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) $(HTSLIB) -o bin/$(EXEC) $(LIBS)
	rm -f $(OBJECTS)

# To obtain object files
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

