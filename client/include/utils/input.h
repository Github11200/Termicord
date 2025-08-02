#ifndef INPUT_H
#define INPUT_H

#include <iostream>
#include <spdlog/spdlog.h>
#include <string>
#include <type_traits>
#include <vector>
// #include <magic_enum/magic_enum.hpp>

using namespace std;

namespace Input {

void getStringInput(string message, string &input, int minimumLength = -1);

template <typename T> void getNumericInput(string message, T &input) {
  while (true) {
    string temp;
    cout << message;
    cin >> temp;
    if (is_integral<T>::value) {
      try {
        input = stoi(temp);
        return;
      } catch (const exception &e) {
        cout << "Invalid input. Please enter an integer." << endl;
      }
    } else if (is_floating_point<T>::value) {
      try {
        input = stod(temp);
        return;
      } catch (const exception &e) {
        cout << "Invalid input. Please enter a floating point value." << endl;
      }
    }
  }
}

template <typename T>
void getBoundedNumericInput(string message, T &input, T lower, T upper) {
  while (true) {
    getNumericInput<T>(message, input);
    if (input > upper || input < lower)
      cout << "Invalid input. The input must be between " + to_string(lower) +
                  " and " + to_string(upper) + "."
           << endl;
    else
      return;
  }
}

template <typename Enum>
void getOption(const string &message, const vector<string> &options,
               Enum &input) {
  try {
    cout << message << endl;
    // Print out all the optoins
    for (int i = 1; i <= options.size(); ++i)
      cout << "  (" << to_string(i) << ") " << options[i - 1] << endl;
    int numberInput = 0;
    Input::getBoundedNumericInput<int>("Input: ", numberInput, 1,
                                       options.size());

    // Once the user inputs a number map it to the enum values
    input = static_cast<Enum>(numberInput);
  } catch (const exception &e) {
    cout << "Error: " << e.what() << endl;
  }
}

} // namespace Input

#endif
