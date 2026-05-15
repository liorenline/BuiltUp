#include "mem.h"

#include <fstream>
#include <string>
#include <stdexcept>

MemInfo MemReader::read() {
    std::ifstream file("/proc/meminfo");
    if (!file.is_open()) {
        throw std::runtime_error("Не вдалось відкрити /proc/meminfo");
    }

    MemInfo info;
    uint64_t available_kb = 0;
    uint64_t swap_free_kb = 0;

    std::string key;
    uint64_t value;

    while (file >> key >> value) {
        std::string rest;
        std::getline(file, rest);

        if (key == "MemTotal:")     info.total_kb = value;
        if (key == "MemAvailable:") available_kb = value;
        if (key == "SwapTotal:")    info.swap_total_kb = value;
        if (key == "SwapFree:")     swap_free_kb = value;
    }

    info.free_kb = available_kb;
    if (info.total_kb > available_kb) {
        info.used_kb = info.total_kb - available_kb;
    }
    if (info.total_kb > 0) {
        info.percent = 100.0 * info.used_kb / info.total_kb;
    }

    if (info.swap_total_kb > swap_free_kb) {
        info.swap_used_kb = info.swap_total_kb - swap_free_kb;
    }
    if (info.swap_total_kb > 0) {
        info.swap_percent = 100.0 * info.swap_used_kb / info.swap_total_kb;
    }

    return info;
}