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
my $pdfFname = $benchmarkName . "NumInputOutputData" . ".pdf";
my $pdf  = PDF::API2->new(-file => $pdfFname);
$pdf->mediabox(595,842);

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
    my $data = \@col1Values;
    my $page = $pdf->page;
    my $txt = $page->text;

    my $fnt = $pdf->corefont('Arial',-encoding => 'latin1'); 
    $txt->textstart;
    $txt->font($fnt, 20);
    $txt->textend;
    close $info;
}
$pdf->saveas("$pdfFname");
$pdf->end();

# Process files that are inputs and outputs 

# Add a table to the existing PDF 
sub addTableToPDF {
    $pdf = $_[0];
    $page = $_[1]
    $titleRow = $_[1];
    $valueRows = $_[2];

    my $pdfTable = new PDF::Table;

    # table Variables
    my $xOfUpperLeftCorner;
    my $tableWidth;
    my $y
    ($end_page, $pages_spanned, $table_bot_y) = $pdftable->table($pdf, $page, $data,
		     x => $xOfUpperLeftCorner, 
		     w => $tableWidth,
		     start_y => $yPosFirstPage, next_y => $yPosNewPage,
		     start_h => $heightFirstPage, next_h=> $heightNewPage,
		     # OPTIONAL PARAMS
		     max_word_length=>$maxWordLength, # add a space after $MaxWordLength symbols  
		     padding=>$cellPadding, padding_top =>$cellTopPadding, 
		     padding_right=>$cellRightPadding, padding_left=>$cellLeftPadding,
		     padding_bottom=>$cellBottomPadding,
		     border=>$borderSize, border_color=>$borderColor,
		     horizontal_borders=>$horizontalBorders, # defaults to 1
		     vertical_borders=>$verticalBorders, # defaults to 1
		     font=>$pdf->corefont("$tableFont", -encoding => "utf8"),
		     font_size=>$tableFontSize, font_color_odd=>$tableOddFontColor, font_color_even=>$tableEvenFontColor,
		     background_color_odd=>$tableBackgroundColorOdd, background_color_even=>$tableBackgroundColorEven,
		     new_page_func => $codeRef,
		     header_props=> $hdrProps,
		     column_props=> $colProps,
		     cell_props=> $rowProps);
		     
	# some optional params
	padding => 5,
	padding_right => 10,
	background_color_odd  => "gray",
	background_color_even => "lightblue", #cell background color for even rows
	);

    my $pdftable = new PDF::Table;    
}

