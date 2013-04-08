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
$synPath = $ENV{"SYNTHESIS_PATH"};
$iccPath = $ENV{"ICC_PATH"};

# Path of the technology librarries used to perform 
# synthesis
$libraryPath = $ENV{"LIB_PATH"};
$LEFfile = $ENV{"LEF_FILE"};

# input the design name with the script
# Options are as follows:
# c16, m1_core, avr_core, usb_sie
$designName = $ARGV[0];
$ENV{"DESIGN_NAME"} = $designName;

my $stepCount = 1;
my $stepResult;
####################################################################################
####################################################################################
# STEP 1: Delete stale data to generate new files
####################################################################################
####################################################################################
print "STEP $stepCount: Deleting the old benchmark bookshelf files\n"; $stepCount++;
(system("rm -rf $benchmarkGenRoot/results/$designName/bookshelf/*") == 0) || die ("STEP ($stepCount - 1) failed\n");


####################################################################################
####################################################################################
# STEP 2: Run design compiler in the current directory
####################################################################################
####################################################################################
print "STEP $stepCount: Run DC to generate files\n"; $stepCount++;
#(system("$synPath/dc_shell -64bit -f $scriptRoot/genericRTLCompile_dc.tcl > dc_syn_log") == 0) ||     die ("Synthesis using Design Compiler failed\n");
(system("$synPath/dc_shell -f $scriptRoot/readCompiledRTLDC.tcl > dc_syn_log") == 0) || die ("Synthesis using Design Compiler failed\n");


####################################################################################
####################################################################################
# STEP 3: Convert the generated .nodes .nets and .pl file into bookshelf format 
#         compliant files. Make copies of the original
####################################################################################
####################################################################################
print "STEP $stepCount: Make copies of the original files. These are golden\n"; $stepCount++;
(system("cp $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nodes $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nodes.orig") == 0) || die("Could not make copy of nodes files");
(system("cp $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nets $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nets.orig") == 0) || die("Could not make copy of nets files");
(system("cp $benchmarkGenRoot/results/$designName/bookshelf/${designName}.pl $benchmarkGenRoot/results/$designName/bookshelf/${designName}.pl.orig") == 0) || die("Could not make copy of pl files");


####################################################################################
####################################################################################
# STEP 4: Run the ICC script to generate the floorplan and the terminal positions
#         for the chip
####################################################################################
####################################################################################
print "STEP $stepCount: Run IC compiler to generate the DEF file and the terminal positions for top level ports\n"; $stepCount++;
(system("$iccPath/icc_shell -f $scriptRoot/generateFloorplanICC.tcl > icc_floor_log") == 0) ||    die ("Floorplan generation using IC compiler failed\n");
#(system("cp $benchmarkGenRoot/results/$designName/${designName}.def $benchmarkGenRoot/results/$designName/bookshelf/${designName}.def") == 0) || 


####################################################################################
####################################################################################
# STEP  : Get the cell dimensions for the cells and relative positions of pins
#         from the LEF file. Substitution occurs in the nodes file and the nets
#         file
####################################################################################
####################################################################################
print "STEP $stepCount: Get the cell dimensions and relative pin positions from LEF\n"; $stepCount++;
(system("$scriptRoot/getCellDimAndPinPos.pl $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nodes $LEFfile") == 0) ||
    die ("Obtaining cell dimensions and relative pin positions from LEF failed \n");


####################################################################################
####################################################################################
# STEP  : Get the terminal top level port positions from the floorplan
#         and substitute it in the .pl file. These terminals are fixed objects.
####################################################################################
####################################################################################
print "STEP $stepCount: Write terminal pin positions into the PL file\n"; $stepCount++;
(system("$scriptRoot/getTermPositions.pl $benchmarkGenRoot/results/$designName/bookshelf/${designName}.pl $benchmarkGenRoot/results/$designName/bookshelf/${designName}.term.pl") == 0) || die ("STEP ($stepCount - 1) failed\n");


####################################################################################
####################################################################################
# STEP  : From the timing report, extract all the paths which have been dumped 
#         by the timing analyzer and create the .timing_paths file 
####################################################################################
####################################################################################
print "STEP $stepCount: Generate the .timing_paths file from timing report\n"; $stepCount++;
(system("$scriptRoot/getTimingRpt.pl $benchmarkGenRoot/results/$designName/${designName}_timing.rpt $benchmarkGenRoot/results/$designName/bookshelf/${designName}.timing_paths") == 0) ||
    die ("Generation of the timing report failed\n");


####################################################################################
####################################################################################
# STEP  : Extract delays of all cells in the design from liberty (.lib)
#         file and examine the corresponding rise and fall times From the timing 
#         report
####################################################################################
####################################################################################
print "STEP $stepCount: Generate the delays from liberty\n"; $stepCount++;
(system("$scriptRoot/getCellDelays.pl $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nodes.orig $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nodes.delays") == 0) || print("STEP FAILED!: Will be obsoleted soon.\n");


####################################################################################
####################################################################################
# STEP  : Generate pseudonames for the cells which are simply an  alphabet suffixed
#         with a number. For e.g. o1, o2, o3... and generate a map file which has 
#         the following details
#         PSEUDO NAME     ORIGINAL NAME     LIB CELL NAME     FF/nFF
####################################################################################
####################################################################################
print "STEP $stepCount: Generate the pseudo names for cells\n"; $stepCount++;
(system("$scriptRoot/map2Pseudo.pl $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nodes.orig $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nodes.map") == 0) || 
    die ("Generate pseudo names for cells failed\n");


####################################################################################
####################################################################################
# STEP  : Generate pseudonames for the nets which are simply an  alphabet suffixed
#         with a number. For e.g. o1, o2, o3... and generate a map file which has 
#         the pseudo name and the original name
#         PSEUDO NAME     ORIGINAL NAME     
####################################################################################
####################################################################################
print "STEP $stepCount: Generate the pseudo names for nets\n"; $stepCount++;
(system("$scriptRoot/mapNets2Pseudo.pl $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nets $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nets.map") == 0) ||
    die ("Generate pseudo names for nets failed\n");    


####################################################################################
####################################################################################
# STEP  : Replace the original names of cells with pseudo names in the .nodes file,
#         the .nets file, .pl file and the .timing_paths file
####################################################################################
####################################################################################
# MAKE COPIES OF ALL FILES WHICH HAVE ORIGINAL NAMES 
print "STEP $stepCount: Substitute CELL pseudo names in the nodes file\n"; $stepCount++;
(system("cp $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nodes $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nodes.orig.names") == 0) || die ("Cannot make copy of nodes file");
(system("$scriptRoot/subFromMaps.pl $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nodes $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nodes.map") == 0) || die ("Replace pseudo names in .nodes file\n");

print "STEP $stepCount: Substitute CELL pseudo names in the nets file\n"; $stepCount++;
(system("cp $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nets $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nets.orig.names") == 0) || die ("Cannot make copy of nets file");
(system("$scriptRoot/subFromMaps.pl $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nets $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nodes.map") == 0) ||  die ("Replace pseudo names in .nets file failed\n");

print "STEP $stepCount: Substitute CELL pseudo names in the timing paths file\n"; $stepCount++;
(system("cp $benchmarkGenRoot/results/$designName/bookshelf/${designName}.timing_paths $benchmarkGenRoot/results/$designName/bookshelf/${designName}.timing_paths.orig.names") == 0) || die ("Cannot make copy of timing_paths file");
(system("$scriptRoot/subFromMaps.pl $benchmarkGenRoot/results/$designName/bookshelf/${designName}.timing_paths $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nodes.map") == 0) || die ("Replace pseudo names in .timing_paths file failed\n");

print "STEP $stepCount: Substitute CELL pseudo names in the pl file\n"; $stepCount++;
(system("cp $benchmarkGenRoot/results/$designName/bookshelf/${designName}.pl $benchmarkGenRoot/results/$designName/bookshelf/${designName}.pl.orig.names") == 0) || die ("Cannot make copy of the pl file");
(system("$scriptRoot/subFromMaps.pl $benchmarkGenRoot/results/$designName/bookshelf/${designName}.pl $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nodes.map") == 0) || die ("STEP ($stepCount - 1) failed\n");

print "STEP $stepCount: Substitute NET pseudo names in the nets file\n"; $stepCount++;
(system("$scriptRoot/subNetsFromMaps.pl $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nets $benchmarkGenRoot/results/$designName/bookshelf/${designName}.nets.map") == 0) || die ("Replace pseudo net names in .nets file failed\n");    


####################################################################################
####################################################################################
# STEP  : Generate the .scl file describing rows in the bookshelf format
####################################################################################
####################################################################################
my $minWidth = 152; # min-Width of cells in the LEF file
my $height = 1672; # Height of the rows = height of standard cells
my $symmetry = "Y X";

print "STEP $stepCount: Generate the SCL file for bookshelf format\n"; $stepCount++;
(system("$scriptRoot/getSclFromDef.pl $benchmarkGenRoot/results/$designName/bookshelf/${designName}.def $minWidth $height $symmetry") == 0) ||
    die ("STEP ($stepCount - 1) failed\n");


####################################################################################
####################################################################################
# STEP  : Create an empty weights file 
####################################################################################
####################################################################################
open(wtsFile, ">$benchmarkGenRoot/results/$designName/bookshelf/${designName}.wts") || die ("Cannot write to ${designName}.wts");
print wtsFile "UCLA wts 1.0\n";
print wtsFile "# Created        :\n";
print wtsFile "# User           :\n";
close(wtsFile);


####################################################################################
####################################################################################
# STEP  : Create an aux file for the placer
####################################################################################
####################################################################################
open(auxFile, ">$benchmarkGenRoot/results/$designName/bookshelf/${designName}.aux") || die ("Cannot write to ${designName}.aux");
print auxFile "RowBasedPlacement : ${designName}.nodes ${designName}.nets ${designName}.wts ${designName}.pl ${designName}.scl\n";
close(auxFile);


####################################################################################
####################################################################################
# STEP  : Create a file to indicate that the bookshelf format generation 
#         completed successfully
####################################################################################
####################################################################################
open (doneFile, ">$benchmarkGenRoot/results/$designName/bookshelf/${designName}.done") || die ("Cannot write to ${designName}.done");
print doneFile "All Done!\n";
close(doneFile);
