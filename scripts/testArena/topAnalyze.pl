#!/usr/bin/perl -w

require 'aliases.pl';
use warnings;

my $topInputFile = $ENV{INPUT_TAB};
my $scriptRoot = $ENV{SCRIPT_ROOT};
my $setSelect;
my $setBegin;
my $setEnd;
my @allOptions = ();
my @thisRun = ();
my $plFileName;
my $logFileName;
my @plFiles;
my $maxareaCount;
my $maxcellCount;
my %maxArea;
my %maxCells;
my $forceRun = 0;
my %forceHash;
my $runControl;
my @allCommands;
my %legend;
my $outputPath = $ENV{OUTPUT_PATH};
my $execPath = $ENV{EXEC_FILE};
my $designRoot = $ENV{BENCHMARK_ROOT};
my $benchmarkRoot = $designRoot;
my @designs = split(/\s+/,$ENV{DESIGNS});

$plFileName = "";
$setSelect = 0;
$setBegin = 0;
$setEnd = 1;
$maxareaCount = 1;
$maxcellCount = 1;


#########################################################################################
##### STEP 1.READ THE COMMAND OPTIONS FILE AND PARSE IT TO FORM THE EXECUTABLE STRING ###
#########################################################################################

open (inFile, $topInputFile) || die ("Cannot open $topInputFile for reading\n");
while(my $line = <inFile>) {
    chomp($line);
    # Ignore comment lines or blank lines
    next if (($line =~ /^\#/) || ($line eq ""));
    # Lines beginning with a '-' represent parameters to the placer
    if (($line =~ /^-(.*)/) && ($setSelect)) {
	# Assuming parameters to be of the form -<param> <value>
	my @elems = split(/\t+/, $line);
	
	# These represent special cases to taken care of while 
	# generating uniq file names for the final result
	if ($elems[0] eq "-clust_max_area") {
	    if (exists $optionMap{$elems[0]}{"MA"}) {
		# Appends a unique number to the command alias 
		my $extension = "$optionMap{$elems[0]}{\"MA\"}"."$maxareaCount";
		$legend{$extension} = "$elems[0]"."  $elems[1]";
		$extension .= "_";
		$plFileName .= "$extension";
		$maxareaCount++;
	    }
	    next;
	}
	
	if ($elems[0] eq "-clust_max_cells") {
	    if (exists $optionMap{$elems[0]}{"MC"}) {
		my $tempStr = $optionMap{$elems[0]}{"MC"};
		# Appends a unique number to the command alias 
		my $extension = "$tempStr"."$maxcellCount";
		$legend{$extension} = "$elems[0]"."  $elems[1]";
		$extension .= "_";
		$plFileName .= "$extension";
		$maxcellCount++;
	    }
	    next;
	}
	# Presence of '-force' in the input file denotes the
	# tool has to be run irrespective of whether placement 
	# results exists or not
	if ($elems[0] eq "-force") {
	    $forceRun = 1;
	    next;
	}
	
	# Creating an array for each <param>, <value> pair
	push @thisRun, $elems[0];
	push @thisRun, $elems[1];

	# For all other cases
	if (exists $optionMap{$elems[0]}{$elems[1]}) {
	    my $alias = $optionMap{$elems[0]}{$elems[1]};
	    $legend{$alias} = "$elems[0]"."  $elems[1]";
	    $plFileName .= $optionMap{$elems[0]}{$elems[1]};
	    $plFileName .= "_";
	    next;
	}

    } elsif ($line =~ /^\$(.*)/) {
	# A line beginning with '$' is used to decide whether the 
	# block following that has to executed for this run of
	# the tool or not. [1-Execute] [0-Don't Execute]

	my @select = split(/\t+/, $line);
	$setSelect = $select[1];
	next;
    } elsif (($line =~ /^\*BEGIN/) && ($setSelect)) {
	# Indicates that a command block has begun and select flag
	# has been set. So, this block has to be executed

	if (!$setEnd) {
	    print "ERROR: BEGIN before preceding block END!!\n";
	    print "Exiting.... \n";
	    exit(0);
	} else {
	    $setBegin = 1;
	    $setEnd = 0;
	    next;
	}
    } elsif (($line =~ /^\*END/) && ($setSelect)) {
	# Indicates the end of the command block and 
	# since the select flag is set, proceeds with the 
	# creation of the executable string
	
	if (!$setBegin) {
	    print "ERROR: END found without preceding block BEGIN!!\n";
	    print "Exiting.... \n";
	    exit(0);
	} else {
	    $setBegin = 0;
	    $setEnd = 1;
	    $setSelect = 0;
	    push @allOptions, [ @thisRun ];
	    chop($plFileName);
	    push @plFiles, $plFileName;
	    if ($forceRun eq "1") {
		$forceHash{$plFileName} = $forceRun;
	    }
	    $forceRun = 0;
	    $plFileName = "";
	    @thisRun = ();
	    next;
	}
    }
}
close(inFile);
print "....Command options file read successfully\n\n";

# runControl decides whether the tool has to be run for 
# a specific combination of params or not


foreach my $design(@designs) {
    # Creating the entire directory structure if it does
    # not exist
    my $dirPath = "$outputPath"."/$design";
    if (-d "$dirPath") {
	unless (-d "$dirPath/work") {
	    (system("mkdir $dirPath/work") == 0) || die("ERROR: Cannot create work directory\n");
	    print "Work directory created\n";
	}
 	unless (-d "$dirPath/PL_files") {
	    (system("mkdir $dirPath/PL_files") == 0) || die("ERROR: Cannot create PL_files directory\n");
	    print "PL_files directory created\n";
	}
	unless (-d "$dirPath/logs") {
	    (system("mkdir $dirPath/logs") == 0) || die("ERROR: Cannot create logs directory\n");
	    print "logs directory created\n";
	}
    } else {
	print "Top Level DIRECTORY DOES NOT EXIST for design \"$design\". \nCREATING directory tree...\n";
	(system("mkdir $dirPath") == 0) || die("ERROR: Cannot create specified directory\n");
	(system("mkdir $dirPath/work") == 0) || die("ERROR: Cannot create work directory\n");
	(system("mkdir $dirPath/PL_files") == 0) || die("ERROR: Cannot create PL_files directory\n");
	(system("mkdir $dirPath/logs") == 0) || die("ERROR: Cannot create logs directory\n");
    }
    
    # Filenum creates a sync between file name and commands to be executed
    my $fileNum = 0;
    $runControl = 1;
    # Iterating over all the command blocks in the input file
    foreach my $thisFile(@plFiles){
	$plFileName = "$dirPath/PL_files/$design.$thisFile";
	# Extension added to a copy of file name to check if file exists
	my $plFileNameExt = "$plFileName".".pl";
	if (-e "$plFileNameExt") {
	    # Previous results exist
	    # Unsetting run control to stop execution
	    $runControl = 0;
	    if (exists($forceHash{$thisFile})) {
		# requires the tool to run again overwritting earlier results
		# manual override of run control by '-force'
		$runControl = 1;
	    }
	}
	if ($runControl == 1) {
	    # Change current directory to work to run the tool
	    # print "EXECUTABLE: \n";
	    my $execString = "$execPath -des $designRoot/$design/bookshelf/$design".".aux";
	    
	    # Adding the output parameter to the exec string
	    $execString .= " -output $plFileName";
	    
            # Get the remaining exec string from allOptions array
	    my $arrRef = $allOptions[$fileNum] ;
	    my $fullExec = "$execString"." @$arrRef "."> ../logs/$design".".$thisFile".".log";
	    
	    # print "$fullExec\n";
	    my $workDirectory = "$dirPath/work";
	    print "RUNNING placer for \"$design\" ..... \n";

	    # Change current directory to ./work to run the placer 
	    chdir($workDirectory) || die("Cannot chdir to $workDirectory $!");
	    (system("$fullExec") == 0) || die ("Cannot execute system command");
	    print "..... DONE\n";

	    # STEP 3: Clip components and nets from the original def to produce the bare def
	    # Copying the original DEF file into the work directory and 
	    (system("cp $benchmarkRoot/$design/bookshelf/$design.def ./$design.def") == 0) || die ("Cannot make copy of the DEF file\n");
	    (system("mv ./$design.def ./$design.def.copy") == 0) || die ("Cannot make copy of the DEF file\n");
	    open(DEFFileCopy, "./$design.def.copy") ||
		die ("Cannot open copied DEF file\n");
	    open(DEFFile, ">./$design.def") ||
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
	    # (system("rm -rf $benchmarkRoot/$design/bookshelf/$design.def.copy") == 0) || die ("Cannot remove copied DEF file");
	    # Make a copy of the placed pl file
	    (system("cp $plFileNameExt $plFileNameExt.orig") == 0) || die ("Cannot make copy of $plFileNameExt ");
	    
	    # STEP 4: Execute script to replace the pseudo names in the .pl output of the placer with original names of the cells  
	    (system("$scriptRoot/subPseudoFromMaps.pl $plFileNameExt $benchmarkRoot/$design/bookshelf/$design.nodes.map") == 0) || die ("Cannot replace pseudo names in PL file");

	    # STEP 5: Execute script to replace the original DEF with the placed DEF
	    (system("cp $benchmarkGenRoot/results/$designName/bookshelf/$designName.def $benchmarkGenRoot/results/$designName/bookshelf/$plFile.def"));
(system("$scriptRoot/convertPlToDef.pl $benchmarkGenRoot/results/$designName/bookshelf/$plFile.def $benchmarkGenRoot/results/$designName/bookshelf/$plFile $benchmarkGenRoot/results/$designName/bookshelf/$plFile.placed.def") == 0) || die ("Cannot generate DEF for placed netlist");

	    
	    # Print legend to a file
	    my $legendFileName = "$plFileName".".legend";
	    open(legendFile, ">$legendFileName") || die ("Cannot open file for writing");
	    foreach my $key(sort {$a cmp $b} keys %legend) {
		print legendFile "$key ====> $legend{$key}\n";
	    }
	    close(legendFile);

	} elsif ($runControl == 0) {
	    # Indicates files containing earlier results exist and
	    # -force keyword not present in input file
	    print "**** Results EXIST in ./$design/PL_files/ *****\n";
	    print "To RE-RUN placer DELETE existing files or USE \'-force\' in Input file\n";
	}
	
	$runControl = 1;
	$fileNum++;
    }
}	    

print "\n ANALYSIS COMPLETED. EXITING...... \n";
exit(0);
