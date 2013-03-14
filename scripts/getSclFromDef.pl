#!/usr/bin/perl
use strict;
use warnings;

my $defFile = shift(@ARGV);
my $width = shift(@ARGV);
my $height = shift(@ARGV);
my $symmetry = shift(@ARGV);

open(inDEF, $defFile) || die ("Cannot open $defFile for reading");
#open(inLEF, $lefFile) || die ("Cannot open $lefFile for reading");
my %rowSpecs = ();
my $rowCount = 0;
while(my $line = <inDEF>){
    chomp($line);
    next unless($line =~ m/^ROW/);
    $rowCount++;
    my @rowI = split (/\s+/, $line);
    next if(exists $rowSpecs{$rowI[1]});
    $rowSpecs{"$rowI[1]"}=\@rowI;
}
close(inDEF);
my $sclFile = $defFile;
$sclFile =~ s/\.def$/\.scl/g;
open(outSCL, ">$sclFile") || die ("Cannot open file for writing");
print outSCL "Benchmark name SCL \n";
my $dateInfo = `date`;
my $userInfo = "User info";
my $systemInfo = `uname -a`;
print outSCL "Created    :    $dateInfo\n";
print outSCL "User    :    $userInfo\n";
print outSCL "Platform    :    $systemInfo\n";
print outSCL "\n\n";
print outSCL "NumRows  :  $rowCount\n";
print outSCL "\n";
foreach my $row_I(keys %rowSpecs){
    my $rowRef = $rowSpecs{$row_I};
    my @rowArr = (@$rowRef);
    print outSCL "CoreRow Horizontal\n";
    print outSCL " Coordinate   :  $rowArr[4]\n";
    print outSCL " Height       :  $height\n";
    print outSCL " Sitewidth    :  $width\n";
    print outSCL " Sitespacing  :  $rowArr[11]\n";
    print outSCL " Siteorient   :  $rowArr[5]\n";
    print outSCL " Sitesymmetry :  $symmetry\n";
    print outSCL " SubrowOrigin :  $rowArr[3]  NumSites :\t$rowArr[7]\n";
    print outSCL "End\n";    
}
close(outSCL);
exit(0);
