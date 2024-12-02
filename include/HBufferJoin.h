#pragma once

#include "HBuffer.h"

class HBufferJoin{
public:
    HBufferJoin();
    HBufferJoin(HBuffer& buff1, HBuffer& buff2);
    HBufferJoin(HBuffer&& buff1);
    ~HBufferJoin();

    /// @brief calls Free() on buffers
    void Free();
    
    HBuffer SubStringToDelim(char delimeter)const;
    HBuffer SubString(size_t pos, size_t len) const;
public:
    bool StartsWith(size_t at, const char* str) const noexcept;
    bool StartsWith(size_t at, const char* str, size_t len) const noexcept;
    bool StartsWith(const char* str) const noexcept;
    bool StartsWith(const char* str, size_t len) const noexcept;
    
    ///@brief opies from buffers into dest.
    ///@return returns 0 if success
    int Memcpy(void* dest, size_t len) const;
    int Memcpy(void* dest, size_t offset, size_t len) const;
    //Acts as if cstr with strcmp
    int StrCmp(const char* str) const noexcept;
    
    /// @return returns 0 if success return -1 if buffer is out of data and 1 if data doesnt match
    int StrXCmp(const char* str) const noexcept;
    int StrXCmp(size_t at, const char* str) const noexcept;
public:
    /// @return returns the character at i. returns '\0' if out of range of buffers
    char At(size_t i) const noexcept;
public:
    constexpr HBuffer& GetBuffer1() const noexcept{return (HBuffer&)m_Buffer1;}
    constexpr HBuffer& GetBuffer2() const noexcept{return (HBuffer&)m_Buffer2;}
    constexpr size_t GetSize() const noexcept{return m_Buffer1.GetSize() + m_Buffer2.GetSize();}
public:
    HBufferJoin& operator+=(size_t i);
private:
    HBuffer m_Buffer1;
    HBuffer m_Buffer2;
};