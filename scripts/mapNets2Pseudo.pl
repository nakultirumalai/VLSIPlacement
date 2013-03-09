#!/usr/bin/perl
use strict;
use warnings;

my $netsFile = shift(@ARGV);
open(inFile, $netsFile) || die ("Cannot open $netsFile for reading");
open(outFile, ">fileNetMap.txt") || die ("Cannot open file for writing");
my $i=0;
print outFile "Pseudo_name\tNet_name\n";
while(my $line = <inFile>) {
    next if($line =~ m/^(#|\n|NumNodes|NumTerminals|UCLA)/);
    chomp($line);
    next unless($line =~ m/^NetDegree/);
    my @arrSplitted = split('\s+', $line);
    print outFile "Net$i\t$arrSplitted[3]\n";
    $i++;
}
close(outFile);
close(inFile);
exit(0);
