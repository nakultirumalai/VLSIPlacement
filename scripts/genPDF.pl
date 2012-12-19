#!/usr/bin/perl -w

# Packages to use
use PDF::API2;
use PDF::Table;
use PDF::TextBlock;

#use strict; 
#use warnings FATAL => 'all';

# Global variables for table properties
my $pageHeight = 800;
my $pageWidth = 550;
my $spaceBetweenObjs = 20;
my $topMargin = 20;
my $botMargin = 20;
my $numObjsFirstPage=0;
my $totalNumPages=1;

# PDF file variables
# For each of the sorted files,
# read the table and write it out to PDF
#my $pdfFname = $benchmarkName . "NumInputOutputData" . ".pdf";
my $pdfFname = "out.pdf";
my $pdf  = PDF::API2->new(-file => $pdfFname);
$pdf->mediabox('Letter');
my $page = $pdf->page;
my $tableYStart = $pageHeight;
my $numPages;

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
	repeat => 0,
    };

    # Table spanning function
    # This parameter must be a function reference which when
    # executed will create  a new page and return the object 
    # back to the module.
    # The returned page must be a page object created with the 
    # the PDF::API2 page method();
    # my $codeRef = $pdf->page;

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
    
    # If the next table Y start is less than 
    # botMargin + minCellHeight add another page
    my $bottomSpaceForTwoRows = 40;
    my $leverage = $botMargin + $spaceBetweenObjs + $bottomSpaceForTwoRows;
    if ($beginYPos <= $leverage) {
	$page = $pdf->page;
	$totalNumPages++;
	$numObjsFirstPage=0;
	$beginYPos = $pageHeight;
    }
    my $imgYPos = $beginYPos;
    my $imgPage = $page;

    # table Variables
    my $xOfUpperLeftCorner = 50;
    my $tableWidth = 200;
    my $yPosFirstPage;# Initial Y position on the first page
    if ($numObjsFirstPage == 0) {
	# While drawing the first table
	$yPosFirstPage = $beginYPos - $topMargin;
    } else {
	# While drawing other tables in the new page
	$yPosFirstPage = $beginYPos - $spaceBetweenObjs;
    }
    # Question of num objects for a new page does not arise
    # as the new page itself is being created by the table
    # subroutine.
    my $yPosNewPage = $pageHeight - $topMargin;

    my $heightFirstPage; # Height of the table in the first page;
    if ($numObjsFirstPage == 0) {
	# While drawing the first table
	$heightFirstPage = $beginYPos - $topMargin -$botMargin;
    } else {
	# While drawing other tables in the new page
	$heightFirstPage = $beginYPos - $spaceBetweenObjs - $botMargin;
    }

    # Question of num objects for a new page does not arise
    # as the new page itself is being created by the table
    # subroutine.
    $heightNewPage = $pageHeight - $topMargin - $botMargin; 

    my $maxWordLength = 20; # Maximum number of symbols after which a space should be inserted
    my $cellPadding = 5; # Padding on all sides of a cell;
    my $cellTopPadding = 5; # Top padding of all cells. Setting this over-rides the original cell padding value
    my $cellLeftPadding = 5; # Top padding of all cells. Setting this over-rides the original cell padding value
    my $cellRightPadding = 5; #Top padding of all cells. Setting this over-rides the original cell padding value
    my $cellBottomPadding = 5; # Top padding of all cells. Setting this over-rides the original cell padding value
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
    my $tableOddBackgroundColor = 'lightred'; # Odd row background color
    my $tableEvenBackgroundColor= 'lightblue'; # Even row background color
    my $font = $pdf->corefont($tableFontName, -encoding => "utf8");
    my $colProps = [ 
	{
	    min_w => 20,
	    max_w => 20,
	},
	];
	
    my $pdftable = new PDF::Table;
    my ($end_page, $pages_spanned, $table_bot_y) = $pdftable->table($pdf, $page, $data,
		     x => $xOfUpperLeftCorner, 
		     w => $tableWidth,
		     start_y => $yPosFirstPage, next_y => $yPosNewPage,
		     start_h => $heightFirstPage, next_h=> $heightNewPage,
		     # OPTIONAL PARAMS
		     font=>$font,
 		     padding=>$cellPadding,
		     font_size=>$tableFontSize, font_color_odd=>$tableOddFontColor, font_color_even=>$tableEvenFontColor,
		     background_color_odd=>$tableOddBackgroundColor, background_color_even=>$tableEvenBackgroundColor,
		     header_props=> $hdrProps,
		     column_props=> $colProps,
	);
    # Add optional params for the table if required:
    # max_word_length=>$maxWordLength, # add a space after $MaxWordLength symbols  
    # padding=>$cellPadding, padding_top =>$cellTopPadding, 
    # padding_right=>$cellRightPadding, padding_left=>$cellLeftPadding,
    # padding_bottom=>$cellBottomPadding,
    # border=>$borderSize, border_color=>$borderColor,
    # horizontal_borders=>$horizontalBorders, # defaults to 1
    # vertical_borders=>$verticalBorders, # defaults to 1
    # cell_props=> $cellProps,    
    # new_page_func => $codeRef,
    $numObjsFirstPage = 1;

    return ($end_page, $pages_spanned, $table_bot_y, $imgPage, $imgYPos);
}

# Add text block to existing PDF
sub addTxtBlockToPDF {
    $pdf = $_[0];
    $page = $_[1];
    $beginYPos = $_[2];
    $data = $_[3];

    my $bottomSpaceForTwoRows = 40;
    my $leverage = $botMargin + $spaceBetweenObjs + $bottomSpaceForTwoRows;
    if ($beginYPos <= $leverage) {
	$page = $pdf->page;
	$totalNumPages++;
	$numObjsFirstPage=0;
	$beginYPos = $pageHeight;
    } else {
	$beginYPos = $beginYPos - $spaceBetweenObjs;
    }    
    my $blockFontName = "Arial-bold";
    my $font = {
        b => PDF::TextBlock::Font->new({
	    pdf  => $pdf,
	    font => $pdf->corefont($blockFontName, -encoding => 'latin1'),
	    fillcolor => '#ff0000', 
				       }),
    };
    my $fontSize = 11;
    my $tb  = PDF::TextBlock->new({
	pdf  => $pdf,
	page => $page,
	y => $beginYPos,
	fonts => $font,
        fontsize => $fontSize,
    });

    $tb->text($data);
    
    my ($endw, $ypos, $overflow) = $tb->apply;    

    return ($page, $endw, $ypos, $overflow);
}

# sub routine to add an image to 
# a PDF file
sub addImgToPDF {
    $pdf = $_[0];
    $page = $_[1];
    $xPos = $_[2];
    $yPos = $_[3];
    $imgFileName = $_[4];

    my $image=$pdf->image_png($imgFileName);
    my $gfx=$page->gfx;
    $gfx->image($image, $xPos, ($yPos - $topMargin), ($pageWidth - $xPos), 200);
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


my $tableGap = 10;
my $count = 0;

print "Reading ".scalar(@sortedFileNames)." files for PDF creation\n";

foreach my $fname (@sortedFileNames) {
    my $fullName = $analysisDir . "/" . $fname;
    open my $info, $fullName or die "Could not open $fullName: $!";
    my ($col1Title, $col2Title, $title);
    my @col1Values;
    my ($total, $mean, $variance, $stdDev);
    my $numDataAdded = 0;
    my $graph = 0;
    my $titleRead = 0;

    while (my $line = <$info>) {
	if ($line =~ /^#TITLE[ \t]+(.*)/) { 
	    $title = $1;	    
	} elsif ($line =~ /^#GRAPH.*/) {
	    $graph = 1;
	} elsif ($titleRead == 0) {
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
	    $numDataAdded++;
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
    close $info;
    my $data;
    my ($endw, $overflow);
    my ($xPos, $yPos);
    my $imgYPos;
    my $imgPage;

    if (defined $title) {
	# Add the title text for the particular file
	($page, $endw, $tableYStart, $overflow, $img) = addTxtBlockToPDF($pdf, $page, $tableYStart, $title);    
    }
    
    $data = \@col1Values;
    # Add the table which has count vs param for the particular file
    ($page, $numPages, $tableYStart, $imgPage, $imgYPos) = addTableToPDF($pdf, $page, $tableYStart, $data, $imgPage, $imgYPos);

    if ($numDataAdded > 1) {
	# Add the statistical data 
	$data = "TOTAL COUNT = " . $total . "\n";
	$data = $data . "STD DEVIATION = " . $stdDev . "\n";
	$data = $data . "MEAN = " . $mean . "\n";
	$data = $data . "VARIANCE = " . $variance;
	($page, $endw, $tableYStart, $overflow) = addTxtBlockToPDF($pdf, $page, $tableYStart, $data);    
	
	# Generate the image using gnuplot and 
	@args = ("cp", $fullName, "./plot.dat");
	(system(@args) == 0) or die "copying source data file failed";
	@args = ("./plotscript.gpl");
	(system(@args) == 0) or die "system @args failed: $?";
	@args = ("cp", "./figure.png", $fullName . ".png");
	(system(@args) == 0) or die "copying result png file failed";
	
	# Add image to the PDF file
	my $imgFileName = "./figure.png";
	my $xPos = 270;
	my $yPos = $imgYPos;
	addImgToPDF($pdf, $imgPage, $xPos, $yPos, $imgFileName);
    }

    if ($numPages > 2) {
	# If the page span was more than 1, it means 
	# additional pages were added. If n pages is the page
	# span, then n - 1 additional pages were added to add the
	# tabular data.
	$totalNumPages += $numPages - 1;
    }
    $count++;
#    if ($count == 1) {
#	last;
#    }
}

print "Total of $count tables generated\n";
$pdf->saveas("$pdfFname");
$pdf->end();
