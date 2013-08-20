#!/usr/bin/perl -w


require "$ENV{SCRIPT_ROOT}/aliases.pl";
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
my @designPLFiles;
my %allPLFiles;
my $maxareaCount;
my $maxcellCount;
my %maxArea;
my %maxCells;
my $forceRun = 0;
my %forceHash;
my $runControl;
my @allCommands;
my %legend;
my $designName;
my %finalResults;
my $foundOption;
my $defaultCount;


my $forceFlag = 0;
my $outputPath = $ENV{OUTPUT_PATH};
my $execPath = $ENV{EXEC_FILE};
my $designRoot = $ENV{BENCHMARK_ROOT};
my $benchmarkRoot = $designRoot;
my $utilsRoot = $ENV{UTILS_ROOT};
my @designs = split(/\s+/,$ENV{DESIGNS});
my $execType = $ENV{EXEC_TYPE};

if ($execType eq "") {
    $execType = "both";
}

$plFileName = "";
$setSelect = 0;
$setBegin = 0;
$setEnd = 1;
$maxareaCount = 1;
$maxcellCount = 1;

my $stepCount = 0;
$foundOption = 0;
$defaultCount = 0;

if ($execType eq "place" || $execType eq "both") {

###########################################################
# STEP 0. READ THE COMMAND OPTIONS FILE AND PARSE 
#         IT TO FORM THE EXECUTABLE STRING 
###########################################################
    print "STEP $stepCount: Reading options for placer from input file\n"; $stepCount++;
    open (inFile, $topInputFile) || die ("Cannot open $topInputFile for reading\n");
    while(my $line = <inFile>) {
	chomp($line);
	# Ignore comment lines or blank lines
	next if (($line =~ /^\#/) || ($line eq ""));
	# Lines beginning with a '-' represent parameters to the placer
	if (($line =~ /^-(.*)/) && ($setSelect)) {
	    # Assuming parameters to be of the form -<param> <value>
	    my @elems = split(/\s+/, $line);
	    
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
		$forceFlag = 1;
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
	    $foundOption = 1;

	} elsif ($line =~ /^\$(.*)/) {
	    # A line beginning with '$SELECT' is used to decide whether the 
	    # block following that has to be executed for this run of
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
		if (!$foundOption) {
		    $plFileName = "DEFAULT_"."$defaultCount"."_";
		    $defaultCount++;
		}
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

    # print "....Command options file read successfully\n\n";


    foreach  $designName(@designs) {
	######################################################
	# STEP 1. CHECK IF THE REQUIRED DIRECTORIES EXIST. 
	#         IF NOT CREATE THEM 
	######################################################

	print "STEP $stepCount: Checking if required directory structure exists in current directory\n"; $stepCount++;
	if (-d "$outputPath/$designName") {
	    unless (-d "$outputPath/$designName/PL_files") {
		(system("mkdir $outputPath/$designName/PL_files") == 0) || die("ERROR: Cannot create PL_files directory\n");
		print "PL_files directory created\n";
	    }
	    unless (-d "$outputPath/$designName/logs") {
		(system("mkdir $outputPath/$designName/logs") == 0) || die("ERROR: Cannot create logs directory\n");
		print "logs directory created\n";
	    }
	    unless (-d "$outputPath/$designName/DEF") {
		(system("mkdir $outputPath/$designName/DEF") == 0) || die("ERROR: Cannot create def directory\n");
		print "DEF directory created\n";
	    }
	    unless (-d "$outputPath/$designName/Timing") {
		(system("mkdir $outputPath/$designName/Timing") == 0) || die("ERROR: Cannot create Timing directory\n");
		print "Timing directory created\n";
	    }
	} else {
	    print "Top Level DIRECTORY DOES NOT EXIST for design \"$designName\". \nCREATING directory tree...\n";
	    (system("mkdir $outputPath/$designName") == 0) || die("ERROR: Cannot create $outputPath/$designName directory\n");
	    
	    (system("mkdir $outputPath/$designName/PL_files") == 0) || die("ERROR: Cannot create $outputPath/$designName/PL_files directory\n");
	    (system("mkdir $outputPath/$designName/logs") == 0) || die("ERROR: Cannot create $outputPath/$designName/logs directory\n");
	    (system("mkdir $outputPath/$designName/DEF") == 0) || die("ERROR: Cannot create $outputPath/$designName/DEF directory\n");
	    (system("mkdir $outputPath/$designName/Timing") == 0) || die("ERROR: Cannot create $outputPath/$designName/Timing directory\n");
	}

	
	#######################################################
	# STEP 2. CHECK IF RESULTS EXIST. IF NOT CREATE 
	#         THE FILENAME ALIAS AND EXECUTABLE STRING 
	#######################################################
	print "STEP $stepCount: Check if previous results exist. Create file alias and executable string\n"; $stepCount++;
	# Filenum creates a sync between file name and commands to be executed
	my $fileNum = 0;
	$runControl = 1;
	
	# Iterating over all the command blocks in the input file
	foreach my $currentFileName(@plFiles){
	    
	    # Extension added to a copy of file name to check if file exists
	    if (-e "$outputPath/$designName/PL_files/${designName}.${currentFileName}.pl") {
		# Previous results exist, Unsetting run control to stop execution
		$runControl = 0;
		if (exists($forceHash{$currentFileName})) {
		    # requires the tool to run again overwritting earlier results
		    # manual override of run control by '-force'
		    $runControl = 1;
		}
	    }

	    # Change the current directory to work here
	    unless (-d "$outputPath/$designName/work_${currentFileName}") {
		(system("mkdir $outputPath/$designName/work_${currentFileName}") == 0) || die ("ERROR: Cannot create work directory\n");
		print "Work directory created\n";
	    }

	    my $workDirectory = "$outputPath/$designName/work_${currentFileName}";
	    chdir($workDirectory) || die("Cannot chdir to $workDirectory $!");

	    if ($runControl == 1) {
		# Create the complete executable string here
		# print "EXECUTABLE: \n";
		my $execString = "$execPath -des ${designRoot}/${designName}/bookshelf/$designName.aux -output $outputPath/$designName/PL_files/${designName}.${currentFileName}";
		
		
		# Get the remaining exec string from allOptions array
		my $arrRef = $allOptions[$fileNum] ;
		my $fullExec = "$execString"." @$arrRef "."> ../logs/${designName}.${currentFileName}.log";
		
		# print "$fullExec\n";
		print "RUNNING placer for \"$designName\" ..... \n";


		#########################################################
		# STEP 3. RUN THE PLACER WITH SPECIFIED OPTIONS HERE  
		#########################################################
		print "STEP $stepCount: Running placer\n"; $stepCount++;
		(system("$fullExec") == 0) || die ("Cannot execute system command");
		print "EXECUTION OF THE PLACER COMPLETED SUCCESSFULLY\n";

		###########################################################
		# STEP 4. CREATE A .legend FILE TO INTERPRET THE FILE NAMES  
		###########################################################
		print "STEP $stepCount: Create \".legend\" file for interpretation of results\n"; $stepCount++;
		open(legendFile, ">$outputPath/$designName/PL_files/${designName}.${currentFileName}.legend") || die ("Cannot open file for writing");
		foreach my $key(sort {$a cmp $b} keys %legend) {
		    print legendFile "$key ====> $legend{$key}\n";
		}
		close(legendFile);

	    } elsif ($runControl == 0) {
		# Indicates files containing earlier results exist and
		# -force keyword not present in input file
		print "**** Results EXIST in ./$designName/PL_files/ *****\n";
		print "To RE-RUN placer DELETE existing files or USE \'-force\' in Input file\n";
	    }
	    
	    
	    ###############################################################
	    # STEP 5. CALCULATE THE TOTAL HPWL OF THE PLACED FILE
	    ###############################################################
	    print "STEP $stepCount: Calculating the HPWL of the placed file\n"; $stepCount++;
	    if ((-e "../PL_files/${designName}.${currentFileName}.hpwl") && !(exists $forceHash{$currentFileName})) {
		my $modStepCount = $stepCount - 1;
		print "Skipping STEP ${modStepCount}. HPWL file exist\n";
	    } else {
		my $resultsPath = "$designRoot/${designName}/bookshelf";
		(system("$utilsRoot/Utils/hpwl ${resultsPath}/${designName}.nodes ${resultsPath}/${designName}.pl ../PL_files/${designName}.${currentFileName}.pl ${resultsPath}/${designName}.nets > ../PL_files/${designName}.${currentFileName}.hpwl") == 0) || 
		    die ("Cannot generate the hpwl file");
	    }

	    ###############################################################
	    # STEP 6. REMOVE THE CLUSTERED NETLIST BEFORE NEXT RUN
	    ###############################################################
	    print "STEP $stepCount: Removing the clusters created in an intermediate step\n"; $stepCount++;
	    (system("rm -rf *__c*") == 0) || die ("Cannot remove the cluster directories created");
	    (system("rm -f c*_*") == 0) || die ("Cannot remove the cluster files");
	    
	    
	    push(@designPLFiles, $currentFileName);
	    $runControl = 1;
	    $fileNum++;
	}
	$allPLFiles{$designName} = [ @designPLFiles ];
	@designPLFiles = ();
    }
}

if ($execType ne "place") {
    
    #########################################################################
    #           PERFORM ROUTING AND TIMING ANALYSIS FOR ALL DESIGNS
    #########################################################################

    foreach  $designName(keys %allPLFiles) {
	foreach $currentFileName(@{$allPLFiles{$designName}}) {
	    
	    # Change to the specific work directory
	    my $workDirectory = "$outputPath/$designName/work_${currentFileName}";
	    chdir($workDirectory) || die("Cannot chdir to $workDirectory $!");
            


            #########################################################
	    # STEP 6. CLIP COMPONENTS AND NETS FROM THE ORIGINAL DEF 
	    #         TO PRODUCE THE BARE DEF
	    #########################################################
	    print "STEP $stepCount: Remove NETS and COMPONENTS from def to produce bare def\n"; $stepCount++;
	    # (system("pwd") == 0) || die ("Cannot execute pwd command");
	    if ((-e "../DEF/${designName}.${currentFileName}.def") && !(exists $forceHash{$currentFileName})) {
		my $modStepCount = $stepCount - 1;
		print "Skipping STEP ${modStepCount}. DEF File exists\n";
	    } else {
		(system("cp ${designRoot}/${designName}/bookshelf/${designName}.def ../DEF/${designName}.${currentFileName}.def") == 0) || die ("Cannot make copy of the DEF file\n");
		(system("mv ../DEF/${designName}.${currentFileName}.def ../DEF/${designName}.${currentFileName}.def.copy") == 0) || die ("Cannot make copy of the DEF file\n");
		open(DEFFileCopy, "../DEF/${designName}.${currentFileName}.def.copy") ||
		    die ("Cannot open copied DEF file\n");
		open(DEFFile, ">../DEF/${designName}.${currentFileName}.def") ||
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
	    }

	    
	    #############################################################
	    # STEP 7. REPLACE THE PSEUDO NAMES IN THE .pl OUTPUT OF THE
	    #         PLACER WITH THE ORIGINAL CELL NAMES
	    #############################################################
	    print "STEP $stepCount: Replace pseudo names in pl file with original cell names\n"; $stepCount++;
	    if ((-e "$outputPath/$designName/PL_files/${designName}.${currentFileName}.pl.orig") && !(exists $forceHash{$currentFileName})) {
		my $modStepCount = $stepCount - 1;
		print "Skipping STEP ${modStepCount}. PL File replaced with original cell names\n";
	    } else {
		(system("cp $outputPath/$designName/PL_files/${designName}.${currentFileName}.pl $outputPath/$designName/PL_files/${designName}.${currentFileName}.pl.orig") == 0) || die ("Cannot make copy of $outputPath/$designName/PL_files/${designName}.${currentFileName}.pl ");
		(system("$scriptRoot/subPseudoFromMaps.pl $outputPath/$designName/PL_files/${designName}.${currentFileName}.pl $designRoot/${designName}/bookshelf/$designName.nodes.map") == 0) || die ("Cannot replace pseudo names in PL file");
	    }
	    

	    ################################################################
	    # STEP 8. REPLACE THE ORIGINAL BARE DEF FILE WITH THE PLACED DEF
	    ################################################################
	    print "STEP $stepCount: Replace bare def file with placed def\n"; $stepCount++;
	    if ((-e "../DEF/${designName}.${currentFileName}.placed.def") && !(exists $forceHash{$currentFileName})) {
		my $modStepCount = $stepCount - 1;
		print "Skipping STEP ${modStepCount}. Placed DEF file exists\n";
	    } else {
		(system("cp ../DEF/${designName}.${currentFileName}.def ../DEF/${designName}.${currentFileName}.bare.def"));
		(system("$scriptRoot/convertPlToDef.pl ../DEF/${designName}.${currentFileName}.def $outputPath/$designName/PL_files/${designName}.${currentFileName}.pl ../DEF/${designName}.${currentFileName}.placed.def") == 0) || die ("Cannot generate DEF for placed netlist");
	    }

	    ###############################################################
	    # STEP 9. RUN ICC TO GENERATE A ROUTED NETLIST
	    ###############################################################
	    print "STEP $stepCount: Run ICC to generate routed netlist\n"; $stepCount++;
	    if ((-e "../DEF/${designName}.${currentFileName}.prerout.spef.max") && (-e "../DEF/${designName}.${currentFileName}.prerout.spef.min") && (-e "../DEF/${designName}.${currentFileName}.postrout.spef.max") && (-e "../DEF/${designName}.${currentFileName}.postrout.spef.min") && !(exists $forceHash{$currentFileName})){
		my $modStepCount = $stepCount - 1;
		print "Skipping STEP ${modStepCount}. spef files exist\n";
	    } else {
		(system("$scriptRoot/run_routing_flow.sh $designName ../DEF/${designName}.${currentFileName}.placed.def ../DEF/${designName}.${currentFileName}") == 0) || die ("Routing failed\n");
	    }
	    

	    ###############################################################
	    # STEP 10. READ THE PARASITICS FILE IN PRIME TIME AND GENERATE 
	    #         THE TIMING REPORT
	    ###############################################################
	    print "STEP $stepCount: Generate Timing reports using primetime\n"; $stepCount++;
	    if ((-e "../Timing/${designName}.${currentFileName}_post_route_timing.rpt")  && (-e "../Timing/${designName}.${currentFileName}_pre_route_timing.rpt") && !(exists $forceHash{$currentFileName})) {
		my $modStepCount = $stepCount - 1;
		print "Skipping STEP ${modStepCount}. Timing report files exist\n";
	    } else {
		(system("$scriptRoot/run_sta_flow.sh $designName ../DEF/${designName}.${currentFileName} ../Timing/${designName}.${currentFileName}") == 0) || die ("Cannot perform post route timing analysis");
	    }


            ###############################################################
	    # STEP 11. GENERATE THE PATHS FILE FOR THE BENCHMARK FROM THE  
	    #          POST ROUTED TIMING REPORT
	    ###############################################################
	    print "STEP $stepCount: Generate paths file from the timing reports\n"; $stepCount++;
	    if ((-e "../Timing/${designName}.${currentFileName}_post_route_timing_paths")  && (-e "../Timing/${designName}.${currentFileName}_pre_route_timing_paths") && !(exists $forceHash{$currentFileName})) {
		my $modStepCount = $stepCount - 1;
		print "Skipping STEP ${modStepCount}. Timing_paths files exist\n";
	    } else {
		(system("$scriptRoot/getTimingRpt.pl ../Timing/${designName}.${currentFileName}_pre_route_timing.rpt ../Timing/${designName}.${currentFileName}_pre_route_timing_paths") == 0) || 
		    die ("Cannot generate the pre-routed timing paths file");
		(system("$scriptRoot/getTimingRpt.pl ../Timing/${designName}.${currentFileName}_post_route_timing.rpt ../Timing/${designName}.${currentFileName}_post_route_timing_paths") == 0) || 
		    die ("Cannot generate the routed timing paths file");
	    }


	    
	}
    }
    if (0) {
    ###############################################################
    # STEP 12. COMPILE THE RESULTS OF ANALYSIS INTO A HASH OF   
    #          HASHES AND DUMP OUT THE RESULTS TO A FILE
    ###############################################################
    my $hpwlFile;
    my $hpwlVal;
    my $logFile;
    my $cpuTime;
    my $globalPTime;
    my $clusteringTime;
    my $shapingTime;
    my $legalizeTime;
    my $detailedPTime;
    my $preTimingFile;
    my $preWNSVal;
    my $postTimingFile;
    my $postWNSVal;

    open(resultFile, "> ${outputPath}/resultsDump.tab") || die("Cannot open file for writing");

# Print the header for the compiled results file 
    print resultFile "DESIGN\tPL_FILE\tHPWL\tTOTAL_CPU_TIME\tGLOBAL_PLACEMENT_TIME\tCLUSTERING_TIME\tSHAPE_SELECTION_TIME\tLEGALIZATION_TIME\tDETAILED_PLACEMENT_TIME\tPRE_ROUTE_WNS\tPOST_ROUTE_WNS\n";

    foreach  $designName(keys %allPLFiles) {
	foreach $currentFileName(@{$allPLFiles{$designName}}) {
	    
	    # Changing current directory to work
	    my $workDirectory = "$outputPath/$designName/work_${currentFileName}";
	    chdir($workDirectory) || die("Cannot chdir to $workDirectory $!");
	    print "Output path : $workDirectory\n";
	    $hpwlFile = "../PL_files/${designName}.${currentFileName}.hpwl";
	    $logFile = "../logs/${designName}.${currentFileName}.log";
	    $preTimingFile = "../Timing/${designName}.${currentFileName}_pre_route_timing.rpt";
	    $postTimingFile = "../Timing/${designName}.${currentFileName}_post_route_timing.rpt";
	    $hpwlVal = getHPWL($hpwlFile);
	    $globalPTime = getGlobalPlaceTime($logFile);
	    $clusteringTime = getClustTime($logFile);
	    $shapingTime = getShapingTime($logFile);
	    $legalizeTime = getLegalizingTime($logFile);
	    $detailedPTime = getDetailedPlaceTime($logFile);
	    $preWNSVal = getPreWNS($preTimingFile);
	    $postWNSVal = getPostWNS($postTimingFile);
	    $cpuTime = $globalPTime + $clusteringTime + $shapingTime + $legalizeTime + $detailedPTime;
	    print resultFile "$designName\t${designName}.${currentFileName}.pl\t$hpwlVal\t$cpuTime\t$globalPTime\t$clusteringTime\t$shapingTime\t$legalizeTime\t$detailedPTime\t$preWNSVal\t$postWNSVal\n";
	}
    }
    close(resultFile);
    }
}


print "\n ANALYSIS COMPLETED. EXITING...... \n";

sub getHPWL 
{
    my $line;
    my $hpwlVal;
    open(inFile, $_[0]) || die ("Cannot open ${_[0]} file reading");

    while ($line=<inFile>) {
	chomp($line);
	if ($line =~ m/^\s*Total[\s]HPWL:[\s](\d+)[\s](.*)/) {
	    $hpwlVal = $1;
	    last;
	}
    }
    close(inFile);
    return "$hpwlVal";
}

sub getGlobalPlaceTime
{
    my $line;
    my $gpTime;
    open(inFile, $_[0]) || die ("Cannot open ${_[0]} file reading");
    while ($line=<inFile>) {
	chomp($line);
	if ($line =~ m/^[\s]+GLOBAL[\s]+PLACEMENT[\s]+TIME:[\s]*(\d+\.\d+|\.\d+|\d+)s$/) {
	    $gpTime = $1;
	    last;
	}
    }
    return "$gpTime";
}

sub getClustTime
{
    my $line;
    my $clustTime;
    open(inFile, $_[0]) || die ("Cannot open ${_[0]} file reading");
    while ($line=<inFile>) {
	chomp($line);
	if ($line =~ m/^[\s]+CLUSTERING[\s]+TIME:[\s]*(\d+\.\d+|\.\d+|\d+)s$/) {
	    $clustTime = $1;
	    last;
	}
    }
    return "$clustTime";
}

sub getShapingTime
{
    my $line;
    my $shapeTime;
    open(inFile, $_[0]) || die ("Cannot open ${_[0]} file reading");
    while ($line=<inFile>) {
	chomp($line);
	if ($line =~ m/^[\s]+SHAPE[\s]+SELECTION[\s]+TIME:[\s]*(\d+\.\d+|\.\d+|\d+)s$/) {
	    $shapeTime = $1;
	    last;
	}
    }
    return "$shapeTime";
}

sub getLegalizingTime
{
    my $line;
    my $lgTime;
    open(inFile, $_[0]) || die ("Cannot open ${_[0]} file reading");
    while ($line=<inFile>) {
	chomp($line);
	if ($line =~ m/^[\s]+LEGALIZATION[\s]+TIME:[\s]*(\d+\.\d+|\.\d+|\d+)s$/) {
	    $lgTime = $1;
	    last;
	}
    }
    return "$lgTime";
}

sub getDetailedPlaceTime
{
    my $line;
    my $dpTime;
    open(inFile, $_[0]) || die ("Cannot open ${_[0]} file reading");
    while ($line=<inFile>) {
	chomp($line);
	if ($line =~ m/^[\s]+DETAILED[\s]+PLACEMENT[\s]+TIME:[\s]*(\d+\.\d+|\.\d+|\d+)s$/) {
	    $dpTime = $1;
	    last;
	}
    }
    return "$dpTime";
}

sub getPreWNS
{
    my $line;
    my $preWns;
    open(inFile, $_[0]) || die ("Cannot open ${_[0]} file reading");
    while ($line=<inFile>) {
	chomp($line);
	if ($line =~ /^[\s]+slack[\s]+\(VIOLATED\)[\s]+(-?\d+\.\d+|-?\.\d+)/) {
	    $preWns = $1;
	    last;
	}
    }
    return "$preWns";
}

sub getPostWNS
{
    my $line;
    my $postWns;
    open(inFile, $_[0]) || die ("Cannot open ${_[0]} file reading");
    while ($line=<inFile>) {
	chomp($line);
	if ($line =~ /^[\s]+slack[\s]+\(VIOLATED\)[\s]+(-?\d+\.\d+|-?\.\d+)/) {
	    $postWns = $1;
	    last;
	}
    }
    return "$postWns";
}

exit(0);
