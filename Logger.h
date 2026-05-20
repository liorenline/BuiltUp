#pragma once

#include <string>
#include <fstream>
#include "cpu.hpp"
#include "mem.h"
#include "disk.h"

// Клас Logger записує метрики у JSON-файл
class Logger {
public:
    // Конструктор приймає ім'я файлу куди писати
    Logger(const std::string& filename);

    // Записує один рядок (один "знімок") у файл
    void write(const CpuInfo& cpu, const MemInfo& mem, const std::vector<DiskInfo>& disks);

private:
    std::string m_filename;

    // Повертає поточний час у форматі "2025-05-20 14:30:00"
    std::string getTime();

    // Формує JSON-рядок зі всіх метрик
    std::string buildJson(const CpuInfo& cpu,
                          const MemInfo& mem,
                          const std::vector<DiskInfo>& disks,
                          const std::string& time);
};