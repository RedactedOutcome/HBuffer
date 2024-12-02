#pragma once

#include "pch.h"

//#define HBUFFER_TRACK_ALLOCATIONS

class HBuffer{
public:
    friend class HBufferJoin;
    HBuffer():m_Data(nullptr), m_Size(0), m_Capacity(0), m_CanFree(false){}
    HBuffer(const char* str, bool canFree, bool canModify):m_Size(strlen(str)),
m_Data(const_cast<char*>(str)), m_CanFree(canFree), m_CanModify(canModify){}
    HBuffer(const char* str, size_t len, bool canFree, bool canModify):m_Size(len), m_Data(const_cast<char*>(str)), m_Capacity(m_Size), m_CanFree(canFree), m_CanModify(canModify){}
    HBuffer(const HBuffer& buffer):m_Data(buffer.m_Data), m_Size(buffer.m_Size), m_Capacity(m_Size), m_CanFree(false), m_CanModify(buffer.m_CanModify){
        //We assume @param buffer owns the data and will manage it properly
    }
    HBuffer(const std::string& string);
    HBuffer(HBuffer&& buffer);
    ~HBuffer();

    /// @return returns the character at i. returns '\0' if out of range
    char At(size_t i) const noexcept;

    void SetSize(size_t size) noexcept;

    //Assign buffers, sizes, and owenership
    //TODO: maybe capacity
    void Assign(const char* str, bool canFree, bool canModify = true);
    void Assign(const char* str, size_t len, bool canFree, bool canModify = true);
    void Assign(HBuffer& buffer, bool owns, bool canModify = true);

    /// @brief We will append the "foods" data to our buffer and the foods data will get released
    void Consume(HBuffer& food);
    /// @param offset adds offset to buffer as if +=
    void Consume(size_t from, HBuffer& food);

    void Append(HBuffer& buffer);
    /// @brief Frees data if we own that data and releases
    void Free();
    /// @brief releases data as in makes m_Data nullptr
    void Release();
    void Swap(HBuffer& buff);

    /// @brief Copies data into buffer and reallocates if newsize is greater than old. If not greater the new size of the buffer is the size of the new string and the rest is unmodified
    /// @param str
    void Copy(const char* str);
    void Copy(char* str, size_t size);
    void Copy(const std::string& string);
    void Copy(const HBuffer& buff);

    ///@brief Copies data into newly allocated buffer except new buffer has a null terminateor
    void CopyString(const char* str);
    void CopyString(char* str, size_t size);
    void CopyString(const std::string& string);
    void CopyString(const HBuffer& buff);

public:
    bool StartsWith(size_t at, const char* str) const noexcept;
    bool StartsWith(const char* str) const noexcept;
    bool StartsWith(const char* str, size_t len) const noexcept;
    
    /// @return returns 0 if success return -1 if buffer is out of data and 1 if data doesnt match
    int StrXCmp(const char* str) const noexcept;

    void Memcpy(void* dest, size_t len) const noexcept;
    /// @brief Attempts to memcpy into dest if in range and pads with 0s if len is greater than accumulated buffers size
    /// @param at the position in the buffers to start reading from
    void Memcpy(void* dest, size_t at, size_t len) const noexcept;
public:
    /// @brief if data is not null returns that data if it is null we return a non allocated "" literal
    const char* GetCStr() const noexcept;
    /// @brief returns data ptr
    constexpr bool CanFree() const noexcept{return m_CanFree;}
    constexpr bool CanModify() const noexcept{return m_CanModify;}
    constexpr char* GetData() const noexcept{return m_Data;}
    constexpr size_t GetSize() const noexcept{return m_Size;}
public:
    HBuffer& operator=(const HBuffer& right) noexcept;
    HBuffer& operator=(HBuffer&& right) noexcept;
    /// @brief assigns a non owning view of para right that cannot modify
    HBuffer& operator=(const char* right) noexcept;
    /// @brief adds an offset to the vector. If owns data it frees and reallocates. If not then we just increment pointer and change size
    HBuffer& operator+=(size_t offset) noexcept;

    //TODO:maybe possible -= operator if needed

    //Checks if content matches
    constexpr bool operator==(const HBuffer& right)const noexcept;
    constexpr bool operator==(const char* str)const noexcept;
    constexpr bool operator!=(const HBuffer& right)const noexcept;
private:
    char* m_Data = nullptr;
    size_t m_Size = 0;
    size_t m_Capacity = 0;
    bool m_CanFree = false;
    bool m_CanModify = false;
};