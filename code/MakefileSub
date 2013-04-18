OBJDIR=$(CODE_ROOT)/objdir
INCLUDE_DIR=$(CODE_ROOT)/include

CFLAGS_LOCAL = $(CFLAGS) -c

include src.mk

HFILES:=$(HFILES:=.h)
HFILES:= $(addprefix $(INCLUDE_DIR)/, $(HFILES))

OBJFILES:= $(SRCFILES:=.o)
OBJGFILES:= $(SRCFILES:=-g.o)
OBJS := $(addprefix $(OBJDIR)/,$(OBJFILES))
OBJSG := $(addprefix $(OBJDIR)/,$(OBJGFILES))
SRCCPPFILES:= $(SRCFILES:=.cxx)

$(OBJDIR)/%.o : %.cxx $(HFILES)
	g++ $(CFLAGS_LOCAL) $*.cxx -I $(INCLUDE_DIR) -o $(OBJDIR)/$*.o

$(OBJDIR)/%-g.o : %.cxx $(HFILES)
	g++ -g $(CFLAGS_LOCAL) $*.cxx -I $(INCLUDE_DIR) -o $(OBJDIR)/$*-g.o

all:  $(OBJS)
	ld -r $(OBJS) -o $(OBJDIR)/$(MODULE).o

debug:  $(OBJSG)
	ld -r $(OBJSG) -o $(OBJDIR)/$(MODULE)-g.o
