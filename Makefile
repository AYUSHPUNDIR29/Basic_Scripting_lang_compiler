# Detect Windows (MSYS/MinGW/Command Prompt)
ifeq ($(OS),Windows_NT)
    EXE = .exe
    RM = del /Q /F
    PY = python
else
    EXE =
    RM = rm -f
    PY = python3
endif

CC = gcc
CFLAGS = -Wall -Wno-unused-function -I./include
LEX = flex
YACC = bison
YFLAGS = -d

TARGET = compiler$(EXE)

SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = tests

# Generated sources
LEX_SRC = $(SRC_DIR)/lex.yy.c
YACC_SRC = $(SRC_DIR)/parser.tab.c
YACC_HEADER = $(INCLUDE_DIR)/parser.tab.h

# All C source files (excluding generated ones)
C_SRCS = $(wildcard $(SRC_DIR)/*.c) main.c
C_SRCS := $(filter-out $(LEX_SRC) $(YACC_SRC), $(C_SRCS))

# All sources including generated
SRCS = $(C_SRCS) $(LEX_SRC) $(YACC_SRC)

# All object files
OBJS = $(SRCS:.c=.o)

.PHONY: all clean test run

all: $(TARGET)

# --- Main target ---
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# --- Dependencies and Rules ---

# Rule for Bison: generates parser.tab.c and parser.tab.h
$(YACC_SRC) $(YACC_HEADER): $(SRC_DIR)/parser.y
	$(YACC) $(YFLAGS) --header=$(YACC_HEADER) -o $(YACC_SRC) $(SRC_DIR)/parser.y

# Rule for Flex: generates lex.yy.c. Depends on the parser header.
$(LEX_SRC): $(SRC_DIR)/lexer.l $(YACC_HEADER)
	$(LEX) -o$(LEX_SRC) $(SRC_DIR)/lexer.l

# All C object files depend on the parser header being generated first.
# This rule ensures parser.tab.h is created before any C files are compiled.
$(OBJS): $(YACC_HEADER)

# --- Generic Compilation Rule ---
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- Housekeeping ---
clean:
	-$(RM) $(TARGET) $(OBJS) $(LEX_SRC) $(YACC_SRC) $(YACC_HEADER) core 2> NUL || true

# Run tests (works in MSYS2, Git Bash, or Unix shells)
test: all
	@echo "Running tests..."
	./$(TARGET) < $(TEST_DIR)/test_variables.txt

# Run the Python GUI (ensure compiler is built first)
run: all
	$(PY) compiler_gui.py 