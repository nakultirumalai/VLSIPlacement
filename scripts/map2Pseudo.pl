#!/usr/bin/perl
use strict;
use warnings;

my $nodesFile = shift(@ARGV);
my $outFile = shift(@ARGV);

open(inFile, $nodesFile) || die ("Cannot open $nodesFile for reading");
open(outFile, ">$outFile") || die ("Cannot open file for writing");
my $i=0;
print outFile "Pseudo_name\tCell_name\tLibrary_name\tStandard_cell_name\n";
while(my $line = <inFile>) {
    next if($line =~ m/^(#|\n|NumNodes|NumTerminals|UCLA)/);
    if ($line =~m/\s*(.*)\s*PAD\s*$/) {
	my $cell_name = $1; 
	print outFile "C$i\t$cell_name -- PAD\n";
	$i++;
	next;
    } 
    chomp($line);
    $line =~ s/^\s*//;
    my ($cell_name, $full_lib_name, $type) = split('\s+', $line);
    my @splitlib_names = split('/',$full_lib_name);
    print outFile "C$i\t$cell_name\t$splitlib_names[0]\t$splitlib_names[1]\n";
    $i++;
}
close(outFile);
close(inFile);
exit(0);
