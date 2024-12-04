#pragma once

#include "Core.h"

class HBuffer{
public:
    friend class HBufferJoin;
    /// @brief Initializes HBuffer to point to nothing and own nothing
    HBuffer():m_Data(nullptr), m_Size(0), m_Capacity(0), m_CanFree(false){}
    /// @brief Points data to str and allows you to decide if that data should be modified or not
    /// @param str the string literal to point to
    /// @param canFree gives ownership to buffer
    /// @param canModify decides if buffer is allowed to modify data
    HBuffer(const char* str, bool canFree, bool canModify):m_Size(strlen(str)),
        m_Data(const_cast<char*>(str)), m_CanFree(canFree), m_CanModify(canModify){}
    
    
    /// @brief Makes data point to str and gives it a size/capacity to use depending on canModify. Will complete buffer if canFree is true
    /// @param str 
    /// @param len 
    /// @param canFree 
    /// @param canModify decides if the buffer can directly modify data or if it has to make a copy if it needs to edit data
    HBuffer(const char* str, size_t len, bool canFree, bool canModify):m_Size(len), m_Data(const_cast<char*>(str)), m_Capacity(m_Size), m_CanFree(canFree), m_CanModify(canModify){}

    /// @brief Makes the buffer an exact non owning copy of param buffer
    /// @param buffer 
    HBuffer(const HBuffer& buffer):m_Data(buffer.m_Data), m_Size(buffer.m_Size), m_Capacity(m_Size), m_CanFree(false), m_CanModify(buffer.m_CanModify){
        //We assume @param buffer owns the data and will manage it properly
    }
    /// @brief Makes a copy of the string
    /// @param string 
    HBuffer(const std::string& string){
        m_Capacity = strlen(str.c_str());
        m_Size = m_Capacity;
        m_Data = new char[m_Capacity];
        m_CanFree = true;
        m_CanModify = true;

        memcpy(m_Data, str.c_str(), m_Size);
    }
    /// @brief Moves param buffer into self and releases param buffers data
    /// @param buffer the buffer to get data from and release
    HBuffer(HBuffer&& buffer);
    
    /// @brief will Free data if owns
    ~HBuffer(){
        Free();
    }
    /// @brief Frees data if buffer is owning and releases after regardless
    void Free(){  
        if(m_CanFree){
            //TODO: implement tracking of allocations
            //#ifdef HBUFFER_TRACK_ALLOCATIONS
            //CORE_DEBUG("Freeing HBuffer with size {0}", m_Size);
            //#endif
            delete m_Data;
        }
        //Release data regardless
        m_Data = nullptr;
        m_Size = 0;
        m_Capacity = 0;
        m_CanFree = false;
        m_CanModify = false;
    }
    /// @brief releases data without freeing. Will lead to memory leaks unless used properly
    inline void Release(){
        m_Data = nullptr;
        m_Size = 0;
        m_Capacity = 0;
        m_CanFree = false;
        m_CanModify = false;
    }

    /*
    /// @brief Retrieves the char at param i. Throws std::out_of_range if i is out of buffer range/size
    char Get(size_t i) const noexcept;
    */

    /// @return returns the character at i. returns '\0' if out of range
    char At(size_t i) const noexcept{
        if(i < m_Size)return m_Data[i];
        return '\0';
    }

    /// @brief Sets the size of the buffer and reallocates and changes data if param size > m_Capacity
    /// @param size the size to set to
    void SetSize(size_t size) noexcept{
        if(size > m_Capacity){
            char* newData = new char[size];
            memcpy(newData, m_Data, m_Capacity);
            m_Capacity = size;
        }
        m_Size = size;
    }

    //Assign data to point to a string literal.
    //TODO: maybe capacity


    /// @brief Makes buffer point to a null terminated string literal
    /// @param str the string to point to
    /// @param canFree do we own this data
    /// @param canModify can we modify this data
    void Assign(const char* str, bool canFree, bool canModify){
        Free();
        m_Data = const_cast<char*>(str);
        m_Size = strlen(m_Data);
        m_Capacity = m_Size;
        m_CanFree = canFree;
        m_CanModify = canModify;
    }

    /// @brief Sets data to point at a null terminated string literal.
    /// @param str the string that the buffer points to
    /// @param len the new length and capacity
    /// @param canFree is this buffer owning this data
    /// @param canModify can we modify this data
    void Assign(const char* str, size_t len, bool canFree, bool canModify){
        Free();
        m_Data = const_cast<char*>(str);
        m_Size = len;
        m_Capacity = len;
        m_CanFree = canFree;
        m_CanModify = canModify;
    }


    /// @brief Sets buffer to a non owning view that has the same properties of the param buffer
    /// @param buffer 
    /// @param owns 
    /// @param canModify 
    void Assign(HBuffer& buffer, bool owns, bool canModify){
        Free();
        m_Data = buffer.m_Data;
        m_Size = buffer.m_Size;
        m_Capacity = buffer.m_Capacity;
        m_CanModify = buffer.m_CanModify;
        m_CanFree = false;
    }

    /// @brief We will append the "foods" data to our buffer and the foods data will get released
    void Consume(HBuffer& food);
    /// @param offset adds offset to buffer as if +=
    void Consume(size_t from, HBuffer& food);

    void Append(HBuffer& buffer);
    /// @brief swaps the contents of self with param buff
    /// @param buff
    void Swap(HBuffer& buff){
        char* data = buff.m_Data;
        size_t capacity = buff.m_Capacity;
        size_t size = buff.m_Size;
        bool canFree = buff.m_CanFree;
        bool canModify = buff.m_CanModify;

        buff.m_Data = m_Data;
        buff.m_Size = m_Size;
        buff.m_Capacity = m_Capacity;
        buff.m_CanFree = m_CanFree;
        buff.m_CanModify = m_CanModify;

        m_Data = data;
        m_Capacity = capacity;
        m_Size = size;
        m_CanFree = canFree;
        m_CanModify = canModify;
    }

    /// @brief Calls Free() and makes data point to a copy of the null terminated string literal
    /// @param str the null ternimated string literal we are copying
    void Copy(const char* str);
    /// @brief Calls Free() and makes data point to a copy of the null terminated with a size of param size
    /// @param str the null terminated string literal to copy
    /// @param size the amount of bytes to copy
    void Copy(char* str, size_t size);
    /// @brief Calls Free() and makes a copy of that std::string with the size and capacity being the strings size
    /// @param string the string to make a copy of
    void Copy(const std::string& string);
    /// @brief Calls Free() Makes an exact copy of param buff except we own this new data
    /// @param buff the HBuffer to make a copy of
    void Copy(const HBuffer& buff);

    ///@brief Calls Free() and copies all characters and adds a null terminator at end of buffer. The size will be the amount of characters in the string despite the buffers size being +1 with the null terminator
    void CopyString(const char* str);
    /// @brief 
    /// @param str 
    /// @param size 
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