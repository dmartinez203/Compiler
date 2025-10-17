CC = gcc
LEX = flex
YACC = bison

# Build flags
# Default: optimized release build. Use `make debug` for an unoptimized, instrumented build.
CFLAGS = -O2 -march=native -pipe -Wall

# Debug flags: no optimizations, include debug symbols
DEBUGFLAGS = -g -O0 -Wall

TARGET = minicompiler
OBJS = lex.yy.o parser.tab.o main.o ast.o symtab.o codegen.o tac.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

.PHONY: debug release rebuild

# Build a debug version with symbols and no optimizations
debug: CFLAGS := $(DEBUGFLAGS)
debug: clean $(TARGET)
	@echo "Built debug version: $(TARGET)"

# Explicit release target (same as default)
release: clean $(TARGET)
	@echo "Built release version: $(TARGET)"

# Rebuild from scratch
rebuild: clean all
	@echo "Rebuilt all targets"

lex.yy.c: scanner.l parser.tab.h
	$(LEX) scanner.l

parser.tab.c parser.tab.h: parser.y
	$(YACC) -d parser.y

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c

parser.tab.o: parser.tab.c
	$(CC) $(CFLAGS) -c parser.tab.c

main.o: main.c ast.h codegen.h tac.h
	$(CC) $(CFLAGS) -c main.c

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

symtab.o: symtab.c symtab.h
	$(CC) $(CFLAGS) -c symtab.c

codegen.o: codegen.c codegen.h ast.h symtab.h
	$(CC) $(CFLAGS) -c codegen.c

tac.o: tac.c tac.h ast.h
	$(CC) $(CFLAGS) -c tac.c

clean:
	rm -f $(TARGET) $(OBJS) lex.yy.c parser.tab.c parser.tab.h *.s

test: $(TARGET)
	./$(TARGET) test.c test.s
	@echo "\n=== Generated MIPS Code ==="
	@cat test.s

.PHONY: all clean test