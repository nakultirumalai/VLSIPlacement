/****** 
FUNCTIONS TO CONVERT THE DESIGN THAT HAS 
BEEN READ TO A HYPERGRAPH 
******/
# include <Design.h>
# include <DesignIter.h>
# include <Cell.h>

/**************/
void DesignWriteDesignStats(Design& myDesign)
{
  Cell *CellPtr;
  string Name;
  unsigned int width, height, area, max_area, max_width, max_height;
  map<unsigned int, unsigned int>widthRanges;
  map<unsigned int, unsigned int>areaRanges;
  map<unsigned int, unsigned int>heightRanges;

  max_area = 0;
  max_height = 0;
  max_width = 0;
  /* First add all the nodes */
  DESIGN_FOR_ALL_CELLS(myDesign, Name, CellPtr) {
    width = (*CellPtr).CellGetWidth();
    height = (*CellPtr).CellGetHeight();
    area = (*CellPtr).CellGetArea();
    if (area > max_area) max_area = area;
    if (width > max_width) max_width = width;
    if (height > max_height) max_height = height;


    if (widthRanges.find(width) != widthRanges.end()) {
      widthRanges[width]++;
    } else {
      widthRanges[width] = 1;
    }

    if (areaRanges.find(area) != areaRanges.end()) {
      areaRanges[area]++;
    } else {
      areaRanges[area] = 1;
    }

    if (heightRanges.find(height) != heightRanges.end()) {
      heightRanges[height]++;
    } else {
      heightRanges[height] = 1;
    }

  } DESIGN_END_FOR;

  {
    ofstream outputFile;
    outputFile.open("DesignCellAreaAnalysis.txt");
    unsigned int count;
    for (int i=0;i <= max_area; i++) {
      if (areaRanges.find(i) != areaRanges.end()) {
	count = areaRanges[i];
	outputFile << i << "   " << count << endl;
      } else {
	outputFile << i << "   " << "0" << endl;	
      }
    }
    outputFile.close();
  }
  {
    ofstream outputFile;
    outputFile.open("DesignCellWidthAnalysis.txt");
    unsigned int count;
    for (int i=0;i <= max_width; i++) {
      if (widthRanges.find(i) != widthRanges.end()) {
	count = widthRanges[i];
	outputFile << i << "   " << count << endl;
      } else {
	outputFile << i << "   " << "0" << endl;	
      }
    }
    outputFile.close();
  }
  {
    ofstream outputFile;
    outputFile.open("DesignCellHeightAnalysis.txt");
    unsigned int count;
    for (int i=0;i <= max_height; i++) {
      if (heightRanges.find(i) != heightRanges.end()) {
	count = heightRanges[i];
	outputFile << i << "   " << count << endl;
      } else {
	outputFile << i << "   " << "0" << endl;	
      }
    }
    outputFile.close();
  }
  
# if 0
  {
    ofstream outputFile;
    outputFile.open("DesignCellWidthAnalysis.txt");
    unsigned int count;
    MAP_FOR_ALL_ELEMS(widthRanges, unsigned int, unsigned int, width, count) {
      outputFile << width << "   " << count << endl;
    } END_FOR;
    outputFile.close();
  }

  {
    ofstream outputFile;
    outputFile.open("DesignCellHeightAnalysis.txt");
    unsigned int count;
    MAP_FOR_ALL_ELEMS(heightRanges, unsigned int, unsigned int, height, count) {
      outputFile << height << "   " << count << endl;
    } END_FOR;
    outputFile.close();
  }

  {
    ofstream outputFile;
    outputFile.open("DesignCellAreaAnalysis.txt");
    unsigned int count;
    MAP_FOR_ALL_ELEMS(areaRanges, unsigned int, unsigned int, area, count) {
      outputFile << area << "   " << count << endl;
    } END_FOR;
    outputFile.close();
  }
# endif
}

