OBJDIR=$(CODE_ROOT)/objdir
INCLUDE_DIR=$(CODE_ROOT)/include

CFLAGS_LOCAL = $(CFLAGS) -c

SRCFILES:=  DesignMain DesignUtils DesignRead DesignGraph DesignAnalysis DesignCluster

HFILES:= Design.h Cell.h
HFILES:= $(addprefix $(INCLUDE_DIR)/, $(HFILES))

SRCCPPFILES:= $(SRCFILES:=.cxx)

MAKE_DESIGN: $(SRCCPPFILES) $(HFILES)
	for file in $(SRCFILES); do \
		g++ $(CFLAGS_LOCAL) $$file.cxx -I $(INCLUDE_DIR) -o $(OBJDIR)/$$file.o; \
	done

