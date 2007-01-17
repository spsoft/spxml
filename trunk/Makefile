
#--------------------------------------------------------------------

CC = gcc
AR = ar cru
CFLAGS = -Wall -D_REENTRANT -D_GNU_SOURCE -g
SOFLAGS = -shared
LDFLAGS =

LINKER = $(CC)
LINT = lint -c
RM = /bin/rm -f

#--------------------------------------------------------------------

LIBOBJS = spxmlutils.o spxmlevent.o spxmlreader.o spxmlparser.o spxmlstag.o \
		spxmlnode.o spdomparser.o spdomiterator.o

TARGET =  libspxml.so \
		testpull testdom

#--------------------------------------------------------------------

all: $(TARGET)

libspxml.so: $(LIBOBJS)
	$(LINKER) $(SOFLAGS) $^ -o $@

testpull: testpull.o
	$(LINKER) $(LDFLAGS) $^ -L. -lspxml -o $@

testdom: testdom.o
	$(LINKER) $(LDFLAGS) $^ -L. -lspxml -o $@

clean:
	$(RM) *.o vgcore.* core core.* $(TARGET)

#--------------------------------------------------------------------

# make rule
%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@	

%.o : %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@	

