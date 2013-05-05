MAKE=make
#CODE_ROOT=/root/thesis/Ideas/PlacementImprovement/code
MAKEFLAGS += -j1

OBJDIR=$(CODE_ROOT)/objdir
INCLUDE_DIR=$(CODE_ROOT)/include
LIBS= -L/usr/local/lib -lc -ldl -lm -lmosek64 -pthread

SRCDIRS:= Pin Cell Net Design HyperGraph common Stat PhysData Flags AnalyticalSolve Legalize

OBJFILES:= $(SRCDIRS)
DEBUG = 

ifeq ($(MAKECMDGOALS), debug)
	OBJFILES:= $(SRCDIRS:=-g)
	DEBUG = debug
	CFLAGS= -g 
else
	CFLAGS= -pg
endif
OBJFILES:= $(OBJFILES:=.o)
OBJFILES:= $(addprefix $(OBJDIR)/, $(OBJFILES))

export CFLAGS

LFLAGS= 

ifeq ($(MAKECMDGOALS), debug)
	OFILE=the_exec-g
else
	OFILE=the_exec
endif


CC=g++

TARGETS = all 

all: COMPILE_ALL GEN_EXEC

COMPILE_ALL: $(SRCDIRS)
	echo $(OBJDIR)
	@+for dir in $(SRCDIRS); do \
		make $(DEBUG) -C $$dir; \
	done	

GEN_EXEC: main.cxx $(OBJFILES) 
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJFILES) main.cxx variables.cxx -o $(OFILE) -I $(INCLUDE_DIR) $(LIBS)

tag: 	
	ctags . -R $(CODE_ROOT)
	sed 's/\/home\/nakul\/thesis\/VLSIPlacement\/code/\$$CODE_ROOT/g' tags > _tags
	sed 's/\/home\/mukherts\/thesis\/VLSIPlacement\/code/\$$CODE_ROOT/g' _tags > __tags
	mv __tags tags
	rm -rf _tags

debug: all

clean:
	rm -rf $(OBJDIR)/*.o
	rm -rf $(CODE_ROOT)/the_exec
	rm -rf $(CODE_ROOT)/the_exec-g
