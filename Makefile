SDIR = src
IDIR = src
ODIR = obj
BDIR = bin

CC = gcc
CFLAGS = -Wextra -Wall

LIBS = -lcurl -lpthread

_DEPS = argparser.h wordlist.h help.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o argparser.o wordlist.o help.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BDIR)/cbuster: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean

clean:
	rm -rf $(BDIR)/cbuster
	rm -rf $(ODIR)
