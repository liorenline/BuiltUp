//
// Created by lioren on 5/15/26.
//

#include "cpu.hpp"

#include "cpu.hpp"

#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <stdexcept>

CpuReader::CpuReader() {
    prev = readSnapshot();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

CpuSnapshot CpuReader::readSnapshot() {
    std::ifstream file("/proc/stat");
    if (!file.is_open()) {
        throw std::runtime_error("Не вдалось відкрити /proc/stat");
    }

    std::string label;
    CpuSnapshot s;

    file >> label
         >> s.user >> s.nice >> s.system >> s.idle
         >> s.iowait >> s.irq >> s.softirq >> s.steal;

    return s;
}

CpuInfo CpuReader::read() {
    CpuSnapshot curr = readSnapshot();

    uint64_t total = (curr.user - prev.user)
                   + (curr.nice - prev.nice)
                   + (curr.system - prev.system)
                   + (curr.idle - prev.idle)
                   + (curr.iowait - prev.iowait)
                   + (curr.irq - prev.irq)
                   + (curr.softirq - prev.softirq)
                   + (curr.steal - prev.steal);

    uint64_t busy = total - (curr.idle - prev.idle)
                          - (curr.iowait - prev.iowait);

    CpuInfo info;
    if (total > 0) {
        info.percent = 100.0 * busy / total;
    }

    std::ifstream file("/proc/stat");
    std::string line;
    info.cores = 0;
    while (std::getline(file, line)) {
        if (line.size() > 3 && line[0] == 'c' && line[1] == 'p' &&
            line[2] == 'u' && line[3] >= '0' && line[3] <= '9') {
            info.cores++;
            }
    }

    prev = curr;
    return info;
}
