# Makefile for MINIX 3.1.8 with LLVM-20 Magic framework tests

CC = clang
CFLAGS = -Wall -Wextra -std=c11 -O2 -g
LDFLAGS = 
INCLUDES = -I../minix/llvm/include -I../minix -I.

# Test executables
TESTS = test_magic_framework test_memory_safety test_type_analysis

# Source files
MAGIC_TEST_SRCS = test_magic_framework.c
MEMORY_TEST_SRCS = test_memory_safety.c
TYPE_TEST_SRCS = test_type_analysis.c

# Object files
MAGIC_TEST_OBJS = $(MAGIC_TEST_SRCS:.c=.o)
MEMORY_TEST_OBJS = $(MEMORY_TEST_SRCS:.c=.o)
TYPE_TEST_OBJS = $(TYPE_TEST_SRCS:.c=.o)

# Default target
all: $(TESTS)

# Test executables
test_magic_framework: $(MAGIC_TEST_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

test_memory_safety: $(MEMORY_TEST_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

test_type_analysis: $(TYPE_TEST_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# Object file compilation
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Run all tests
test: $(TESTS)
	@echo "=== Running all tests ==="
	@for test in $(TESTS); do \
		echo "Running $$test..."; \
		./$$test || exit 1; \
		echo ""; \
	done
	@echo "=== All tests completed successfully ==="

# Run individual tests
test-magic: test_magic_framework
	./test_magic_framework

test-memory: test_memory_safety
	./test_memory_safety

test-type: test_type_analysis
	./test_type_analysis

# Clean up
clean:
	rm -f *.o $(TESTS)

# Install tests (for system integration)
install: $(TESTS)
	mkdir -p /usr/local/bin/minix-tests
	cp $(TESTS) /usr/local/bin/minix-tests/

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build all tests"
	@echo "  test         - Run all tests"
	@echo "  test-magic   - Run Magic framework tests"
	@echo "  test-memory  - Run memory safety tests"
	@echo "  test-type    - Run type analysis tests"
	@echo "  clean        - Remove build artifacts"
	@echo "  install      - Install tests to system"
	@echo "  help         - Show this help message"

.PHONY: all test test-magic test-memory test-type clean install help
