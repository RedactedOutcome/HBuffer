#pragma once

#include "Core.h"
#include "HBuffer.hpp"

/// TODO: finish because this is not ready to publish

/// @brief A vector of type std::vector<HBuffer>
template <typename Allocator=std::allocator<HBuffer>>
class HBufferVector{
public:
    HBufferVector() HBUFF_NOEXCEPT{}
    ~HBufferVector() HBUFF_NOEXCEPT{}

    bool StartsWith(const char* str, size_t len) const HBUFF_NOEXCEPT{}
    bool StartsWith(size_t at, const char* str) const HBUFF_NOEXCEPT{}
    bool StartsWith(size_t at, const char* str, size_t len) const HBUFF_NOEXCEPT{}

    /// @brief returns the character at param at if inside the buffer else \0
    char Get(size_t at)const HBUFF_NOEXCEPT{}

    /// @brief returns the character reference at the character at.
    /// @brief will segfault if at is outside of the buffers ranges
    char& At(size_t at)const HBUFF_NOEXCEPT{}


    /// @brief creates a non null terminated copy of the buffer starting from param at with a size of param len
    /// @param at the byte offset of the join to start copying from
    /// @param len the amount of bytes the new sub buffer will be
    HBuffer SubBuffer(size_t at, size_t len) HBUFF_NOEXCEPT{}

    /// @brief creates a single null terminated ascii string starting at at and is size len
    /// @brief at the position to start copying from
    /// @param len the length of the ascii bytes in the new string. will be maxed out at the buffers
    HBuffer SubString(size_t at, size_t len)HBUFF_NOEXCEPT{
        HBuffer string;
        string.Reserve(len + 1);

        size_t totalLen = 0;

        for(size_t i = 0; i < m_Vector.size(); i++){
            HBuffer buffer = m_Vector[i];
            size_t bufferSize = buffer.GetSize();

            if(at >= bufferSize){
                at-=bufferSize;
                continue;
            }

            size_t useSize = std::min(bufferSize, len);
            string.Copy(HBuffer(buffer.GetData() + at, useSize, false, false));
            totalLen+= useSize;
            len-=useSize;

            /// Only works with len is les than total buffers size
            if(totalLen >= len)break;
            at = 0;
        }
        string.AppendString("");
        return string;
    }

    template <typename Args...>
    void EmplaceBack(Args&& args){
        m_Vector.emplace_back(std::forward<Args>(args)...);
    }
private:
    std::vector<HBuffer, Allocator> m_Vector;
};