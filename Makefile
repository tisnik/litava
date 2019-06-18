CC=gcc
CFLAGS=-Wall -ansi -pedantic -Isrc -O9 -ffast-math
LFLAGS=-lm

SRCDIR=src
OBJDIR=obj
EXENAME=litava

all:	$(EXENAME)

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(EXENAME)
	rm -f result*.bmp

run:	$(EXENAME)
	./$(EXENAME)


TEST_NUMBERS = 1 2 3 4 6 7 8 9

.PHONY: test
test:	$(EXENAME)
	$(foreach var,$(TEST_NUMBERS),./$(EXENAME) test/test.bmp test/pattern$(var).bmp result$(var).bmp;)


TEST_NUMBERS_2 = 1 2 3

test2:	$(EXENAME)
	$(foreach var,$(TEST_NUMBERS_2),./$(EXENAME) test/test2.bmp test/pattern2_$(var).bmp result2_$(var).bmp;)


OBJFILES=$(OBJDIR)/litava.o $(OBJDIR)/pixmap.o $(OBJDIR)/bmp_reader.o $(OBJDIR)/bmp_writer.o $(OBJDIR)/ppm_writer.o $(OBJDIR)/hercule.o


$(EXENAME):	$(OBJFILES)
	$(CC) -o $@ $^ $(LFLAGS)

$(OBJDIR)/litava.o:	$(SRCDIR)/litava.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/pixmap.o:	$(SRCDIR)/pixmap.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/bmp_reader.o:	$(SRCDIR)/bmp_reader.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/bmp_writer.o:	$(SRCDIR)/bmp_writer.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/ppm_writer.o:	$(SRCDIR)/ppm_writer.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/hercule.o:	$(SRCDIR)/hercule.c
	$(CC) $(CFLAGS) -c $< -o $@
