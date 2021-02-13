#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization() {
  auto idle = LinuxParser::IdleJiffies();
  auto active = LinuxParser::ActiveJiffies();

  return active / (active + idle);
}