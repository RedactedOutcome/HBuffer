#pragma once

#include "HBuffer.hpp"

struct HBufferLowercaseHash{
    std::size_t operator()(const HBuffer& buff) const {
        std::size_t hash = 0;
        std::size_t prime = 31; // A small prime number

        char* data = buff.GetData();
        for (size_t i = 0; i < buff.GetSize(); i++){
            char c = data[i];
            if(c >= 'A' && c <= 'Z')c -= ('a' - 'A');
            hash = hash * prime + c;
        }

        return hash;
    }
    bool operator()(const HBuffer& left, const HBuffer& right) const {
        return &left == &right;
    }
};