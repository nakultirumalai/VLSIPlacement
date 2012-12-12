#!/usr/bin/perl -w

# Packages to use
use PDF::API2;
use PDF::Table;

# Check if the number of arguments provided as 
# input are correct;
$numArgs = $#ARGV + 1;

if ($numArgs != 1) {
    print "Usage: genPDF.pl <Benchmark Name>\n";
    exit;
}     

$benchmarkName = $ARGV[0];
print "Analysing data generated for benchmark: $benchmarkName\n";

$analysisDir = $benchmarkName."_Analysis";
print "Looking for directory $analysisDir:";

# Search for the analysis directory and exit
# if it does not exist
@files = <*>;
$found = 0;
foreach $file (@files) {
    if (-d $file) {
	if ($file eq $analysisDir) {
	    $found = 1;
	    last;
	}
    }
}

if ($found == 1) {
    print "Found\n";
} else {
    print "Not found\n";
    print "Exiting\n";
    exit;
}

print "Proceeding with analysis.\n";

# Make a list of files for analysis
@files = <$analysisDir/*.txt>;

# Filter files which are analysis of number of outputs 
# and inputs
@inputOutputAnalysisFiles = grep /[0-9]+Outputs[0-9]+Inputs/, @files;
print "Count".$#inputOutputAnalysisFiles."\n";

# Strip directory name from each file
@strippedFileNames=@inputOutputAnalysisFiles;
foreach (@strippedFileNames) {
    s/^.*_Analysis\///g;
}

my %firstHash;

foreach $key (@strippedFileNames) {
    my ($n1, $n2);
    if ($key =~ /(\d+).*(\d+)/) {
	$n1 = $1;
	$n2 = $2;
    } else {
	next;
    }
    
    if (exists $firstHash{$n1}) {
	if (exists $firstHash{$n1}{$n2}) {
	    push(@ {$firstHash{$n1}{$n2}}, $key);
	} else {
	    my @arr = ($key);
	    $firstHash{$n1}{$n2} = \@arr;
	}
    } else {
	my %secondHash;
	my @arr = ($key);
	$secondHash{$n2} = \@arr;
	$firstHash{$n1} = \%secondHash;
    }
}

my @sortedFileNames;
foreach my $key1 (sort { $a <=> $b}  keys %firstHash) {
    foreach my $key2 (sort { $a <=> $b}  keys % { $firstHash{$key1}}) {
	foreach my $val (@ { $firstHash{$key1}{$key2}}) {
	    push(@sortedFileNames, $val);
	}
    }
}

# For each of the sorted files,
# read the table and write it out to PDF
foreach $fname (@sortedFileNames) {
    $fullName = $analysisDir . "/" . $fname;
    open my $info, $fullName or die "Could not open $fullName: $!";
    $titleRead = 0;
    my ($col1Title, $col2Title, $title);
    my @col1Values;
    my ($total, $mean, $variance, $stdDev);
    while (my $line = <$info>) {
	if ($titleRead == 0) {
	    if ($line =~ /^#(.*)[ \t]+(.*)/) {
		$col1Title = $1;
		$col2Title = $2;
		$titleRead = 1;
		my @col2Values;
		push(@col2Values, $1);
		push(@col2Values, $2);
		push(@col1Values, \@col2Values);
		next;
	    }
	} elsif ($line =~ /(\d+)[ \t]+(\d+)/) {
	    my @col2Values;
	    push(@col2Values, $1);
	    push(@col2Values, $2);
	    push(@col1Values, \@col2Values);
	} elsif ($line =~ /^#TOTAL[ \t]+(\d+)/) {
	    $total = $1;
	} elsif ($line =~ /^#STD DEVIATION[ \t]+(\d+)/) {
	    $stdDev = $1;
	} elsif ($line =~ /^#MEAN[ \t]+(\d+)/) {
	    $mean = $1;
	} elsif ($line =~ /^#VARIANCE[ \t]+(\d+)/) {
	    $variance = $1;
	} else {
	    die "Garbage found in analysis files. Redo analysis for $benchmarkName";
	}
    }
    my $pdfFname;
    if ($fname =~ /([a-z | A-Z | 0-9]+).txt/) {
	$pdfFname = $1 . ".pdf";
    } else {
	die "Unable to generate PDF file";
    }
    my $data = \@col1Values;
    my $pdf  = PDF::API2->new(-file => $pdfFname);
    my $pdftable = new PDF::Table;

    $pdf->mediabox(595,842);
    my $page = $pdf->page;
    my $fnt = $pdf->corefont('Arial',-encoding => 'latin1'); 
    my $txt = $page->text;
    $txt->textstart;
    $txt->font($fnt, 20);
    $txt->textend;
    $pdftable->table(
	# required params
	$pdf,
	$page,
	$data,
	x => 50,
	w => 495,
	start_y => 750,
	next_y  => 700,
	start_h => 300,
	next_h  => 500,
	# some optional params
	padding => 5,
	padding_right => 10,
	background_color_odd  => "gray",
	background_color_even => "lightblue", #cell background color for even rows
	);
    $pdf->saveas("$pdfFname");
    $pdf->end();
    close $info;
}
# Process files that are inputs and outputs 

