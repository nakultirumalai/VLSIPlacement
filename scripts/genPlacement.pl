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
my $benchmarkGenRoot = $ENV{"BENCHMARK_GEN_ROOT"};
my $scriptRoot = $ENV{"SCRIPT_ROOT"};
my $synPath = $ENV{"SYNTHESIS_PATH"};
my $iccPath = $ENV{"ICC_PATH"};
my $ptPath = $ENV{"PT_PATH"};
my $plPath = $ENV{"PLACER_PATH"};
my $plName = $ENV{"PLACER_NAME"};

# Path of the technology librarries used to perform 
# synthesis
my $libraryPath = $ENV{"LIB_PATH"};
my $LEFfile = $ENV{"LEF_FILE"};

# input the design name with the script
# Options are as follows:
# c16, m1_core, avr_core, usb_sie
my $designName = $ARGV[0];
$ENV{"DESIGN_NAME"} = $designName;

my $stepCount = 1;
my $plFile = "";


####################################################################################
####################################################################################
# STEP 1: Check if the benchmark has been generated successfully
####################################################################################
####################################################################################
print "STEP $stepCount: Check if the benchmark has been generated successfully\n"; $stepCount++;
open(doneFile, "$benchmarkGenRoot/results/$designName/bookshelf/$designName.done") || 
    die ("Benchmark generation incomplete");
close(doneFile);


####################################################################################
####################################################################################
# STEP 2: Run one of the placers to generate a placement:
#         Dragon 
#         mPL6
#         NTUPlace
#         FlowPlace
#         Capo
####################################################################################
####################################################################################
print "STEP $stepCount: Running placer \"$plName\" to generate output placement\n"; $stepCount++;
if ($plName eq "Dragon") {
    (system("$scriptRoot/run_placer.sh Dragon $benchmarkGenRoot/results/$designName/bookshelf $designName.aux") == 0) ||
	die ("Cannot run placer \"$plName\"\n");
    $plFile = "${designName}_Dragon.pl";
} elsif ($plName eq "mPL6") {
    (system("$plPath/mPL6 -d $benchmarkGenRoot/results/$designName/bookshelf/$designName.aux") == 0) ||
	die ("Cannot run placer \"$plName\"\n");
} else {
    print "Done\n";
}


####################################################################################
####################################################################################
# STEP 3: Clip components and nets from the original def to produce the bare def
####################################################################################
####################################################################################
print "STEP $stepCount: Removing sections COMPONENTS & NETS from the DEF file\n"; $stepCount++;
(system("mv $benchmarkGenRoot/results/$designName/bookshelf/$designName.def $benchmarkGenRoot/results/$designName/bookshelf/$designName.def.copy") == 0) ||
    die ("Cannot make copy of the DEF file\n");
open(DEFFileCopy, "$benchmarkGenRoot/results/$designName/bookshelf/$designName.def.copy") ||
    die ("Cannot open copied DEF file\n");
open(DEFFile, ">$benchmarkGenRoot/results/$designName/bookshelf/$designName.def") ||
    die ("Cannot open DEF file for writing");
my $NETS_begin = 0; 
while (my $line = <DEFFileCopy>) {
    if ($line =~ m/^\s*NETS/) {
	$NETS_begin = 1;
	next;
    } elsif ($line =~ m/^\s*END NETS/) {
	$NETS_begin = 0;
	next;
    } 
    if ($NETS_begin == 0) {
	print DEFFile $line;
    }
}

close(DEFFile);
close(DEFFileCopy);
(system("rm -rf $benchmarkGenRoot/results/$designName/bookshelf/$designName.def.copy") == 0) || die ("Cannot remove copied DEF file");


####################################################################################
####################################################################################
# STEP 4: Execute script to replace the pseudo names in the .pl output of the 
#         placer with original names of the cells  
####################################################################################
####################################################################################
print "STEP $stepCount: Replace pseudo names in the PL file with original names\n"; $stepCount++;
(system("$scriptRoot/subPseudoFromMaps.pl $benchmarkGenRoot/results/$designName/bookshelf/$plFile $benchmarkGenRoot/results/$designName/bookshelf/$designName.nodes.map") == 0) || die ("Cannot replace pseudo names in PL file");


####################################################################################
####################################################################################
# STEP 5: Execute script to replace the original DEF with the placed DEF
####################################################################################
####################################################################################
print "STEP $stepCount: Generating placed DEF \n"; $stepCount++;
(system("$scriptRoot/convertPlToDef.pl $benchmarkGenRoot/results/$designName/bookshelf/$designName.def $benchmarkGenRoot/results/$designName/bookshelf/$plFile $benchmarkGenRoot/results/$designName/bookshelf/${designName}.placed.def") == 0) || die ("Cannot generate DEF for placed netlist");


####################################################################################
####################################################################################
# STEP 6: Run ICC to generate a routed netlist
####################################################################################
####################################################################################
print "STEP $stepCount: Run IC compiler to generate the routed netlist and extract parasitics\n"; $stepCount++;
(system("$iccPath/icc_shell -f $benchmarkGenRoot/scripts/genRouting.tcl | tee  icc_route_log") == 0) ||
    die ("STEP ($stepCount - 1) failed\n");


####################################################################################
####################################################################################
# STEP 7: Read the parasitics file in prime time and generate the timing report
####################################################################################
####################################################################################
print "STEP $stepCount: Running prime time to generate timing report using parasitics\n"; $stepCount++;
(system("$ptPath/pt_shell -f $benchmarkGenRoot/scripts/ptAnalysis.tcl | tee pt_log") == 0) || die ("Cannot perform post route timing analysis");


####################################################################################
####################################################################################
# STEP 8: Generate the paths file for the benchmark from the post routed timing 
#         report 
####################################################################################
####################################################################################
(system("$scriptRoot/getTimingRpt.pl $benchmarkGenRoot/results/$designName/${designName}_post_route_timing.rpt > $benchmarkGenRoot/results/$designName/bookshelf/${designName}.route.timing_paths") == 0) || 
    die ("Cannot generate the routed timing paths file");
