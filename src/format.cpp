#include <string>

#include "format.h"

using std::string;
using std::to_string;

string Format::FormatDigit(int num) {
  if (num % 10 == num) return "0" + to_string(num);
  return to_string(num);
}

string Format::ElapsedTime(long seconds) {
  constexpr int secInHour = 3600;
  constexpr int secInMin = 60;
  int hours = seconds / secInHour;
  seconds -= hours * secInHour;

  int minutes = seconds / secInMin;
  seconds -= minutes * secInMin;

  return FormatDigit(hours) + ":" + FormatDigit(minutes) + ":" + FormatDigit(seconds);
}