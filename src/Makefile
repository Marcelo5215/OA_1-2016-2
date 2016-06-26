IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR) -O2 -g

ODIR=../obj
LDIR =../lib

LIBS=-lm

_DEPS = index.h bt.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = index.o main.o bt.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
