#pragma once

#include "cpu.hpp"
#include "mem.h"

// Клас AlertManager перевіряє чи CPU або RAM перевищили поріг
// і виводить WARNING якщо так
class AlertManager {
public:
    // Конструктор приймає два пороги у відсотках
    // Наприклад: AlertManager(80.0, 90.0)
    AlertManager(double cpuLimit, double ramLimit);

    // Перевіряє метрики і виводить WARNING якщо потрібно
    void check(const CpuInfo& cpu, const MemInfo& mem);

private:
    double m_cpuLimit;
    double m_ramLimit;
};