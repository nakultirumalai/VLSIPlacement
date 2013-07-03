#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class LookupTable {
  vector<double> index1;
  vector<double> index2;
  vector<double> value;  
  
 public:

  void insertIndex1(double idx) {
    index1.push_back(idx);
  }
  void insertIndex2(double idx) {
    index2.push_back(idx);
  }
  void insertValue(double val) {
    value.push_back(val);
  }

  double getValueWithoutInterpolation(double idx1, double idx2) {
    vector<double>::iterator it;
    int i2, j2;
    it = lower_bound(index1.begin(), index1.end(), idx1); //returns the first element that is not less than idx1
    if(it==index1.end()) {
      //cout<<"Index1 "<<idx1<<" out of range. Using default value of 0"<<endl;
      return 0;
    }
    i2 = it - index1.begin();
    if(i2==0) {
      //cout<<"Index1 "<<idx1<<" out of range. Using default value of 0"<<endl;
      return 0;
    }
    it = lower_bound(index2.begin(), index2.end(), idx2);
    if(it==index2.end()) {
      //cout<<"Index2 "<<idx2<<" out of range. Using default value of 0"<<endl;
      return 0;
    }
    j2 = it - index2.begin();
    if(j2==0) {
      //cout<<"Index2 "<<idx2<<" out of range. Using default value of 0"<<endl;
      return 0;
    }

    int rowSize = index1.size();
    return value[i2*rowSize+j2];
  }

  double getValue(double idx1, double idx2) {
    vector<double>::iterator it;
    int i1, j1, i2, j2;
    it = lower_bound(index1.begin(), index1.end(), idx1); //returns the first element that is not less than idx1
    if(it==index1.end()) {
      //cout<<"Index1 "<<idx1<<" out of range. Using default value of 0"<<endl;
      return 0;
    }
    i2 = it - index1.begin();
    if(i2==0) {
      //cout<<"Index1 "<<idx1<<" out of range. Using default value of 0"<<endl;
      return 0;
    }
    it = lower_bound(index2.begin(), index2.end(), idx2);
    if(it==index2.end()) {
      //cout<<"Index2 "<<idx2<<" out of range. Using default value of 0"<<endl;
      return 0;
    }
    j2 = it - index2.begin();
    if(j2==0) {
      //cout<<"Index2 "<<idx2<<" out of range. Using default value of 0"<<endl;
      return 0;
    }
    i1 = i2-1;
    j1 = j2-1;

    //Bilinear interpolation
    double fq11, fq12, fq21, fq22, fr1, fr2, fp;
    double x1, x2, y1, y2, dx, dy;
    x1 = index1[i1];
    x2 = index1[i2];
    y1 = index2[j1];
    y2 = index2[j2];
    dx = x2-x1;
    dy = y2-y1;
    int rowSize = index1.size();
    fq11 = value[i1*rowSize+j1];
    fq12 = value[i1*rowSize+j2];
    fq21 = value[i2*rowSize+j1];
    fq22 = value[i2*rowSize+j2];

    fr1 = ((x2-idx1)/dx)*fq11 + ((idx1-x1)/dx)*fq21;
    fr2 = ((x2-idx1)/dx)*fq12 + ((idx1-x1)/dx)*fq22;
    fp = ((y2-idx2)/dy)*fr1 + ((idx2-y1)/dy)*fr2;

    return fp;
  }

};
