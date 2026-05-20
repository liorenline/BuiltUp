#include "Logger.h"
#include <ctime>
#include <sstream>
#include <iostream>

Logger::Logger(const std::string& filename) {
    m_filename = filename;
}

std::string Logger::getTime() {
    std::time_t t = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return buf;
}

std::string Logger::buildJson(const CpuInfo& cpu,
                               const MemInfo& mem,
                               const std::vector<DiskInfo>& disks,
                               const std::string& time) {
    std::ostringstream json;

    json << "{\n";
    json << "  \"time\": \"" << time << "\",\n";

    // CPU
    json << "  \"cpu\": {\n";
    json << "    \"cores\": " << cpu.cores << ",\n";
    json << "    \"percent\": " << cpu.percent << "\n";
    json << "  },\n";

    // RAM
    json << "  \"ram\": {\n";
    json << "    \"total_kb\": " << mem.total_kb << ",\n";
    json << "    \"used_kb\": " << mem.used_kb << ",\n";
    json << "    \"free_kb\": " << mem.free_kb << ",\n";
    json << "    \"percent\": " << mem.percent << "\n";
    json << "  },\n";

    // Диски
    json << "  \"disks\": [\n";
    for (int i = 0; i < (int)disks.size(); i++) {
        const DiskInfo& d = disks[i];
        json << "    {\n";
        json << "      \"path\": \"" << d.path << "\",\n";
        json << "      \"fstype\": \"" << d.fstype << "\",\n";
        json << "      \"total_bytes\": " << d.total_bytes << ",\n";
        json << "      \"used_bytes\": " << d.used_bytes << ",\n";
        json << "      \"free_bytes\": " << d.free_bytes << ",\n";
        json << "      \"percent\": " << d.percent << "\n";
        json << "    }";
        if (i + 1 < (int)disks.size()) {
            json << ",";
        }
        json << "\n";
    }
    json << "  ]\n";
    json << "}";

    return json.str();
}

void Logger::write(const CpuInfo& cpu, const MemInfo& mem, const std::vector<DiskInfo>& disks) {
    std::string time = getTime();
    std::string json = buildJson(cpu, mem, disks, time);

    // Відкриваємо файл у режимі append (дописуємо, не затираємо)
    std::ofstream file(m_filename, std::ios::app);

    if (!file.is_open()) {
        std::cerr << "[Logger] Помилка: не вдалося відкрити файл " << m_filename << "\n";
        return;
    }

    file << json << "\n";
    file.close();
}