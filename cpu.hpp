#pragma once

#include <cstdint>

struct CpuSnapshot {
    uint64_t user = 0;
    uint64_t nice = 0;
    uint64_t system= 0;
    uint64_t idle = 0;
    uint64_t iowait  = 0;
    uint64_t irq = 0;
    uint64_t softirq = 0;
    uint64_t steal = 0;
};

// процесор
struct CpuInfo {
    double percent = 0.0;
    int    cores   = 0;
};
// завантаженість CPU
class CpuReader {
public:
    CpuReader();        // старт
    CpuInfo read();     // повертає %

private:
    CpuSnapshot readSnapshot();   // читає /proc/stat
    CpuSnapshot prev;
};