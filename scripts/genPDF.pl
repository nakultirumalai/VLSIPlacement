#!/usr/bin/perl -w

# Packages to use
use PDF::API2;
use PDF::Table;
#use strict; 
#use warnings FATAL => 'all';
@INC{"Table.pm"};

sub addTableToPDF {
    $pdf = $_[0];
    $page = $_[1];
    $beginYPos = $_[2];
    $data = $_[3];


    my $pdfTable = new PDF::Table;

    # Special properties of the table #
    # header properties. First row of every tableis the header
    # is a reference to a hash

    my $headerFontName = "Helvetica-bold";
    my $hdrProps = 
    {
	font => $pdf->corefont($headerFontName, -encoding => "utf8"),
	font_size => 11,
	bg_color => 'darkgray',
	repeat => 1,
    };

    # Table spanning function
    # This parameter must be a function reference which when
    # executed will create  a new page and return the object 
    # back to the module.
    # The returned page must be a page object created with the 
    # the PDF::API2 page method();
    my $codeRef = $pdf->page;

    # Properties of columns
    # Reference to array of hashes
    #my $colProps = [ {},];
    # Example is as follows:
    #    [
    #     {}, # First column is empty
    #     {
    #	    min_w => 100, # Minimum column width
    #	    max_w => 150, # Maximum column width
    #	    justify => 'right', # One of left | right | center 
    #	    font => $pdf->corefont($tableFontName, -encoding => "utf8"),
    #	    font_size => 10,
    #	    font_color => 'black',
    #	    background_color => '#FFFFFF',
    #     },
    #     #etc
    #    ];
    # Specifying a column property overwrites the table property for that
    # particular column
    
    # Individual cell properties
    # In case of a conflict between column, odd/even and cell formating, 
    # cell formating will overwrite the other two. In case of a conflict 
    # between header row cell formating, header formating will win.
    # It is an array reference with arrays of hash references
    my $cellProps = [ [ {},],];
    # Example:
    #  [
    #   [
    #    {
    # Row1 cell1
    #     background_color => '#FFFFFF',
    #     font_color       => 'black',
    #    },
    #    {
    # Row1 cell2
    #    },
    #   ],
    #   [,
    #   {},
    #   ],
    # ];
    
    my $end_page;
    my $pages_spanned;
    my $table_bot_y;

    # table Variables
    my $xOfUpperLeftCorner = 20;
    my $tableWidth = 500;
    my $yPosFirstPage = $beginYPos; # Initial Y position on the first page
    my $yPosNewPage = 800; # Y position on every successive page
    my $heightFirstPage = 500; # Height of the table;
    my $heightNewPage = 500; # Height of the table in every new page;
    my $maxWordLength = 20; # Maximum number of symbols after which a space should be inserted
    my $cellPadding = 2; # Padding on all sides of a cell;
    my $cellTopPadding = 2; # Top padding of all cells. Setting this over-rides the original cell padding value
    my $cellLeftPadding = 2; # Top padding of all cells. Setting this over-rides the original cell padding value
    my $cellRightPadding = 2; #Top padding of all cells. Setting this over-rides the original cell padding value
    my $cellBottomPadding = 2; # Top padding of all cells. Setting this over-rides the original cell padding value
    my $borderSize = 1; # default is 1, use 0 for no borders
    my $borderColor = 'black'; # Indicates what color the table border should be;
                               # Select various colors from 'red', 'black', 'yellow', 'gray', 
                               # Color can also be specified in the HTML RGB format "#RRGGBB"
    my $horizontalBorders = 1; # defaults to 1, use 0 for no horizontal borders
    my $verticalBorders = 1; # defaults to 1, use 0 for no vertical borders
    my $tableFontName = "Helvetica"; # Font to set
                                 # Use the following font names: Courier, Courier-Bold, Courier-BoldOblique, Courier-Oblique, Helvetica
                                 # Helvetica-Bold, Helvetica-BoldOblique, Helvetica-Oblique, Symbol, Times-Bold, Times-BoldItalic, Times-Italic
                                 # Times-Roman, ZapfDingbats
    my $tableFontSize = 11; # Font size of the table
    my $tableOddFontColor = 'black'; # Odd row font color
    my $tableEvenFontColor = 'black'; # Even row font color
    my $tableOddBackgroundColor = 'lightblue'; # Odd row background color
    my $tableEvenBackgroundColor= 'lightgray'; # Even row background color
    my $font = $pdf->corefont($tableFontName, -encoding => "utf8");
    my $colProps = [ 
	{
	    min_w => 20,
	    max_w => 20,
	},
	];
	
    my $pdftable = new PDF::Table;
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
		     font=>$font,
		     font_size=>$tableFontSize, font_color_odd=>$tableOddFontColor, font_color_even=>$tableEvenFontColor,
		     background_color_odd=>$tableOddBackgroundColor, background_color_even=>$tableEvenBackgroundColor,
#		     new_page_func => $codeRef,
		     header_props=> $hdrProps,
		     column_props=> $colProps,
#		     cell_props=> $cellProps,
	);
    return ($end_page, $pages_spanned, $table_bot_y);
}

# Add text block to existing PDF
sub addTxtBlockToPDF {
    $pdf = $_[0];
    $page = $_[1];
    $data = $_[2];
    
    my $xOfUpperLeftCorner = 20; # Specify the coordinate from 0 of the 
                                 # left edge of the block
    my $yPosFirstLine = 20; # Specify the y position of the first line
    my $blockWidth = 100; # Width of the text block
    my $blockHeight = 500; # Height of the text block
    my $blockFontName = "Helvetica"; # Name of the font to be used in the block
    my $blockFontSize = 11; # Font size of the block 
    my $blockAlignment = "left"; # "left|right|center|justify|fulljustify"
    my $blockHangingIndent = 0; #

    my $blockParSpace = 5; # Optional vertical space before the first paragraph;
    my $blockFirstLineIndent; # Optional indent of the first line
    my $blockFirstParagraphIndent; # Optional indent of the first paragraph
    my $blockIndent = 0; # Optional indent of text to every non first line

    my $txt = $pdf->text;
    my $pdftable = new PDF::Table;

    my ($width_of_last_line, $ypos_of_last_line, $left_over_text);
    ($width_of_last_line, $ypos_of_last_line, $left_over_text) = $pdftable->text_block(
	$txt,
	$data,
	#X,Y - coordinates of upper left corner
	x        => $xOfUpperLeftCorner,	
	y        => $yPosFirstLine,
        w        => $blockWidth,
        h        => $blockHeight,

        #OPTIONAL PARAMS
	font     => $pdf->corefont($blockFontName, -encoding => "utf8"),
	lead     => $blockFontSize,
	align    => $blockAlignment,
	hang     => $blockHangingIndent,
	# Only one of the subsequent 3params can be given. 
	# They override each other.-parspace is the weightest
	parspace => $blockParSpace,
        #flindent => $optional_indent_of_first_line,
	#fpindent => $optional_indent_of_first_paragraph,
	indent   => $blockIndent,
	);
}


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
my $page = $pdf->page;

my $pagedata = [
    ["COUNT", "DATA", "COL3", "COL4", "COL5"],
    ["1", "2", "3", "4", "5"],
    ["3", "4", "5", "6", "7"],
    ];

my $numPages;
my $boty = 800;
my $tableGap = 10;
($page, $numPages, $boty) = addTableToPDF($pdf, $page, $boty, $pagedata);
$boty = $boty - $tableGap;
$pagedata = [
    ["COUNT", "MYCOL", "COL", "COL", "COL"],
    ["1", "2", "3", "4", "5"],
    ["3", "4", "5", "6", "7"],
    ];

($page, $numPages, $boty) = addTableToPDF($pdf, $page, $boty, $pagedata);
$boty = $boty - $tableGap;
$pagedata = [
    ["COUNT", "MYCOL", "COL", "COL", "COL"],
    ["1", "2", "3", "4", "5"],
    ["3", "4", "5", "6", "7"],
    ];

($page, $numPages, $boty) = addTableToPDF($pdf, $page, $boty, $pagedata);

#addTableToPDF($pdf, $page, $pagedata);
#addTableToPDF($pdf, $page, $pagedata);

$pdf->saveas("$pdfFname");
$pdf->end();

exit;
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
    my $txt = $page->text;

    my $fnt = $pdf->corefont('Arial',-encoding => 'latin1'); 
    $txt->textstart;
    $txt->font($fnt, 20);
    $txt->textend;
    close $info;
}

$pdf->saveas("$pdfFname");
$pdf->end();
