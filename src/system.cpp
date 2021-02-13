#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System()
    : operatingSystem_{LinuxParser::OperatingSystem()},
      kernel_{LinuxParser::Kernel()},
      cpu_{Processor()}
{
  vector<Process> processes {};
  auto pids = LinuxParser::Pids();
  for (auto pid:pids) {
    processes.push_back(Process(pid));
  }

  this->processes_ = processes;
}

Processor& System::Cpu() { return this->cpu_; }

vector<Process>& System::Processes() { return this->processes_; }

std::string System::Kernel() { return this->kernel_; }

std::string System::OperatingSystem() { return this->operatingSystem_; }

float System::MemoryUtilization() {
  return LinuxParser::MemoryUtilization();
}

int System::RunningProcesses() {
  return LinuxParser::RunningProcesses();
}

int System::TotalProcesses() {
  return LinuxParser::TotalProcesses();
}

long int System::UpTime() {
  return LinuxParser::UpTime();
}