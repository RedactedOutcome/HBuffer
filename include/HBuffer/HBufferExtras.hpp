#pragma once

#include "HBuffer.h"


struct HBufferLowercaseHash{
    std::size_t operator()(const HBuffer& h) const {
        return reinterpret_cast<std::uintptr_t>(&h) % 10007;
    }
    bool operator()(const HBuffer& left, const HBuffer& right) const {
        return &left == &right;
    }
}