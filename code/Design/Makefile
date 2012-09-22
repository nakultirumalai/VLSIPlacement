OBJDIR=$(CODE_ROOT)/objdir
INCLUDE_DIR=$(CODE_ROOT)/include

CFLAGS_LOCAL = $(CFLAGS) -c

Design.o: Design.cxx Design.h $(INCLUDE_DIR)/Cell.h
	g++ $(CFLAGS_LOCAL) Design.cxx -I $(INCLUDE_DIR) -o $(OBJDIR)/Design.o
