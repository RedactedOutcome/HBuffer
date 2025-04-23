#pragma once

#include "Core.h"

/// @brief A vector of type std::vector<HBuffer>
template <typename Allocator=std::allocator<HBuffer>>
class HBufferVector{
public:
    HBufferVector() HBUFF_NOEXCEPT{}
    ~HBufferVector() HBUFF_NOEXCEPT{}

    template <typename Args...>
    void EmplaceBack(Args&& args){
        m_Vector.emplace_back(std::forward<Args>(args)...);
    }
private:
    std::vector<HBuffer, Allocator> m_Vector;
};