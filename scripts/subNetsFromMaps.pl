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
my %net2Pseudo=();
while(my $row = <inmapFile>)
{
    chomp($row);
    next if($row =~ m/^Pseudo_name/);
    my ($pseudo, $netName) = split('\t+',$row);
    $net2Pseudo{$netName}= $pseudo;
}
close(inmapFile);

open (targetFileCopy, "$targetFile.copy") || die ("C");
open (targetFileHndl, ">$targetFile") || die ("C");

while (my $line = <targetFileCopy>) {
    if($line =~ m/^NetDegree/)
    {
    	my @atribs = split(/\s+/, $line);
    	foreach my $atribName (@atribs) {
            #my $cellNameBak = $cellName;
            if (exists $net2Pseudo{$atribName}) {
                #    print "BEFORE: \t$line\n";
                $line =~ s/\Q$atribName\E/${net2Pseudo{$atribName}}/g;
                #    print "AFTER:\t$line\n";
            }
        }
    }
    print targetFileHndl $line;
}

close (targetFileHndl);
close (targetFileCopy);

system("rm -rf ${targetFile}.copy") == 0 || die ("Cannot remove target file copy");
