#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <ctime>
#include <csignal>
#include "cpu.hpp"
#include "mem.h"
#include "disk.h"

const int    INTERVAL_SEC = 2;
const double CPU_WARN  = 80.0;
const double MEM_WARN = 85.0;
const double DISK_WARN = 90.0;

bool g_running = true;

void onSignal(int) {
    g_running = false;
}

std::string currentIso() {
    std::time_t t = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", std::localtime(&t));
    return buf;
}

std::string currentDate() {
    std::time_t t = std::time(nullptr);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", std::localtime(&t));
    return buf;
}

std::string escapeJson(const std::string& s) {
    std::string out;
    for (int i = 0; i < (int)s.size(); ++i) {
        if (s[i] == '"')       out += "\\\"";
        else if (s[i] == '\\') out += "\\\\";
        else                   out += s[i];
    }
    return out;
}

std::string logFile() {
    return "sysmon-" + currentDate() + ".json";
}

std::string alertFile() {
    return "alerts-" + currentDate() + ".json";
}

bool hasWarning(const CpuInfo& c, const MemInfo& m,
                const std::vector<DiskInfo>& disks) {
    if (c.percent >= CPU_WARN) return true;
    if (m.percent >= MEM_WARN) return true;
    for (int i = 0; i < (int)disks.size(); ++i)
        if (disks[i].percent >= DISK_WARN) return true;
    return false;
}

void writeMetrics(const CpuInfo& c, const MemInfo& m,
                  const std::vector<DiskInfo>& disks, bool warn) {
    std::ofstream f(logFile(), std::ios::app);
    if (!f.is_open()) return;

    double totalGb = m.total_kb / (1024.0 * 1024.0);
    double usedGb  = m.used_kb  / (1024.0 * 1024.0);

    f << "{\n"
      << "  \"ts\": \""    << currentIso() << "\",\n"
      << "  \"cpu\": {\n"
      << "    \"cores\": " << c.cores << ",\n"
      << "    \"pct\": "   << std::fixed << std::setprecision(1) << c.percent << "\n"
      << "  },\n"
      << "  \"mem\": {\n"
      << "    \"total_gb\": " << std::fixed << std::setprecision(2) << totalGb << ",\n"
      << "    \"used_gb\": "  << std::fixed << std::setprecision(2) << usedGb  << ",\n"
      << "    \"pct\": "      << std::fixed << std::setprecision(1) << m.percent << "\n"
      << "  }";

    if (!disks.empty()) {
        f << ",\n  \"disks\": [";
        for (int i = 0; i < (int)disks.size(); ++i) {
            if (i) f << ",";
            f << "\n    {\n"
              << "      \"path\": \""    << escapeJson(disks[i].path) << "\",\n"
              << "      \"pct\": "       << std::fixed << std::setprecision(1) << disks[i].percent << ",\n"
              << "      \"total_gb\": "  << std::fixed << std::setprecision(2) << (disks[i].total_bytes / 1073741824.0) << ",\n"
              << "      \"used_gb\": "   << std::fixed << std::setprecision(2) << (disks[i].used_bytes  / 1073741824.0) << ",\n"
              << "      \"free_gb\": "   << std::fixed << std::setprecision(2) << (disks[i].free_bytes  / 1073741824.0) << "\n"
              << "    }";
        }
        f << "\n  ]";
    }

    f << ",\n  \"warning\": " << (warn ? "true" : "false") << "\n}\n";
}

void writeAlert(const CpuInfo& c, const MemInfo& m,
                const std::vector<DiskInfo>& disks) {
    std::ofstream f(alertFile(), std::ios::app);
    if (!f.is_open()) return;

    f << "{\n"
      << "  \"ts\": \"" << currentIso() << "\",\n"
      << "  \"alerts\": [";

    bool first = true;
    if (c.percent >= CPU_WARN) {
        f << (first ? "\n" : ",\n")
          << "    { \"type\": \"cpu\", \"pct\": "
          << std::fixed << std::setprecision(1) << c.percent << " }";
        first = false;
    }
    if (m.percent >= MEM_WARN) {
        f << (first ? "\n" : ",\n")
          << "    { \"type\": \"mem\", \"pct\": "
          << std::fixed << std::setprecision(1) << m.percent << " }";
        first = false;
    }
    for (int i = 0; i < (int)disks.size(); ++i) {
        if (disks[i].percent >= DISK_WARN) {
            f << (first ? "\n" : ",\n")
              << "    { \"type\": \"disk\", \"path\": \"" << escapeJson(disks[i].path) << "\""
              << ", \"pct\": " << std::fixed << std::setprecision(1) << disks[i].percent << " }";
            first = false;
        }
    }

    f << "\n  ]\n}\n";
}

void printWarning(const CpuInfo& c, const MemInfo& m,
                  const std::vector<DiskInfo>& disks) {
    std::cout << "\n  [!] WARNING at " << currentIso() << "\n";
    if (c.percent >= CPU_WARN)
        std::cout << "      CPU " << std::fixed << std::setprecision(1)
                  << c.percent << "% (threshold " << CPU_WARN << "%)\n";
    if (m.percent >= MEM_WARN)
        std::cout << "      RAM " << std::fixed << std::setprecision(1)
                  << m.percent << "% (threshold " << MEM_WARN << "%)\n";
    for (int i = 0; i < (int)disks.size(); ++i) {
        if (disks[i].percent >= DISK_WARN)
            std::cout << "      Disk " << disks[i].path << " "
                      << std::fixed << std::setprecision(1)
                      << disks[i].percent << "% (threshold " << DISK_WARN << "%)\n";
    }
}

void printStats(int ticks, double cpuSum, double cpuMax,
                double memSum, double memMax) {
    if (ticks == 0) return;
    std::cout << "\n--- Session stats (" << ticks << " records) ---\n";
    std::cout << "  CPU  avg: " << std::fixed << std::setprecision(1)
              << cpuSum / ticks << "%  max: " << cpuMax << "%\n";
    std::cout << "  RAM  avg: " << std::fixed << std::setprecision(1)
              << memSum / ticks << "%  max: " << memMax << "%\n";
}

int main() {
    std::signal(SIGINT,  onSignal);
    std::signal(SIGTERM, onSignal);

    std::cout << "SysMon started.\n";
    std::cout << "Press Ctrl+C to stop.\n\n";

    CpuReader  cpu;
    MemReader  mem;
    DiskReader disk;

    int ticks   = 0;
    int alerts  = 0;
    double cpuSum  = 0.0;
    double cpuMax  = 0.0;
    double memSum  = 0.0;
    double memMax  = 0.0;

    while (g_running) {
        CpuInfo c = cpu.read();
        MemInfo m = mem.read();
        std::vector<DiskInfo> d = disk.read();

        bool warn = hasWarning(c, m, d);

        writeMetrics(c, m, d, warn);
        if (warn) {
            writeAlert(c, m, d);
            printWarning(c, m, d);
            ++alerts;
        }

        cpuSum += c.percent;
        memSum += m.percent;
        if (c.percent > cpuMax) cpuMax = c.percent;
        if (m.percent > memMax) memMax = m.percent;
        ++ticks;

        std::cout << "\r  Records: " << ticks
                  << "  Alerts: " << alerts
                  << "  Log: " << logFile() << "   " << std::flush;

        for (int i = 0; i < INTERVAL_SEC * 10 && g_running; ++i)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    printStats(ticks, cpuSum, cpuMax, memSum, memMax);

    std::cout << "\nMetrics : " << logFile()  << "\n";
    std::cout << "Alerts  : " << alertFile() << "\n";
    return 0;
}