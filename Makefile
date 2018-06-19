# Declaration of variables
CC = gcc

LIBS = -pthread -Llocal/lib -lz -Ilocal/include

CFLAGS = -Wfatal-errors -Wall -O2 -Wno-unused-result -Wno-char-subscripts

HTSLIB = local/lib/libhts.a

SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:.c=.o)

EXEC = cvar

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) $(HTSLIB) -o $(EXEC) $(LIBS)

# To obtain object files
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXEC)

src/main.o: src/argument.h src/bam.h
src/utils.o: src/utils.h
src/argument.o: src/argument.h src/utils.h
src/bam.o: src/bam.h
