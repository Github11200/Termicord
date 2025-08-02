#include "utils/utils.h"

using namespace std;

vector<string> splitString(string input, char delimeter) {
  stringstream ss(input);
  string line;
  vector<string> lines;

  while (getline(ss, line))
    lines.push_back(line);

  return lines;
}

int randomInteger(int min, int max) {
  srand(time(0));
  return min + (rand() % (max - min + 1));
}
