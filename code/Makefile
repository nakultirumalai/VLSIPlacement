MAKE=make
#CODE_ROOT=/root/thesis/Ideas/PlacementImprovement/code
MAKEFLAGS += -j1
OBJDIR=$(CODE_ROOT)/objdir
INCLUDE_DIR=$(CODE_ROOT)/include

#LIBSOOQP= -lblas /usr/local/lib/libma27.a -lgfortran -lprofiler
LIBSOOQP= -lblas -lgfortran -lprofiler

LIBQUADPROG= #-lQuadProgpp
LIBMOSEK= -lmosek64 -pthread

#LIBS= ./libs/text.a -L/usr/local/lib -lc -ldl -lm $(LIBMOSEK) $(LIBQUADPROG) $(LIBSOOQP)
LIBS= -L/usr/local/lib -lc -ldl -lm $(LIBMOSEK) $(LIBQUADPROG) $(LIBSOOQP)

SRCDIRS:= Pin Cell Net Design HyperGraph common Stat PhysData \
	Flags Legalize TimingModels Bin \
	Plot ConjGrad Env Path lists FDP
# Removed modules 
# CellSpread AnalyticalSolve		

OBJFILES:= $(SRCDIRS)
DEBUG = 

ifeq ($(MAKECMDGOALS), debug)
	OBJFILES:= $(SRCDIRS:=-g)
	DEBUG = debug
else
	CFLAGS= -O3 -Wuninitialized #-pg
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

all-g: COMPILE_ALL GEN_EXEC-g

COMPILE_ALL: $(SRCDIRS)
	echo $(OBJDIR)
	@+for dir in $(SRCDIRS); do \
		make $(DEBUG) -C $$dir; \
	done	

GEN_EXEC: main.cxx $(OBJFILES) 
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJFILES) main.cxx variables.cxx -o $(OFILE) -I $(INCLUDE_DIR) $(LIBS)

GEN_EXEC-g: main.cxx $(OBJFILES) 
	$(CC) $(CFLAGS) -g $(LFLAGS) $(OBJFILES) main.cxx variables.cxx -o $(OFILE) -I $(INCLUDE_DIR) $(LIBS)

tag: 	
	ctags . -R $(CODE_ROOT)
	sed 's/\/home\/nakul\/thesis\/VLSIPlacement\/code/\$$CODE_ROOT/g' tags > _tags
	sed 's/\/home\/mukherts\/thesis\/VLSIPlacement\/code/\$$CODE_ROOT/g' _tags > __tags
	mv __tags tags
	rm -rf _tags

debug: all-g

clean:
	rm -rf $(OBJDIR)/*.o
	rm -rf $(CODE_ROOT)/the_exec
	rm -rf $(CODE_ROOT)/the_exec-g
