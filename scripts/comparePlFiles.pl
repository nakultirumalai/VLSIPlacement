#!/usr/bin/perl

use strict;
use warnings;

my $nodesFile = shift(@ARGV);
my $firstPlFile = shift(@ARGV);
my $secondPlFile = shift(@ARGV);

if ($nodesFile eq "" || $firstPlFile eq "" || $secondPlFile eq "") {
    print "Usage: <designName>.nodes <file1>.pl <file2>.pl\n";
    exit(0);
}

open(nodeFile, $nodesFile);
my $numNodes = 0;
while (my $line = <nodeFile>) {
    next if($line =~ m/^(#|\n|UCLA)/);
    if ($line =~ m/NumNodes/) {
	my @splitAttrs = split('\s+', $line);
	$numNodes = $splitAttrs[2];
	last;
    }
}
close(nodeFile);

my %xposHash;
my %yposHash;
my %orientHash;

open(inFile1, $firstPlFile);
while (my $line1 = <inFile1>) {
    next if ($line1 =~ m/^(#|\n|UCLA)/);
    chomp($line1);
    $line1 =~ s/^\s+//;
    $line1 =~ s/\s+$//;
    my @splitAttrs = split('\s+', $line1);
    my $cellName = $splitAttrs[0];
    my $cellXpos = $splitAttrs[1];
    my $cellYpos = $splitAttrs[2];
    my $cellOrient = $splitAttrs[4];
#    print "Read cell: $cellName   ($cellXpos, $cellYpos) $cellOrient\n";
    $xposHash{$cellName} = $cellXpos;
    $yposHash{$cellName} = $cellYpos;
    $orientHash{$cellName} = $cellOrient;
}
close(inFile1);
#die("\n");
open(inFile2, $secondPlFile);
my $equal = 1;
while (my $line2 = <inFile2>) {
    next if ($line2 =~ m/^(#|\n|UCLA)/);
    chomp($line2);
    $line2 =~ s/^\s+//;
    $line2 =~ s/\s+$//;
#    print $line2;
    my @splitAttrs = split('\s+', $line2);
    my $cellName = $splitAttrs[0];
    my $cellXpos2 = $splitAttrs[1];
    my $cellYpos2 = $splitAttrs[2];
    my $cellOrient2 = $splitAttrs[4];
#    print "Read cell: $cellName   ($cellXpos2, $cellYpos2) $cellOrient2\n";
    my $cellXpos1 = $xposHash{$cellName};
    my $cellYpos1 = $yposHash{$cellName};
    my $cellOrient1 = $orientHash{$cellName};
    if ($cellXpos1 != $cellXpos2  || $cellYpos1 != $cellYpos2 || !($cellOrient1 eq $cellOrient2)) {
	print "$firstPlFile : $cellName  ($cellXpos1, $cellYpos1)   $cellOrient1\n";
	print "$secondPlFile : $cellName  ($cellXpos2, $cellYpos2)   $cellOrient2\n";
	$equal = 0;
	last;
    }
}

if ($equal == 1) {
    print "Files match\n";
}

close(inFile2);

#print "Number of nodes read: $numNodes\n";
