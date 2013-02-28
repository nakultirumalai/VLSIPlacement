#!/usr/bin/perl

# Objective:
# To generate a benchmark in the bookshelf format starting from the RTL
# The following files need to be generated:
# <des>.nodes: Nodes file, defining the terminal nodes, regular nodes, their dimensions
# <des>.nets: Nets file, defining the net names, net degrees and their connectivity to cell pins along with pin locations
# <des>.scl: Floorplan file which defines row locations 
# <des>.pl: File which indicates which of the cells have their placement fixed. Mostly for macros
# Additional files:
# <des>.map: Has information about pseudo cell names, actual cell names, lib cell names and identification of FF 
# <des>.delay: Has information about delays of library cell arcs that have been used by DC 
# <des>.paths: Has information about slacks of various paths. Cellname/pinname cellname/pinname... slack_value

# Path of the benchmark generator parent folder must be 
# set in the environment
$benchmarkGenRoot = $ENV{"BENCHMARK_GEN_ROOT"};
$scriptRoot = $ENV{"SCRIPT_ROOT"};

# Path of the technology librarries used to perform 
# synthesis
$libraryPath = $ENV{"LIB_PATH"};
$LEFfile = $ENV{"LEF_FILE"};

# input the design name with the script
# Options are as follows:
# c16, m1_core, avr_core, usb_sie
$designName = $ARGV[0];
$ENV{"DESIGN_NAME"} = $designName;

# STEP 1:
# Change directory to $benchmarkGenRoot/work
system("cd $benchmarkGenRoot");

# STEP 2:
# Run design compiler in the work directory
system("dc_shell -f $benchmarkGenRoot/scripts/genericRTLCompile_dc.tcl | tee dc_log");
system("cd ..");

# STEP 3:
# Convert the generated nodes file into a bookshelf 
# format compliant file using the LEF file provided with 
# the library kit
system("$scriptRoot/getCellDimensions.pl $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nodes $LEFfile");

# STEP 4:
# From the timing report, extract all the paths which have been 
# dumped by the timing analyzer and create the paths file
system("$scriptRoot/getTimingRpt.pl $benchmarkGenRoot/results/$designName/${designName}_timing.rpt > $benchmarkGenRoot/results/$designName/bookshelf/${designName}.timing_paths");

# STEP 5:
# Extract delays of all cells in the design by using the .lib
# file and examining the corresponding rise and fall times

# STEP 6:
# Generate pseudonames for the cells which are simply an 
# alphabet suffixed with a number. For e.g. o1, o2, o3...
# and generate a map file which has the following details
# PSEUDO NAME     ORIGINAL NAME     LIB CELL NAME     FF/nFF

# STEP 7:
# Replace the original names for cells in the .nodes file,
# and the .nets file 

# STEP 8:
# Replace pin names in the .nodes file with coordinates 
# after having examined the LEF file

# STEP 9:
# Generate a floorplan using any open source floor planning
# tool for row-based and non-rowbased placement

# STEP 10:
# Create an empty net weights file

# STEP 11:
# Create an aux file 
