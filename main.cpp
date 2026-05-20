#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <filesystem>
#include "cpu.hpp"
#include "mem.h"
#include "disk.h"
#include "Logger.h"
#include "AlertManager.h"

int main() {
    CpuReader cpu;
    MemReader mem;
    DiskReader disk;

    std::string logFile = "metrics.json";

    Logger logger(logFile);
    AlertManager alerts(80.0, 90.0);

    std::string fullPath = std::filesystem::absolute(logFile).string();
    std::cout << "SysMon запущено.\n";
    std::cout << "Метрики записуються у: " << fullPath << "\n";
    std::cout << "Ctrl+C для виходу.\n";

    while (true) {
        CpuInfo c  = cpu.read();
        MemInfo m  = mem.read();
        std::vector<DiskInfo> d = disk.read();

        alerts.check(c, m);
        logger.write(c, m, d);

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}