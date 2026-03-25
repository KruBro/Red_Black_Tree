CC = gcc
CFLAGS = -Wall -Wextra -g

# List of all the core utility files (everything except main files)
SRCS = insertion.c display.c tree_utils.c rotation.c
OBJS = $(SRCS:.c=.o)

# Executable names
EXEC = rb_tree
TEST_EXEC = test_rb_tree

# Default target compiles the main interactive program
all: $(EXEC)

# Link the main program
$(EXEC): $(OBJS) main.o
	$(CC) $(CFLAGS) -o $@ $^

# Link the unit test program and run it immediately
test: $(TEST_EXEC)
	./$(TEST_EXEC)

$(TEST_EXEC): $(OBJS) test.o
	$(CC) $(CFLAGS) -o $@ $^

# Generic rule to compile any .c file into a .o file
%.o: %.c tree.h
	$(CC) $(CFLAGS) -c $<

# Clean up build artifacts
clean:
	rm -f *.o $(EXEC) $(TEST_EXEC)