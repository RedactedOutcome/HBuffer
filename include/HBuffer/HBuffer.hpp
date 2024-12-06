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
    inline void Free() noexcept{  
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
    inline void Release() noexcept{
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
    void Assign(const char* str, bool canFree, bool canModify) noexcept{
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
    void Assign(const char* str, size_t len, bool canFree, bool canModify) noexept{
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
    void Assign(HBuffer& buffer, bool owns, bool canModify) noexcept{
        Free();
        m_Data = buffer.m_Data;
        m_Size = buffer.m_Size;
        m_Capacity = buffer.m_Capacity;
        m_CanModify = buffer.m_CanModify;
        m_CanFree = false;
    }

    /// @brief We will append the "foods" data to our buffer and the foods data will get released
    void Consume(HBuffer& food) noexcept{
        Append(food);
        food.Free();
    }
    /// @param offset adds offset to buffer as if +=
    void Consume(size_t from, HBuffer& food) noexcept{
        size_t newBuff1Size = (m_Size - std::min(from, m_Size));
        size_t newBuff2Size = food.m_Size - (from - std::min(from, m_Size));
        size_t newBuffSize = newBuff1Size + newBuff2Size;

        if(newBuffSize < 1)return;
        if(newBuffSize > m_Capacity || !m_CanModify){
            m_Capacity = newBuffSize;
            char* newData = new char[newBuffSize];
            memcpy(newData, m_Data + from, newBuff1Size);
            if(m_CanFree)delete m_Data;
            m_Data = newData;
            m_CanFree = true;
            m_CanModify = true;
        }else
            memcpy(m_Data, m_Data + from, newBuff1Size);
        
        m_Size = newBuffSize;
        memcpy(m_Data + newBuff1Size, food.m_Data + std::min(from - newBuff1Size, from), newBuff2Size);
        food.Free();
    }

    void Append(HBuffer& buffer) noexept;
    /// @brief swaps the contents of self with param buff
    /// @param buff
    void Swap(HBuffer& buff) noexcept{
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

    /// @brief makes data point to a copy of the null terminated string literal. Frees and reallocates if no data, cant modify, or strlen > capacity.
    /// @param str the null ternimated string literal we are copying
    void Copy(const char* str)noexcept{
        m_Size = strlen(str);
        if(m_Size < 1){
            //copy of nothing
            Free();
            return;
        }
        if(!m_Data || !m_CanModify || m_Size > m_Capacity){
            Free();

            m_Data = new char[m_Capacity];
            m_Capacity = m_Size;
            memcpy(m_Data, str, m_Size);
            m_CanFree = true;
            m_CanModify = true;
            return;
        }

        memcpy(m_Data, const_cast<char*>(str), m_Size);
    }
    /// @brief makes data point to a copy of the null terminated with a size of param size. Frees and reallocates if no self has no valid data ptr, cant modify self, or self size > capacity.
    /// @param str the null terminated string literal to copy
    /// @param size the amount of bytes to copy
    void Copy(char* str, size_t size)noexcept{
        m_Size = size;
        if(m_Size < 1){
            //copy of nothing
            Free();
            return;
        }
        if(!m_Data || !m_CanModify || m_Size > m_Capacity){
            Free();

            m_Data = new char[m_Capacity];
            m_Capacity = m_Size;
            memcpy(m_Data, str, m_Size);
            m_CanFree = true;
            m_CanModify = true;
            return;
        }

        memcpy(m_Data, const_cast<char*>(str), m_Size);
    }
    /// @brief Copies string into own buffer. Frees and reallocates if no self has no valid data ptr, cant modify self, or self size > capacity.
    /// @param string the string to make a copy of
    void Copy(const std::string& string)noexcept{
        m_Size = string.size();
        if(m_Size < 1){
            //copy of nothing
            Free();
            return;
        }
        if(!m_Data || !m_CanModify || m_Size > m_Capacity){
            Free();

            m_Data = new char[m_Capacity];
            m_Size = m_Capacity;
            m_CanFree = true;
            m_CanModify = true;
            return;
        }
        //We have access to a valid memory range to copy to
        memcpy(m_Data, const_cast<char*>(string.c_str()), m_Size);
    }
    /// @brief Makes an exact owning copy of param buff. Frees and reallocates if no self has no valid data ptr, cant modify self, or self size > capacity.
    /// @param buff the HBuffer to make a copy of
    void Copy(const HBuffer& buff)noexcept{
        m_Size = buff.m_Size;
        if(!m_Data || !m_CanModify || m_Size >= m_Capacity){
            Free();
            m_Capacity = m_Size;
            char* data = new char[m_Capacity];
            memcpy(data, buff.m_Data, m_Capacity);
            m_Data = data;
            m_CanFree = true;
            m_CanModify = true;
            return;
        }
        //Copy data into buff
        memcpy(m_Data, buff.m_Data, m_Size);
    }

    ///@brief The exact same as Copy(const char*) except we add a null terminator at the end to our buffer without including the null terminator in size/capacity. This essentially makes the buffer a string
    void CopyString(const char* str)noexcept{
        m_Size = strlen(str);
        if(m_Size < 1){
            Free();
            return;
        }
        if(!m_Data || m_CanModify || m_Size >= m_Capacity){
            Free();
            m_Capacity = m_Size + 1;
            m_Data = new char[m_Capacity];
            memcpy(data, str, m_Size + 1);
            m_Data = data;
            m_CanFree = true;
            m_CanModify = true;
            return;
        }

        memcpy(m_Data, str, m_Size + 1);
    }
    ///@brief The exact same as Copy(char*, size_t) except we add a null terminator at the end to our buffer without including the null terminator in size/capacity. This essentially makes the buffer a string
    /// @param characters the amount of characters to copy into the buffer. Does not include the null terminator
    void CopyString(char* str, size_t characters)noexcept{
        m_Size = characters;
        if(m_Size < 1){
            Free();
            return;
        }
        if(!m_Data || m_CanModify || m_Size >= m_Capacity){
            Free();
            m_Capacity = m_Size + 1;
            m_Data = new char[m_Capacity];
            memcpy(m_Data, str, m_Size);
            m_Data[m_Size] = '\0';
            m_CanFree = true;
            m_CanModify = true;
            return;
        }

        memcpy(m_Data, str, m_Size);
        m_Data[m_Size] = '\0';
    }
    void CopyString(const std::string& string) noexcept{
        m_Size = string.size();
        if(m_Size < 1){
            Free();
            return;
        }
        if(!m_Data || m_CanModify || m_Size >= m_Capacity){
            Free();
            m_Capacity = m_Size + 1;
            m_Data = new char[m_Capacity];
            memcpy(m_Data, string.c_str(), m_Size);
            m_Data[m_Size] = '\0';
            
            m_CanFree = true;
            m_CanModify = true;
            return;
        }

        memcpy(m_Data, string.c_str(), m_Size);
        m_Data[m_Size] = '\0';
    }
    void CopyString(const HBuffer& buff) noexcept{
        m_Size = buff.m_Size;
        if(m_Size < 1){
            Free();
            return;
        }
        if(!m_Data || m_CanModify || m_Size >= m_Capacity){
            Free();
            m_Capacity = m_Size + 1;
            m_Data = new char[m_Capacity];
            memcpy(m_Data, buff.m_Data, m_Size);
            m_Data[m_Size] = '\0';
            
            m_CanFree = true;
            m_CanModify = true;
            return;
        }

        memcpy(m_Data, buff.m_Data, m_Size);
        m_Data[m_Size] = '\0';
    }

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
    const char* GetCStr() const noexcept{
        if(m_Data)return m_Data;
        return "";
    }
    /// @brief returns data ptr
    constexpr bool CanFree() const noexcept{return m_CanFree;}
    constexpr bool CanModify() const noexcept{return m_CanModify;}
    constexpr char* GetData() const noexcept{return m_Data;}
    constexpr size_t GetSize() const noexcept{return m_Size;}
public:
    //Assins buffer to non owning copy of data
    HBuffer& operator=(const HBuffer& right) noexcept{
        m_Size = right.m_Size;
        if(!m_Data || !m_CanModify || m_Size >= m_Capacity){
            Free();
            m_Capacity = m_Size;
            char* data = new char[m_Capacity];
            memcpy(data, right.m_Data, m_Capacity);
            m_Data = data;
            m_CanFree = true;
            m_CanModify = true;
            return;
        }
        //Copy data into buff
        memcpy(m_Data, right.m_Data, m_Size);
    }
    HBuffer& operator=(HBuffer&& right) noexcept{
        Free();
        m_Data = right.m_Data;
        m_Size = right.m_Size;
        m_Capacity = right.m_Capacity;
        m_CanFree = right.m_CanFree;
        m_CanModify = right.m_CanModify;
        right.Release();
    }
    /// @brief assigns a non owning view of para right that cannot modify
    HBuffer& operator=(const char* right) noexcept;
    /// @brief adds an offset to the vector. If owns data it frees and reallocates. If not then we just increment pointer and change size
    ///HBuffer& operator+=(size_t offset) noexcept;
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