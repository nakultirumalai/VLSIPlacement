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
  map<unsigned int, unsigned long>widthRanges;
  map<unsigned int, unsigned long>areaRanges;
  map<unsigned int, unsigned long>heightRanges;

  unsigned int areaRange[17];
  string areaRangeStr[17];
  
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
    outputFile.open("DesignCellWidthAnalysis.txt");
    unsigned int count;
    MAP_FOR_ALL_ELEMS(widthRanges, unsigned int, unsigned int, width, count) {
      outputFile << count << "   " << width << endl;
    } END_FOR;
    outputFile.close();
  }

  {
    ofstream outputFile;
    outputFile.open("DesignCellHeightAnalysis.txt");
    unsigned int count;
    MAP_FOR_ALL_ELEMS(heightRanges, unsigned int, unsigned int, height, count) {
      outputFile << count << "   " << height << endl;
    } END_FOR;
    outputFile.close();
  }

  {
    ofstream outputFile;
    outputFile.open("DesignCellAreaAnalysis.txt");
    unsigned int count;
  /* Have 20 ranges 
     0-100
     100-500
     500-1000
     1000-2000
     2000-3000
     3000-4000
     4000-5000
     5000-6000
     6000-7000
     7000-8000
     8000-9000
     9000-10000
     10000-50000
     50000-100000
     100000-1000000
     1000000+
   */
    areaRangeStr[0] = "0-100";
    areaRangeStr[1] = "100-500";
    areaRangeStr[2] = "500-1000";
    areaRangeStr[3] = "1000-2000";
    areaRangeStr[4] = "2000-3000";
    areaRangeStr[5] = "3000-4000";
    areaRangeStr[6] = "4000-5000";
    areaRangeStr[7] = "5000-6000";
    areaRangeStr[8] = "6000-7000";
    areaRangeStr[9] = "7000-8000";
    areaRangeStr[10] = "8000-9000";
    areaRangeStr[11] = "9000-10000";
    areaRangeStr[12] = "10000-50000";
    areaRangeStr[13] = "50000-100000";
    areaRangeStr[14] = "100000-500000";
    areaRangeStr[15] = "500000-1000000";
    areaRangeStr[16] = "1000000+";

    MAP_FOR_ALL_ELEMS(areaRanges, unsigned int, unsigned int, area, count) {
      if (area > 0 && area <= 100) {
	areaRange[0] += count;
      } else if (area > 100 && area <= 500) {
	areaRange[1] += count;
      } else if (area > 500 && area <= 1000) {
	areaRange[2] += count;
      } else if (area > 1000 && area <= 2000) {
	areaRange[3] += count;
      } else if (area > 2000 && area <= 3000) {
	areaRange[4] += count;
      } else if (area > 3000 && area <= 4000) {
	areaRange[5] += count;
      } else if (area > 4000 && area <= 5000) {
	areaRange[6] += count;
      } else if (area > 5000 && area <= 6000) {
	areaRange[7] += count;
      } else if (area > 6000 && area <= 7000) {
	areaRange[8] += count;
      } else if (area > 7000 && area <= 8000) {
	areaRange[9] += count;
      } else if (area > 8000 && area <= 9000) {
	areaRange[10] += count;
      } else if (area > 9000 && area <= 10000) {
	areaRange[11] += count;
      } else if (area > 10000 && area <= 50000) {
	areaRange[12] += count;
      } else if (area > 50000 && area <= 100000) {
	areaRange[13] += count;
      } else if (area > 100000 && area <= 500000) {
	areaRange[14] += count;
      } else if (area > 500000 && area <= 1000000) {
	areaRange[15] += count;
      } else if (area > 1000000) {
	areaRange[16] += count;
      }
    } END_FOR;
    for (int i=0; i < 17; i++) {
      outputFile << i << "   " << areaRangeStr[i] << "  " << areaRange[i] << endl;
    }
    outputFile.close();
  }
}

