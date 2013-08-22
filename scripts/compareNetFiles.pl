#!/usr/bin/perl

use strict;
use warnings;

# Compare two net files of the same design
# Compares the following attributes:
# 1) Number of nets are equal
# 2) Each net in nets file 1 is equivalent to each net in nets file 2
#    While checking for equivalence, the following things are checked:
#      - For each net, the number of pins is the same
#      - For each net, the cell names recorded is the same
#      - For each cell name, the pin offsets are the same
#my $nodesFile = shift(@ARGV);
my $firstNetFile = shift(@ARGV);
my $secondNetFile = shift(@ARGV);

if ($firstNetFile eq "" || $secondNetFile eq "") {
    print "Usage: <file1>.nets <file2>.nets\n";
    exit(0);
}

my %netHash;
my %cellNameHash;
my $netName;
my $netDefBegin;
my $netNumPins;
my $curNetPinCount;
# Store all the nets in the memory hashed by name 
# Each name key returns an array value which has the 
# following:
# Hash of the cell names and each value in the hash 
# is a pair of offsets
$netDefBegin = 0;
$netNumPins = 0;
$curNetPinCount = 0;
open(inFile1, $firstNetFile) || die ("Cannot open file: $firstNetFile\n");;
while (my $line = <inFile1>) {
    next if ($line =~ m/^(#|\n|UCLA)/);
    chomp($line);
    $line =~ s/^\s+//;
    $line =~ s/\s+$//;
    if ($line =~ m/^NetDegree/) {
	my @splitAttrs = split('\s+', $line);
	my $netDegree = $splitAttrs[2];
	$netName = $splitAttrs[3];
	$netDefBegin = 1;
	$netNumPins = $netDegree;
	$curNetPinCount = 0;
	$netHash{$netName} = [ ];
	next;
    }
    if ($netDefBegin == 1) {
	my @splitAttrs = split('\s+', $line);
	my $cellName = $splitAttrs[0];
	my $pinDirection = $splitAttrs[1];
	my $pinXOffset = $splitAttrs[3];
	my $pinYOffset = $splitAttrs[4];
	my $pinInfoArray = [$cellName, $pinDirection, $pinXOffset, $pinYOffset];
	push(@{$netHash{$netName}}, $pinInfoArray);
	$curNetPinCount++;
	if ($curNetPinCount == $netNumPins) {
	    $netDefBegin = 0;
	    $curNetPinCount = 0;
	    $netNumPins = 0;
	}
    }
}
close(inFile1);

$netDefBegin = 0;
$netNumPins = 0;
$curNetPinCount = 0;
my $problemFound = 0;
open(inFile2, $secondNetFile) || die ("Cannot open file: $secondNetFile\n");
while (my $line = <inFile2>) {
    next if ($line =~ m/^(#|\n|UCLA)/);
    chomp($line);
    $line =~ s/^\s+//;
    $line =~ s/\s+$//;
    if ($line =~ m/^NetDegree/) {
	my @splitAttrs = split('\s+', $line);
	my $netDegree = $splitAttrs[2];
	$netName = $splitAttrs[3];
	$netDefBegin = 1;
	$netNumPins = $netDegree;
	$curNetPinCount = 0;
	next;
    }
    if ($netDefBegin == 1) {
	my @splitAttrs = split('\s+', $line);
	my $cellName = $splitAttrs[0];
	my $pinDirection = $splitAttrs[1];
	my $pinXOffset = $splitAttrs[3];
	my $pinYOffset = $splitAttrs[4];
	my $pinInfoArrOfArr = $netHash{$netName};
	my $found = 0;
	foreach (@$pinInfoArrOfArr) {
	    my @pinInfoArr = @$_;
	    if ($cellName eq $pinInfoArr[0]) {
		if (!($pinInfoArr[1] eq $pinDirection)) {
		    next;
		}
		if (!($pinInfoArr[2] == $pinXOffset)) {
		    next;
		}
		if (!($pinInfoArr[3] == $pinYOffset)) {
		    next;
		}
		$found = 1;
		last;
	    }
	}
	if ($found == 0) {
	    print "Problem found in equivalence of net \"$netName\"\n";
	    print "   Problem: pin of cell \"$cellName\" does not match\n";
	    $problemFound = 1;
	    last;
	}
    }
}
if ($problemFound == 1) {
    print "Problem has been when the two net files are compared!\n";
} else {
    print "Files match!\n";
}
close(inFile2);

if (0) {
    print "Iterating over built hash:";
    foreach my $key1 (keys %netHash) {
	print "KEY1: $key1\n";
	my $pinInfoArrayRef = $netHash{$key1};
	my @pinInfoArr = @$pinInfoArrayRef;
	foreach (@pinInfoArr) {
	    foreach (@$_) {
		print "$_ ";
	    }
	    print "\n";
	}
    }
}

if (0) {
    print "Iterating over built hash:";
    foreach my $key1 (keys %netHash) {
	print "KEY1: $key1\n";
	foreach my $key2 (keys $netHash{$key1}) {
	    print "\tKEY2: $key2\n";
	    my $pinInfoArrayRef = $netHash{$key1}{$key2};
	    my @pinInfoArr = @$pinInfoArrayRef;
	    foreach (@pinInfoArr) {
		print "\t$_\n";
	    }
	}
    }
}
exit(0);
#die("\n");
