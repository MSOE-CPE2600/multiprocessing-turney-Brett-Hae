CC = gcc
CFLAGS = -c -Wall -g
LDFLAGS = -ljpeg -lpthread

# Executables
EXEC1 = mandel
EXEC2 = mandelmovie

# Sources for each executable
SRC1 = mandel.c jpegrw.c
SRC2 = mandelmovie.c

# Objects
OBJ1 = $(SRC1:.c=.o)
OBJ2 = $(SRC2:.c=.o)

# Default target
all: $(EXEC1) $(EXEC2)

# Pull in dependency info for existing .o files
-include $(OBJ1:.o=.d) $(OBJ2:.o=.d)

# Build mandel
$(EXEC1): $(OBJ1)
	$(CC) $(OBJ1) $(LDFLAGS) -o $@

# Build mandelmovie
$(EXEC2): $(OBJ2)
	$(CC) $(OBJ2) $(LDFLAGS) -o $@

# Compile .c to .o and generate dependency file
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
	$(CC) -MM $< > $*.d

# Clean up
clean:
	rm -rf $(OBJ1) $(OBJ2) $(EXEC1) $(EXEC2) *.d