# ──────────────────────────────────────────────────────────────────────────────
# Makefile — Emertxe Red-Black Tree
#
# TARGETS
#   all       : Builds the interactive binary (./rbt) from main.c + all modules.
#   test      : Builds the unit test binary (./test_rbt) from test.c + all modules.
#               test.c has its own main(), so main.c is EXCLUDED from this build.
#   clean     : Removes all compiled object files and both binaries.
#
# USAGE
#   make              — same as 'make all'
#   make all          — build the interactive program
#   make test         — build and prepare the test suite (run with ./test_rbt)
#   make clean        — remove all build artifacts
#
# FLAGS
#   -Wall     : Enable all standard warnings (unused variables, implicit types, etc.)
#   -Wextra   : Enable additional warnings beyond -Wall (missing initializers, etc.)
#   -g        : Include debug symbols for use with gdb and valgrind.
#               Remove -g and add -O2 for a release build.
#
# ADDING NEW SOURCE FILES
#   1. Add a new compilation rule:   newfile.o: newfile.c tree.h
#                                        $(CC) $(CFLAGS) -c newfile.c
#   2. Add newfile.o to SHARED_OBJS (if used by both binaries)
#      or to the appropriate target-specific object list.
# ──────────────────────────────────────────────────────────────────────────────

# Compiler and compilation flags
CC     = gcc
CFLAGS = -Wall -Wextra -g

# ── Object files shared by BOTH the main binary and the test binary ──────────
# These are the algorithmic modules. They contain no main() function.
SHARED_OBJS = \
    insertion.o  \
    delete.o     \
    rotation.o   \
    search.o     \
    find_min.o   \
    find_max.o   \
    display.o    \
    tree_utils.o

# ── Default target ─────────────────────────────────────────────────────────
# 'make' with no arguments builds the interactive binary.
.DEFAULT_GOAL := all

# ── Target: all (interactive binary) ───────────────────────────────────────
# Links main.o with all shared modules to produce the './rbt' executable.
all: $(SHARED_OBJS) main.o
	$(CC) $(CFLAGS) -o rbt $(SHARED_OBJS) main.o
	@echo "[BUILD] Interactive binary './rbt' ready."

# ── Target: test (unit test binary) ────────────────────────────────────────
# Links test.o with all shared modules to produce './test_rbt'.
# main.c is intentionally EXCLUDED here to avoid a duplicate main() link error.
# test.c provides its own main() entry point.
test: $(SHARED_OBJS) test.o
	$(CC) $(CFLAGS) -o test_rbt $(SHARED_OBJS) test.o
	@echo "[BUILD] Test binary './test_rbt' ready. Run with: ./test_rbt"

# ── Compilation rules for each object file ─────────────────────────────────
# Each rule compiles a single .c file into a .o object file.
# All modules depend on tree.h; display.c and main.c also depend on color.h.

main.o: main.c tree.h color.h
	$(CC) $(CFLAGS) -c main.c

test.o: test.c tree.h
	$(CC) $(CFLAGS) -c test.c

insertion.o: insertion.c tree.h
	$(CC) $(CFLAGS) -c insertion.c

delete.o: delete.c tree.h
	$(CC) $(CFLAGS) -c delete.c

rotation.o: rotation.c tree.h
	$(CC) $(CFLAGS) -c rotation.c

search.o: search.c tree.h
	$(CC) $(CFLAGS) -c search.c

find_min.o: find_min.c tree.h
	$(CC) $(CFLAGS) -c find_min.c

find_max.o: find_max.c tree.h
	$(CC) $(CFLAGS) -c find_max.c

display.o: display.c tree.h color.h
	$(CC) $(CFLAGS) -c display.c

tree_utils.o: tree_utils.c tree.h
	$(CC) $(CFLAGS) -c tree_utils.c

# ── Target: clean ───────────────────────────────────────────────────────────
# Removes all generated files. Safe to run at any time — missing files are
# silently ignored by the shell's wildcard expansion.
clean:
	rm -f *.o rbt test_rbt
	@echo "[CLEAN] Build artifacts removed."

# ── Declare phony targets ───────────────────────────────────────────────────
# Tells make that 'all', 'test', and 'clean' are not actual files.
# Without this, if a file named 'clean' or 'all' ever existed in the
# directory, make would think the target was already up to date and skip it.
.PHONY: all test clean