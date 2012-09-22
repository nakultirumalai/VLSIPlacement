OBJDIR=$(CODE_ROOT)/objdir
INCLUDE_DIR=$(CODE_ROOT)/include

CFLAGS_LOCAL = $(CFLAGS) -c

common.o: common_utils.cxx common.h
	g++ $(CFLAGS_LOCAL) common_utils.cxx -I $(INCLUDE_DIR) -o $(OBJDIR)/common.o
