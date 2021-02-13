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
Process::Process(int pid)
    : pid_{pid},
      user_{LinuxParser::User(pid)},
      command_{LinuxParser::Command(pid)} {};

int Process::Pid() const { return this->pid_;}

string Process::User() const {return this->user_;}

string Process::Command() const {return this->command_;}

float Process::CpuUtilization() const {
  auto process_active = LinuxParser::ActiveJiffies(this->Pid());
  auto system_active = LinuxParser::ActiveJiffies();
  return process_active / system_active;
}

string Process::Ram() const {
  return LinuxParser::Ram(this->Pid());
}

long int Process::UpTime() const {
  return LinuxParser::UpTime(this->Pid());
}

bool Process::operator<(Process const& a) const {
  return this->CpuUtilization() > a.CpuUtilization();
}