#!/usr/bin/perl

my $timingFileName = $ARGV[0];
my $outputFileName = $ARGV[1];

open (timingRptFile, $timingFileName) or die "Cannot open timing report";
open (OUT, ">$outputFileName") or die "Cannot open output file";

# Print in the following format
# PATH        DELAY         MAX DELAY       SETUP TIME        REQUIRED TIME         ARRIVAL TIME     SLACK
# PATH: comma separated names of cell/pin_name   
# DELAY: delay of the path 
# MAX DELAY: delay allowed on the path
# 

my $count=0;
my $startPath = 0;
my $createArr;
my $starDefBegin = 0;
my @pathsArray;
my @path;
foreach $line (<timingRptFile>) {
#    print "LINE: $line";
#    print "starDefBegin: $starDefBegin\n";
    chomp($line);

    if ($starDefBegin == 0) {
	if ($line =~ m/^\*+/) {
	    $starDefBegin = 1;
	    next;
	}
    }
    if ($starDefBegin == 1) {
	if ($line =~ m/^\*+/) {
	    $starDefBegin = 0;
	}
	next;
    }

#    print "$line\n";
    if ($startPath == 1) {
	# start matching to any of the below 
	if ($line =~ /[ \t]*input external delay[ \t]+([0-9]+\.[0-9]+)[ \t]+([0-9]+\.[0-9]+)[ ]+([fr]*).*$/) {
	    push @path, ( "input_external_delay" );
#	    print "MATCH\n";
#	    print "@path\n";
	    push @path, ( $1 );
	} elsif ($line =~ /[ \t]*data arrival time[ \t]+([0-9]+\.[0-9]+).*$/) {
	    push @path, ( "data_arrival_time" );
	    push @path, ( $1 );
#	    print "MATCH\n";
#	    print "@path\n";
	} elsif ($line =~ /[ \t]*max_delay[ \t]+([0-9]+\.[0-9]+).*$/) {
	    push @path, ( "max_delay" );
	    push @path, ( $1 );
#	    print "MATCH\n";
#	    print "@path\n";
	} elsif ($line =~ /[ \t]*library setup time[ \t]+([0-9]+\.[0-9]+).*$/) {
	    push @path, ( "library_setup_time" );
	    push @path, ( $1 );
#	    print "MATCH\n";
#	    print "@path\n";
	} elsif ($line =~ /[ \t]*data required time[ \t]+([0-9]+\.[0-9]+).*$/) {
	    push @path, ( "data_required_time" );
	    push @path, ( $1 );
#	    print "MATCH\n";
#	    print "@path\n";
	} elsif ($line =~ /\s*(\S+)\s+(\S+)\s+([0-9]+\.[0-9]+).*([0-9]+\.[0-9]+)\s+([fr]+)/) {
#	    print "MATCH CELL\n";
#	    print "FIRST ARG: $1\n";
#	    print "SECOND ARG: $2\n";
	    push @path, ( $1 );
	    push @path, ( $2 );
	}
    }
    if ($line =~/[ \t]*Point[ \t]*Incr[ \t]*Path[ \t]*$/) {
#	print "MATCH begin\n";
#	print "@path\n";
	$startPath = 1;
	@path = ();
    } elsif ($line =~/[ \t]*slack[ \t]+([^ ]+)[ \t]+([^ ]+).*$/) {
	push @path, ( "slack" );
	push @path, ( $2 );
	$startPath = 0;
#	print "MATCH end\n";
#	print "@path\n";
	my @pathToBeAdded;
	foreach $pathData (@path) {
	    push @pathToBeAdded, ( $pathData );
	}
	push @pathsArray, [ @pathToBeAdded ];
    }
}


foreach $pathArrayRef ( @pathsArray ) {
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
		print OUT "$keyword ";
		$pathDelay = $pathDelay + $pathData;
	    }
	} else {
	    $keyword = $pathData;
	}
    }
    print OUT " $slack\n";
}

 #   my @pathArray = $$pathArrayRef;
#    foreach $pathData (@pathArray) {
#	print "$pathData ";
#    }
#    print "\n";
#}
