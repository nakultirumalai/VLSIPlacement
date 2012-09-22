OBJDIR=$(CODE_ROOT)/objdir
INCLUDE_DIR=$(CODE_ROOT)/include

CFLAGS_LOCAL= $(CFLAGS) -c

Pin.o: Pin.cxx Pin.h 
	echo $(CFLAGS_LOCAL)
	g++ $(CFLAGS_LOCAL) Pin.cxx -I $(INCLUDE_DIR) -o $(OBJDIR)/Pin.o
