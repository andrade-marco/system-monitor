#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// Constructor
Process::Process(int pid): pid{pid} {};

int Process::Pid() {
  return this->pid;
}

float Process::CpuUtilization() const {
  auto process_active = LinuxParser::ActiveJiffies(this->Pid());
  auto system_active = LinuxParser::ActiveJiffies();
  return process_active / system_active;
}

string Process::Command() {
    return LinuxParser::Command(this->Pid());
}

string Process::Ram() {
  return LinuxParser::Ram(this->Pid());
}

string Process::User() {
  return LinuxParser::User(this->Pid());
}

long int Process::UpTime() {
  return LinuxParser::UpTime(this->Pid());
}

bool Process::operator<(Process const& a) const {
  return this->CpuUtilization() > a.CpuUtilization();
}