/************************************************
 **
 ** Implementation of Force-Directed Placement.
 ** 
 ** Authors: Shaun K. Peter (M04326446)
 **          Kishan Sarpangala (M04307966)
 ** 
 ** Date: 2/22/2012
 **
 ************************************************/

#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<vector>
#include<map>
#include<string.h>
#include<algorithm>
#include<stdio.h>
#include<math.h>
#include<time.h>

using namespace std;
typedef vector< map<int,int>* > Matrix;

#define CELLSIZE 6

class Cell {   //The basic datastructure which represents a cell

public:
  int cellNumber;   //The cell number
  int x;
  int y;
  int numNets;
  bool isLocked;

  Cell(int cellNum) : cellNumber(cellNum) {
    x = y = 0;
    numNets = 0;
    isLocked = false;
  }
};

// Custom comparison function for sort
bool cellCompare(Cell *c1, Cell *c2) {
  return (c1->numNets > c2->numNets);
}

class ConnectivityMatrix {  //Datastructure having the connectivity information

  Matrix theMatrix;

public:
  void readBenchmark(int&, int&);
  void updateNumNets(Cell*&);
  void getNewLocation(int, vector<Cell*>&, int&, int&);
  void estimateWirelength(vector<Cell*>&, int&);
};

void clearLocks(vector<Cell*>&);
void printMagic(vector<Cell*>&);

// The main program
int main(int argc, char *argv[]) {

  clock_t start_t, elapsed_t;
  double elapsed_time;
  start_t = clock();

  vector<Cell*> cellList;
  vector<Cell*> sortedCellList;
  ConnectivityMatrix C;
  vector< vector<int> > gridMatrix; 

  int numCells, numNets;
  // Read benchmark
  C.readBenchmark(numCells, numNets);

  // Get initial dimensions of placement area
  int dimension = ceil( sqrt(numCells) );
  gridMatrix.resize(dimension);
  for(int i=0; i<dimension; i++) {
    gridMatrix[i].resize(dimension,0);
  }

  Cell *tempCell;
  int index1, index2;
  cellList.push_back(NULL);
  for(int i=1; i<=numCells; i++) {
    tempCell = new Cell(i);
    C.updateNumNets(tempCell);
    cellList.push_back(tempCell);
    sortedCellList.push_back(tempCell);
  }

  // Initial placement
  for(int i=1; i<=(numCells/2); i++) {
    index1 = (i-1) / dimension;
    index2 = (i-1) % dimension;
    gridMatrix[index1][index2] = i;
    tempCell = cellList[i];
    tempCell->y = index1;
    tempCell->x = index2;
  }
  index1 = index2 = dimension-1;
  for(int i=(numCells/2+1); i<=numCells; i++) {
    gridMatrix[index1][index2] = i;
    tempCell = cellList[i];
    tempCell->y = index1;
    tempCell->x = index2;
    if(index2>0)
      index2--;
    else {
      index1--;
      index2 = dimension-1;
    }
  }

  // Sort cells in decreasing order of connectivity
  stable_sort(sortedCellList.begin(), sortedCellList.end(), cellCompare);
 
  int wirelength; 
  C.estimateWirelength(cellList, wirelength);
  cout<<"\nInitial estimated wirelength = "<<wirelength<<endl;

  int iter_limit=2*cellList.size();
  int iter_count=0;
  unsigned int abort_limit=cellList.size()*3/4;
  unsigned int abort_count=0;
  unsigned int counter=0;
  // Start force directed placement
  while (iter_count < iter_limit) {

    if(counter>=sortedCellList.size())
      counter = 0;
    tempCell = sortedCellList[counter++];
    if(tempCell->isLocked)
      continue;
    int y = tempCell->y;
    int x = tempCell->x;
    gridMatrix[y][x] = 0;

    bool end_ripple=false;
    // Start ripple
    while(!end_ripple) {

      int cellNum = tempCell->cellNumber;
      //cout<<cellNum<<endl;
      int x, y;
      // Calculate correct location of cell
      C.getNewLocation(cellNum, cellList, y, x);

      if(x==-1 && y==-1) {
	//cout<<"No connectivity found"<<endl;
	end_ripple = true;
	int i, j, flag=0;
	for(i=0; i<dimension; i++) {
	  for(j=0; j<dimension; j++) {
	    if(gridMatrix[i][j]==0) {
	      gridMatrix[i][j] = cellNum;
	      tempCell->x = j;
	      tempCell->y = i;
	      flag = 1;
	      break;
	    }
	  }
	  if(flag==1)
	    break;
	}
      }
      else {
	int cellNum2 = gridMatrix[y][x];
	//cout<<cellNum2<<endl;

	if(cellNum2==0) {
	  //cout<<"Empty location found"<<endl;
	  end_ripple = true;
	  abort_count = 0;
	  gridMatrix[y][x] = cellNum;
	  tempCell->x = x;
	  tempCell->y = y;
	  tempCell->isLocked = true;
	}
	else if(cellList[cellNum2]->isLocked) {
	  //cout<<"Location is locked"<<endl;
	  end_ripple = true;
	  abort_count++;
	  int i, j, flag=0;
	  for(i=0; i<dimension; i++) {
	    for(j=0; j<dimension; j++) {
	      if(gridMatrix[i][j]==0) {
		gridMatrix[i][j] = cellNum;
		tempCell->x = j;
		tempCell->y = i;
		flag = 1;
		break;
	      }
	    }
	    if(flag==1)
	      break;
	  }

	  if(abort_count>=abort_limit) {
	    clearLocks(cellList);
	    //cout<<"Locks cleared"<<endl;
	    iter_count++;
	    counter = 0;
            abort_limit = abort_limit*3/4;
            if(abort_limit < cellList.size()/2)
              abort_limit = cellList.size()*3/4;
	  }
	}
	else {
	  //cout<<"Found occupied but not locked"<<endl;
	  abort_count = 0;
	  gridMatrix[y][x] = cellNum;
	  tempCell->x = x;
	  tempCell->y = y;
	  tempCell->isLocked = true;
	  tempCell = cellList[cellNum2];
	}
      }
    }
  }

  C.estimateWirelength(cellList, wirelength);
  cout<<"Final estimated wirelength = "<<wirelength<<endl;

  // Print magic file
  printMagic(cellList);

  // Print memory and time
  ifstream statStream;
  statStream.open("/proc/self/stat");
  string statField;
  unsigned long vmsize;
  long rss, pageSize_KB;
  for(int i=0; i<22; i++)
    statStream >> statField;
  statStream >> vmsize;
  statStream >> rss;
  // Assume 64-bit machine. Use 1024 for 32-bit machine  
  pageSize_KB = sysconf(_SC_PAGE_SIZE)/2048;
  cout<<"\nTotal memory used: "<<(rss*pageSize_KB + vmsize/2048)<<" KB"<<endl;

  elapsed_t = clock()-start_t;
  elapsed_time = elapsed_t/((double)CLOCKS_PER_SEC);
  cout<<"Time elapsed: "<<elapsed_time<<" seconds"<<endl;

  return 0;
}


// Read the benchmark and populate the connectivity matrix
void ConnectivityMatrix::readBenchmark(int& numC, int& numN) {
  int netNum, cellNum1, termNum1, cellNum2, termNum2;
  cin>>numC>>numN;
  theMatrix.resize(numC+1,NULL);

  map<int,int>::iterator iter;

  while(1) {
    cin >> netNum >> cellNum1 >> termNum1 >> cellNum2 >> termNum2;
    if(cin.eof())
      break;

    if(cellNum1==cellNum2)
      continue;   

    if(theMatrix[cellNum1]) {
      iter = theMatrix[cellNum1]->find(cellNum2);
      if( iter!=theMatrix[cellNum1]->end() ) {
	(iter->second)++;
      }
      else {
	theMatrix[cellNum1]->insert(pair<int,int>(cellNum2,1));
      }
    }
    else {
      theMatrix[cellNum1] = new map<int,int>;
      theMatrix[cellNum1]->insert(pair<int,int>(cellNum2,1));
    }

    if(theMatrix[cellNum2]) {
      iter = theMatrix[cellNum2]->find(cellNum1);
      if( iter!=theMatrix[cellNum2]->end() ) {
	(iter->second)++;
      }
      else {
	theMatrix[cellNum2]->insert(pair<int,int>(cellNum1,1));
      }
    }
    else {
      theMatrix[cellNum2] = new map<int,int>;
      theMatrix[cellNum2]->insert(pair<int,int>(cellNum1,1));
    }
  }
}

void ConnectivityMatrix::updateNumNets(Cell*& cell1) {

  int i1 = cell1->cellNumber;
  cell1->numNets = 0;
  map<int,int> *tempMap = theMatrix[i1];
  if(tempMap) {
    map<int,int>::iterator it = tempMap->begin();
    while(it!=tempMap->end()) {
      cell1->numNets += it->second;
      it++;
    }
  }
} 

void ConnectivityMatrix::getNewLocation(int cellNum, vector<Cell*>& cellList, int& y, int& x) {

  map<int,int> *tempMap;
  map<int,int>::iterator it;
  tempMap = theMatrix[cellNum];
  Cell* tempCell;
  float xSum=0.0, ySum=0.0;
  //cout<<cellNum<<endl;

  if(tempMap) {
    it = tempMap->begin();
    int cellNum2;
    float weight, totWeight=0;
    while(it!=tempMap->end()) {
      cellNum2 = it->first;
      weight = it->second;
      totWeight += weight;
      tempCell = cellList[cellNum2];
      xSum += (tempCell->x) * weight;
      ySum += (tempCell->y) * weight;
      //cout<<tempCell->cellNumber<<" ("<<tempCell->y<<","<<tempCell->x<<")"<<endl;
      it++;
    }
    ySum = ySum / totWeight;
    xSum = xSum / totWeight;

    double fractpart, intpart;
    fractpart = modf(ySum, &intpart);
    if(fractpart<=0.5)
      y = intpart;
    else
      y = intpart+1;

    fractpart = modf(xSum, &intpart);
    if(fractpart<=0.5)
      x = intpart;
    else
      x = intpart+1;

    //cout<<"New location = "<<y<<","<<x<<endl;
  } 
  else {
    x = -1;
    y = -1;
  }
}


void clearLocks(vector<Cell*>& cellList) {

  for(unsigned int i=1; i<cellList.size(); i++)
    cellList[i]->isLocked = false;
}

void ConnectivityMatrix::estimateWirelength(vector<Cell*>& cellList, int& wirelength) {

  int tot_x=0, tot_y=0;
  map<int,int> *tempMap;
  for(unsigned int i=1; i<theMatrix.size(); i++) {
    tempMap = theMatrix[i];
    Cell* cell1 = cellList[i];
    int x1 = cell1->x;
    int y1 = cell1->y;
    if(tempMap) {
      map<int,int>::iterator it = tempMap->begin();
      Cell *cell2;
      while(it!=tempMap->end()) {
	cell2 = cellList[it->first];
	tot_x += abs(cell2->x - x1) * it->second;
	tot_y += abs(cell2->y - y1) * it->second;
	it++;
      }
    }
  }
  wirelength = (tot_x+tot_y)*7/2;
}


void printMagic(vector<Cell*>& cellList) {

  ofstream outfile;
  outfile.open("placement.mag");
  outfile<<"magic"<<endl;

  outfile<<"<< polysilicon >>"<<endl;
  Cell* tempCell;
  int y, x;
  for(unsigned int i=1; i<cellList.size(); i++) {
    tempCell = cellList[i];
    y = tempCell->y;
    x = tempCell->x;
    outfile<<"rect "<<((x*CELLSIZE)+x)<<" "<<((y*CELLSIZE)+y)<<" "<<(((x+1)*CELLSIZE)+x)<<" "<<(((y+1)*CELLSIZE)+y)<<endl;
  }

  outfile<<"<< labels >>"<<endl;
  for(unsigned int i=1; i<cellList.size(); i++) {
    tempCell = cellList[i];
    y = tempCell->y;
    x = tempCell->x;
    outfile<<"rlabel polysilicon "<<(x*(CELLSIZE+1)+CELLSIZE/2)<<" "<<(y*(CELLSIZE+1)+CELLSIZE/2)<<" "<<(x*(CELLSIZE+1)+CELLSIZE/2)<<" "<<(y*(CELLSIZE+1)+CELLSIZE/2)<<" 5 "<<i<<endl;
  }
  outfile<<"<< end >>"<<endl;
  outfile.close();

}
