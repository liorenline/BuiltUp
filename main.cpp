#include <iostream>

#include "cpu.hpp"
#include "mem.h"
#include "disk.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>

std::string toHuman(uint64_t bytes) {
    double val = bytes;
    std::string unit = "B";

    if (val >= 1024) { val /= 1024; unit = "KiB"; }
    if (val >= 1024) { val /= 1024; unit = "MiB"; }
    if (val >= 1024) { val /= 1024; unit = "GiB"; }

    std::ostringstream out;
    out << std::fixed << std::setprecision(1) << val << " " << unit;
    return out.str();
}
std::string bar(double percent) {
    int total  = 20;
    int filled = percent / 100.0 * total;
    if (filled < 0) filled = 0;
    if (filled > total) filled = total;

    std::string s = "[";
    s += std::string(filled, '#');
    s += std::string(total - filled, '-');
    s += "] ";
    s += std::to_string((int)percent) + "%";
    return s;
}

std::string currentTime() {
    std::time_t t = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&t));
    return buf;
}

void printCpu(CpuInfo& cpu) {
    std::cout << "  Ядер: " << cpu.cores << "\n";
    std::cout << "  Завантаження: " << bar(cpu.percent) << "\n";
}

void printMem(MemInfo& mem) {
    std::cout << "  Всього:      " << toHuman(mem.total_kb * 1024) << "\n";
    std::cout << "  Використано: " << toHuman(mem.used_kb  * 1024) << "\n";
    std::cout << "  Вільно:      " << toHuman(mem.free_kb  * 1024) << "\n";
    std::cout << "  " << bar(mem.percent) << "\n";

    if (mem.swap_total_kb > 0) {
        std::cout << "  Swap: " << toHuman(mem.swap_used_kb * 1024)
                  << " / " << toHuman(mem.swap_total_kb * 1024) << "\n";
    }
}

void printDisk(std::vector<DiskInfo>& disks) {
    if (disks.empty()) {
        std::cout << "  (дисків не знайдено)\n";
        return;
    }

    for (DiskInfo& d : disks) {
        std::cout << "  " << d.path << "  (" << d.fstype << ")\n";
        std::cout << "    Всього:      " << toHuman(d.total_bytes) << "\n";
        std::cout << "    Використано: " << toHuman(d.used_bytes) << "\n";
        std::cout << "    Вільно:      " << toHuman(d.free_bytes) << "\n";
        std::cout << "    " << bar(d.percent) << "\n";
    }
}


int main() {
    std::cout << "SysMon запущено. Ctrl+C для виходу.\n\n";

    CpuReader  cpu;   // перша зчитка
    MemReader  mem;
    DiskReader disk;

    while (true) {
        CpuInfo             c = cpu.read();
        MemInfo             m = mem.read();
        std::vector<DiskInfo> d = disk.read();

        std::cout << "\033[2J\033[H";

        std::cout << "=== SysMon  " << currentTime() << " ===\n\n";

        std::cout << "[ CPU ]\n";
        printCpu(c);

        std::cout << "\n[ RAM ]\n";
        printMem(m);

        std::cout << "\n[ Диски ]\n";
        printDisk(d);

        std::cout << "\n";
        std::cout.flush();

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}