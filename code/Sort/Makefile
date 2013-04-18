OBJDIR=$(CODE_ROOT)/objdir
INCLUDE_DIR=$(CODE_ROOT)/include

CFLAGS_LOCAL = $(CFLAGS) -c

MODULE = Sort
SRCFILES = SortMain
HFILES := Sort

HFILES:=$(HFILES:=.h)
HFILES:= $(addprefix $(INCLUDE_DIR)/, $(HFILES))

OBJFILES:= $(SRCFILES:=.o)
OBJS := $(addprefix $(OBJDIR)/,$(OBJFILES))
SRCCPPFILES:= $(SRCFILES:=.cxx)

$(OBJDIR)/%.o : %.cxx $(HFILES)
	g++ $(CFLAGS_LOCAL) $*.cxx -I $(INCLUDE_DIR) -o $(OBJDIR)/$*.o

all:  $(OBJS)
	ld -r $(OBJS) -o $(OBJDIR)/$(MODULE).o
