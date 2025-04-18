#pragma once

#include "HBuffer.hpp"

struct HBufferLowercaseHash{
    std::size_t operator()(const HBuffer& buff) const {
        std::size_t hash = 0;
        std::size_t prime = 31; // A small prime number

        char* data = buff.GetData();
        for (size_t i = 0; i < buff.GetSize(); i++){
            char c = data[i];
            if(c >= 'A' && c <= 'Z'){
                c += ('a' - 'A');
            }
            hash = hash * prime + c;
        }

        return hash;
    }
};

struct HBufferLowercaseEquals{
    bool operator()(const HBuffer& left, const HBuffer& right) const {
        size_t size = left.GetSize();
        if(size != right.GetSize())return false;

        //Assuming if it has a size it has valid memory if not then the user created the buffer wrong
        //if(!m_Data || !right.m_Data)return false;

        const char* leftData = left.GetData();
        const char* rightData = right.GetData();
        for(size_t i = 0; i < size; i++){
            char leftChar = leftData[i];
            char rightChar = rightData[i];
            if(leftChar >= 'A' && leftChar <= 'Z')leftChar += 'a' - 'A';
            if(rightChar >= 'A' && rightChar <= 'Z')rightChar += 'a' - 'A';
            if(leftChar != rightChar)return false;
        }
        return true;
    }
};