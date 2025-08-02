#include "utils/input.h"

using namespace std;

namespace Input {

void getStringInput(string message, string &input, int minimumLength) {
  while (true) {
    cout << message;
    cin >> input;
    if (minimumLength != -1 && input.size() < minimumLength)
      cout << "The input is too short, it must be a minimumm of " +
                  to_string(minimumLength)
           << " characters." << endl;
    else
      break;
  }
}

} // namespace Input
