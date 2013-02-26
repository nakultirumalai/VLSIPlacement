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
foreach $line (<timingRptFile>) {
    print "$line\n";
    if ($line =~/[ \t]*Point[ \t]*Incr[ \t]*Path[ \t]*$/) {
	    print "MATCH: $line\n";
	    $count++;
    }
    if ($count > 10) {
	last;
    }
}

