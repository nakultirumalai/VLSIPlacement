# include <iostream>
# include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
  string filename = argv[1];
  cout << endl;
  cout << filename;
  cout << endl;

  string line;
  int i, num_nodes;

  ifstream file (argv[1]);
  
  i = 0;
  while (!file.eof()) {
    getline(file, line);
    cout << line << endl;
    i++;
    if (i == 4) {
      break;
    }
  }
  
  file >> line;
  file >> num_nodes;
  file >> num_nodes;
  
  cout << line << endl << num_nodes << endl;
  
  return 0;
}

