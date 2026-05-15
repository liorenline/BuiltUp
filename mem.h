//
// Created by lioren on 5/15/26.
//

#ifndef BUILTUP_MEM_H
#define BUILTUP_MEM_H

#endif //BUILTUP_MEM_H

#pragma once

#include <cstdint>

struct MemInfo {
    uint64_t total_kb = 0;
    uint64_t used_kb = 0;
    uint64_t free_kb = 0;
    double percent = 0.0;

    uint64_t swap_total_kb = 0;
    uint64_t swap_used_kb = 0;
    double swap_percent = 0.0;
};

class MemReader {
public:
    MemInfo read();
};