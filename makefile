ODIR=./build/objs
BDIR=./build/bin

IDIR=./src
SDIR=./src

CC=gcc
LINK=g++
LLVMCONF=llvm-config
DFLAGS=-g -O0 -fsanitize=address,undefined
RFLAGS=-O3
CFLAGS=`$(LLVMCONF) --cflags` -I$(IDIR) -Wall $(RFLAGS)
LIBS=`$(LLVMCONF) --ldflags --libs --system-libs`

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
	$(LINK) $(CFLAGS) -o $@ $^ $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) -c -o $@ $<
	
.PHONY: check
check: all
	bash ./test/run-test.sh ./test/tests/
	
.PHONY: clean
clean:
	rm -fr $(ODIR)/*

.PHONY: cleanall
cleanall:
	rm -fr $(ODIR)/* $(BDIR)/*

