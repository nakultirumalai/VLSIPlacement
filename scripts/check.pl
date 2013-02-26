#!/usr/bin/perl

my $line;
my $nodesFileStr = $ARGV[0];
my $plFileStr = $ARGV[1]; 
my $stdCellHeight = $ARGV[2];

open (nodesFile, $nodesFileStr) or die "Cannot open nodes file!\n";
open (plFile, $plFileStr) or die "Cannot open pl file!\n";

my %macroNodes;
my $fixedMacros;
my $nonFixedMacros;
my $macroCount;

$macroCount = 0;
foreach $line (<nodesFile>) {
    if ($line =~ /^[ \t]*(o[0-9]+)[ \t]+([0-9]+)[ \t]([0-9]+)(.*)/) {
	if ($3 != $stdCellHeight) {
	    if (0) {
		print "Node: ";
		print "$1";
		print "  Width: ";
		print "$2";
		print " Height: ";
		print "$3";
		print "\n";
	    }
	    $macroNodes{$1} = 0;
	    $macroCount++;
	}
    }
}
print "Total macros in the design: $macroCount\n";

$fixedMacros = 0;
foreach $line (<plFile>) {
    if ($line =~ /^[ \t]*(o[0-9]+).*FIXED$/) {
	if (exists $macroNodes{$1}) {
	    $macroNodes{$1} = 1;
	    $fixedMacros++;
	}
    }
}
$nonFixedMacros = $macroCount - $fixedMacros;
print "Total fixed macros in the design: $fixedMacros\n";
print "Total free macros in the design: $nonFixedMacros\n";

if (0) {
    foreach my $key (keys %macroNodes) {
	if ($macroNodes{$key} != 1) {
	    print "Macro Name: $key\n";
	    print "\tLine from the .nodes file:";
	    system("grep \"$key\" \"$nodesFileStr\"");
	    print "\tLine from the .pl file:";
	    system("grep \"$key\" \"$plFileStr\"");
	    print "\n";
	}
    }
}

close nodesFile;
close plFile;

