#!/usr/bin/perl

#######################################################
# What this script does?
# Substitutes actual cell names in any file with pseudo names
# Usage:
# ./subFromMaps.pl <FileInWhichCellNamesHaveToBeSubstituted> fileMap.txt

use strict;
use warnings;

my $targetFile = shift(@ARGV);
my $mapFile = shift(@ARGV);

system("mv ${targetFile} ${targetFile}.copy") == 0 || die ("Cannot execute copy command");

open(inmapFile, $mapFile) || die ("Cannot open $mapFile for reading");
my %cell2Pseudo=();
while(my $row = <inmapFile>)
{
    chomp($row);
    next if($row =~ m/^Pseudo_name/);
    my ($pseudo, $cellName, $libCellName, $type) = split('\t+',$row);
    $cell2Pseudo{$cellName}= $pseudo;
}
close(inmapFile);

open (targetFileCopy, "$targetFile.copy") || die ("C");
open (targetFileHndl, ">$targetFile") || die ("C");

while (my $line = <targetFileCopy>) {
    my @cellNames = split(/\s+/, $line);
    
    foreach my $cellName (@cellNames) {
	if (exists $cell2Pseudo{$cellName}) {
#	    print "BEFORE: \t$line";
	    $line =~ s/$cellName/${cell2Pseudo{$cellName}}/g;
#	    print "AFTER:\t$line";
	}
    }
    print targetFileHndl $line;
}

close (targetFileHndl);
close (targetFileCopy);

