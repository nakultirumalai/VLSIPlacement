#!/usr/bin/perl

my $timingFileName = $ARGV[0];
my $outputFileName = "out";

open (timingRptFile, $timingFileName) or die "Cannot open timing report";

# Print in the following format
# PATH        DELAY         MAX DELAY       SETUP TIME        REQUIRED TIME         ARRIVAL TIME     SLACK
# PATH: comma separated names of cell/pin_name   
# DELAY: delay of the path 
# MAX DELAY: delay allowed on the path
# 

my $count=0;
my $startPath = 0;
my $createArr;
my @pathsArray;
my @path;
foreach $line (<timingRptFile>) {
    if ($line =~ /[-]+/) {
	next;
    }

    print 
    if ($startPath == 1) {
	# start matching to any of the below 
	if ($line =~ /[ \t]*input external delay[ \t]+([0-9]+\.[0-9]+)[ \t]+([0-9]+\.[0-9]+)[ ]+([fr]*).*$/) {
	    push(@path, "input_external_delay");
	    push(@path, $1);
	} elsif ($line =~ /[ \t]*data arrival time[ \t]+([0-9]+\.[0-9]+).*$/) {
	    push(@path, "data_arrival_time");
	    push(@path, $1);
	} elsif ($line =~ /[ \t]*max_delay[ \t]+([0-9]+\.[0-9]+).*$/) {
	    push(@path, "max_delay");
	    push(@path, $1);
	} elsif ($line =~ /[ \t]*library setup time[ \t]+([0-9]+\.[0-9]+).*$/) {
	    push(@path, "library_setup_time");
	    push(@path, $1);
	} elsif ($line =~ /[ \t]*data required time[ \t]+([0-9]+\.[0-9]+).*$/) {
	    push(@path, "data_required_time");
	    push(@path, $1);
	} elsif ($line =~ /^[ \t]*([^ ]+) ([^ ]+)[ \t]+([0-9]+\.[0-9]+)[ \t]+([0-9]+\.[0-9]+)[ ]+([fr]*).*$/) {
	    push(@path, $1);
	    push(@path, $2);
	}
    }
    if ($line =~/[ \t]*Point[ \t]*Incr[ \t]*Path[ \t]*$/) {
	$startPath = 1;
	@path = ();
    } elsif ($line =~/[ \t]*slack[ \t]+([^ ]+)[ \t]+([^ ]+).*$/) {
	push(@path, "slack");
	push(@path, $2);
	$startPath = 0;
	my @pathToBeAdded;
	foreach $pathData (@path) {
	    push @pathToBeAdded, ($pathData);
	}
	push @pathsArray, [ @pathToBeAdded ];
    }
}

print scalar(@pathsArray);

foreach $pathArrayRef ( @pathsArray ) {
 #   print "$pathArrayRef";
    my $getKeyWord = 0;
    my $addDelay = 0;
    my $pathDelay = 0;
    my $keyword;
    my $addPathDelayInNxtItr = 0;
    my $max_delay = 0.0;
    my $dataReqTime = 0.0;
    my $libSetupTime = 0.0;
    my $dataArrivalTime = 0.0;
    my $slack = 0.0;

    foreach $pathData ( @$pathArrayRef ) {
	if ($getKeyWord == 0) {
	    $getKeyWord = 1;
	    $addDelay = 0;
	} else {
	    $getKeyWord = 0;
	    $addDelay = 1;
	}
	if ($addDelay == 1) {
	    if ($keyword eq "max_delay") {
		$max_delay = $pathData;
	    } elsif ($keyword eq "data_required_time") {
		$dataReqTime = $pathData;
	    } elsif ($keyword eq "data_arrival_time") { 
		$dataArrivalTime = $pathData;
	    } elsif ($keyword eq "library_setup_time") {
		$libSetupTime = $pathData;
	    } elsif ($keyword eq "slack") {
		$slack = $pathData;
	    } else {
		print "$keyword ";
		$pathDelay = $pathDelay + $pathData;
	    }
	} else {
	    $keyword = $pathData;
	}
    }
    print " $slack\n";
}

 #   my @pathArray = $$pathArrayRef;
#    foreach $pathData (@pathArray) {
#	print "$pathData ";
#    }
#    print "\n";
#}
