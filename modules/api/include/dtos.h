#pragma once

#include <cstddef>
#include <cstdint>

struct BenchNode {
    uint64_t time = 0;
    std::size_t threads = 0;
};