#include <string>

#include "format.h"

using std::string;
using std::to_string;

string Format::ElapsedTime(long seconds) {
  constexpr int secInHour = 3600;
  constexpr int secInMin = 60;
  int hours = seconds / secInHour;
  seconds -= hours * secInHour;

  int minutes = seconds / secInMin;
  seconds -= minutes * secInMin;

  return to_string(hours) + ":" + to_string(minutes) + ":" + to_string(seconds);
}