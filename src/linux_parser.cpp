#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <numeric>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;


// Utils
int LinuxParser::ProcessReader(std::string targetLabel) {
  int processes {};

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::string line {};
    while (std::getline(stream, line)) {
      std::istringstream iss(line);
      std::string label {};
      iss >> label;
      if (label == targetLabel) {
        iss >> processes;
      }
    }
  }

  return processes;
}

vector<unsigned long> LinuxParser::JiffiesReader() {
  vector<unsigned long> jiffies {};

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {

    // Get first line - aggregate of CPU clock times
    std::string line {};
    std::getline(stream, line);
    std::istringstream cpuStats(line);

    std::string label {};
    cpuStats >> label; // Remove "cpu" label

    unsigned long stat {};
    while (cpuStats) {
      cpuStats >> stat;
      jiffies.push_back(stat);
    }
  }

  return jiffies;
}

string LinuxParser::GetPath(int pid, string suffix) {
    return kProcDirectory + to_string(pid) + suffix;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  unsigned long totalMem {};
  unsigned long freeMem {};

  if (stream.is_open()) {
    // Loop through the contents of the file; safer this way
    // in case order of items change
    std::string memType {};
    while(stream >> memType) {
      if (memType == "MemTotal:") {
        stream >> totalMem;
      } else if (memType == "MemFree:") {
        stream >> freeMem;
      }
    }
  }

  return (totalMem - freeMem) / totalMem;
}

long LinuxParser::UpTime() {
  unsigned long uptime {};
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    stream >> uptime;
  }

  return uptime;
}

long LinuxParser::Jiffies() {
  auto jiffies = JiffiesReader();
  return std::accumulate(jiffies.begin(), jiffies.end(), 0);
}

long LinuxParser::ActiveJiffies(int pid) {
  vector<long> jiffies {};
  std::ifstream stream(GetPath(pid, kStatFilename));
  if (stream.is_open()) {
    size_t i {1};
    while (i <= 17) {
      long val {};
      stream >> val;

      // Values in place 14 to 17 represent the target values
      if (i >= 14 && i <= 17) {
        jiffies.push_back(val);
      }

      i++;
    }
  }

  return std::accumulate(jiffies.begin(), jiffies.end(), 0);
}

long LinuxParser::ActiveJiffies() {
  auto jiffies = JiffiesReader();
  jiffies[IDLE_IDX] = jiffies[IOWAIT_IDX] = 0; // Remove idle and iowait
  return std::accumulate(jiffies.begin(), jiffies.end(), 0);
}

long LinuxParser::IdleJiffies() {
  auto jiffies = JiffiesReader();
  return jiffies[IDLE_IDX] + jiffies[IOWAIT_IDX];
}

// Change signature to long to make it easier to reuse function
// and simplify percentage calculation in Processor
vector<unsigned long> LinuxParser::CpuUtilization() {
  return JiffiesReader();
}

int LinuxParser::TotalProcesses() {
  return ProcessReader("processes");
}

int LinuxParser::RunningProcesses() {
  return ProcessReader("procs_running");
}

string LinuxParser::Command(int pid) {
  string command;
  std::ifstream stream(GetPath(pid, kCmdlineFilename));
  if (stream.is_open()) {
    std::getline(stream, command);
  }

  return command;
}

string LinuxParser::Ram(int pid) {
  std::ifstream stream(GetPath(pid, kStatusFilename));
  string ram {};
  string unit {};
  if (stream.is_open()) {
    string label {};
    while(stream >> label) {
      if (label == "VmSize:") {
        stream >> ram >> unit;
      }
    }
  }

  return ram + unit;
}

string LinuxParser::Uid(int pid) {
  std::ifstream stream(GetPath(pid, kStatusFilename));
  string uid {};
  if (stream.is_open()) {
    string label {};
    while(stream >> label) {
      if (label == "VmSize:") {
        stream >> uid;
      }
    }
  }

  return uid;
}

string LinuxParser::User(int pid) {
  string line {};
  string user {};
  string foundId {};
  string uid = Uid(pid);

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line) && foundId != uid) {
      // Replace ":" with space
      std::replace_if(
          line.begin(),
          line.end(),
          [] (const char& c) { return std::ispunct(c) ;},
          ' '
          );

      std::istringstream iss(line);
      string x {};
      iss >> user >> x >> foundId;
    }
  }

  return user;
}

long LinuxParser::UpTime(int pid) {
  long uptime {};
  std::ifstream stream(GetPath(pid, kStatFilename));
  constexpr int index {22};
  size_t i {1};
  if (stream.is_open()) {
    while (i <= index) {
      stream >> uptime;
      i++;
    }
  }

  return uptime;
}