#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  auto jiffies = LinuxParser::CpuUtilization();
  return 0.0;
}