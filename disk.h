#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct DiskInfo {
    std::string path;
    std::string device;
    std::string fstype;

    uint64_t total_bytes = 0;
    uint64_t used_bytes = 0;
    uint64_t free_bytes = 0;
    double percent = 0.0;
};

class DiskReader {
public:
    std::vector<DiskInfo> read();
};