#include "AlertManager.h"
#include <iostream>

AlertManager::AlertManager(double cpuLimit, double ramLimit) {
    m_cpuLimit = cpuLimit;
    m_ramLimit = ramLimit;
}

void AlertManager::check(const CpuInfo& cpu, const MemInfo& mem) {
    if (cpu.percent > m_cpuLimit) {
        std::cout << "[WARNING] CPU завантажений на " << cpu.percent
                  << "% (поріг: " << m_cpuLimit << "%)\n";
    }

    if (mem.percent > m_ramLimit) {
        std::cout << "[WARNING] RAM використана на " << mem.percent
                  << "% (поріг: " << m_ramLimit << "%)\n";
    }
}