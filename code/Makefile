MAKE=make
#CODE_ROOT=/root/thesis/Ideas/PlacementImprovement/code
OBJDIR=$(CODE_ROOT)/objdir
INCLUDE_DIR=$(CODE_ROOT)/include

ifeq ($(MAKECMDGOALS), debug)
	CFLAGS= -g 
else
	CFLAGS= -O3
endif

export CFLAGS

LFLAGS= 

ifeq ($(MAKECMDGOALS), debug)
	OFILE=the_exec-g
else
	OFILE=the_exec
endif


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
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJDIR)/*.o main.cxx -o $(OFILE) -I $(INCLUDE_DIR) 

debug: COMPILE_ALL GEN_EXEC
