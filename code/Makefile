MAKE=make
#CODE_ROOT=/root/thesis/Ideas/PlacementImprovement/code
OBJDIR=$(CODE_ROOT)/objdir
INCLUDE_DIR=$(CODE_ROOT)/include

ifeq ($(MAKECMDGOALS), debug)
	CFLAGS= -g 
else
	CFLAGS= -O3 -pg
endif

export CFLAGS

LFLAGS= 

OFILE=the_exec

CC=g++

TARGETS = all debug

SRCDIRS:= Pin Cell Net Design HyperGraph common Stat PhysData

OBJFILES:= $(OBJDIR)/*.o
OBJFILES:= $(addprefix $(OBJDIR)/, $(OBJFILES))

all: COMPILE_ALL GEN_EXEC

COMPILE_ALL: $(SRCDIRS)
	echo $(OBJDIR)
	for dir in $(SRCDIRS); do \
		make -C $$dir; \
	done	

GEN_EXEC: main.cxx
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJDIR)/*.o main.cxx -o the_exec -I $(INCLUDE_DIR) 

debug: COMPILE_ALL GEN_EXEC
