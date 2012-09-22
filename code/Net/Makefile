OBJDIR=$(CODE_ROOT)/objdir
INCLUDE_DIR=$(CODE_ROOT)/include

CFLAGS_LOCAL = $(CFLAGS) -c

Net.o: Net.cxx Net.h $(INCLUDE_DIR)/Pin.h
	g++ $(CFLAGS_LOCAL) Net.cxx -I $(INCLUDE_DIR) -o $(OBJDIR)/Net.o
