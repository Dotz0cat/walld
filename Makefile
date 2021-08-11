CFLAGS?= -O2 -g
CPPFLAGS= $(shell pkg-config --cflags libconfig MagickWand)
LDLIBS= $(shell pkg-config --libs libconfig MagickWand) -lm

SRCDIR= src
OBJDIR= obj
PREFIX?= /usr/local

build: $(OBJDIR) $(OBJDIR)/main.o $(OBJDIR)/config.o $(OBJDIR)/magic.o $(OBJDIR)/list.o $(OBJDIR)/loop.o
	@$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJDIR)/main.o $(OBJDIR)/config.o $(OBJDIR)/magic.o $(OBJDIR)/list.o $(OBJDIR)/loop.o -o walld $(LDLIBS) $(LDFLAGS)

$(OBJDIR):
	@mkdir $(OBJDIR)

$(OBJDIR)/main.o: $(SRCDIR)/main.h $(SRCDIR)/main.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $(SRCDIR)/main.c -o $(OBJDIR)/main.o

$(OBJDIR)/config.o: $(SRCDIR)/config.h $(SRCDIR)/config.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $(SRCDIR)/config.c -o $(OBJDIR)/config.o

$(OBJDIR)/magic.o: $(SRCDIR)/magic.h $(SRCDIR)/magic.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $(SRCDIR)/magic.c -o $(OBJDIR)/magic.o

$(OBJDIR)/list.o: $(SRCDIR)/list.h $(SRCDIR)/list.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $(SRCDIR)/list.c -o $(OBJDIR)/list.o

$(OBJDIR)/loop.o: $(SRCDIR)/loop.h $(SRCDIR)/loop.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $(SRCDIR)/loop.c -o $(OBJDIR)/loop.o

.PHONEY: clean

clean:
	@-rm $(OBJDIR)/main.o
	@-rm $(OBJDIR)/config.o 
	@-rm $(OBJDIR)/magic.o 
	@-rm $(OBJDIR)/list.o
	@-rm $(OBJDIR)/loop.o
	@-rmdir $(OBJDIR)
	@-rm walld

.PHONEY: install

install: build
	@install -D walld -t ${DESTDIR}${PREFIX}/bin

.PHONY: uninstall

uninstall: 
	@rm -f ${DESTDIR}${PREFIX}/bin/walld