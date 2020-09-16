ODIR=./build/objs
BDIR=./build/bin

IDIR=./src
SDIR=./src

CC=gcc
LLVMCONF=llvm-config
CFLAGS=`$(LLVMCONF) --cflags` -I$(IDIR) -O0 -g -Wall -fsanitize=address,undefined
LIBS=`$(LLVMCONF) --libs --link-static`

_SRC=$(wildcard $(SDIR)/*/*.c) $(wildcard $(SDIR)/*.c)
OBJ=$(patsubst $(SDIR)/%.c,$(ODIR)/%.o,$(_SRC))

DEPS=$(wildcard $(IDIR)/*/*.h) $(wildcard $(IDIR)/*.h)

_BIN=tumble
BIN=$(patsubst %,$(BDIR)/%,$(_BIN))

.PHONY: all
all: $(BIN) 

.PHONY: install
install: all
	cp $(BIN) /usr/bin/

$(BDIR)/tumble: $(OBJ)
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) -c -o $@ $<
	
.PHONY: clean
clean:
	rm -f $(ODIR)/*

.PHONY: cleanall
cleanall:
	rm -f $(ODIR)/* $(BDIR)/*

