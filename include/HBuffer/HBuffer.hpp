#pragma once

#include "Core.h"

/// TODO: For reallocation chekds just check if we cn modify 
class HBuffer{
public:
    friend class HBufferJoin;
    /// @brief Initializes HBuffer to point to nothing and own nothing
    HBuffer()HBUFF_NOEXCEPT:m_Data(nullptr), m_Size(0), m_Capacity(0), m_CanFree(false), m_CanModify(false){}
    /// @brief Points data to str and allows you to decide if that data should be modified or not
    /// @param str the string literal to point to
    /// @param canFree gives ownership to buffer
    /// @param canModify decides if buffer is allowed to modify data
    HBuffer(const char* str, bool canFree, bool canModify)HBUFF_NOEXCEPT
        : m_Size(strlen(str)), m_Capacity(m_Size + 1), m_Data(const_cast<char*>(str)), m_CanFree(canFree), m_CanModify(canModify){}
    
    
    /// @brief Makes data point to str and gives it a size/capacity to use depending on canModify. Will complete buffer if canFree is true
    /// @param str 
    /// @param len the amount of characters or size in the param str
    /// @param canFree 
    /// @param canModify decides if the buffer can directly modify data or if it has to make a copy if it needs to edit data
    HBuffer(const char* str, size_t len, bool canFree, bool canModify)HBUFF_NOEXCEPT:m_Size(len), m_Data(const_cast<char*>(str)), m_Capacity(m_Size + 1), m_CanFree(canFree), m_CanModify(canModify){}
    
    /// @brief Makes data point to str and gives it a size/capacity to use depending on canModify. Will complete buffer if canFree is true
    /// @param str 
    /// @param len the amount of characters or size in the param str
    /// @param capacity the amount of characters or size in the param str
    /// @param canFree 
    /// @param canModify decides if the buffer can directly modify data or if it has to make a copy if it needs to edit data
    explicit HBuffer(char* str, size_t len, size_t capacity, bool canFree, bool canModify)HBUFF_NOEXCEPT:m_Size(len), m_Data(const_cast<char*>(str)), m_Capacity(capacity), m_CanFree(canFree), m_CanModify(canModify){}
    
    /// @brief Makes the buffer an exact non owning copy of param buffer
    /// @param buffer 
    HBuffer(const HBuffer& buffer)HBUFF_NOEXCEPT :m_Data(buffer.m_Data), m_Size(buffer.m_Size), m_Capacity(buffer.m_Capacity), m_CanFree(false), m_CanModify(buffer.m_CanModify){
        //We assume @param buffer owns the data and will manage it properly
    }
    /// @brief Moves param buffer into self and releases param buffers data
    /// @param buffer the buffer to get data from and release
    HBuffer(HBuffer&& buffer) HBUFF_NOEXCEPT{
        m_Data = buffer.m_Data;
        m_Size = buffer.m_Size;
        m_Capacity = buffer.m_Capacity;
        m_CanFree = buffer.m_CanFree;
        m_CanModify = buffer.m_CanModify;
        buffer.Release();
    }

    /// @brief sets buffer as non owning view of string literal
    HBuffer(const char* str) HBUFF_NOEXCEPT{
        m_Data = const_cast<char*>(str);
        m_Size = strlen(str);
        m_Capacity = m_Size + 1;
        m_CanFree = false;
        m_CanModify = false;
    }
    /// @brief Makes a copy of the string
    /// @param string 
    explicit HBuffer(const std::string& str) HBUFF_NOEXCEPT{
        m_Capacity = strlen(str.c_str());
        m_Size = m_Capacity;
        m_Data = new char[m_Capacity + 1];
        m_Data[m_Capacity] = '\0';
        m_CanFree = true;
        m_CanModify = true;

        memcpy(m_Data, str.c_str(), m_Size);
    }

    /// @brief will Free data if owns
    ~HBuffer(){
    #ifdef HBUFFER_PRINT_DECONSTRUCTION_STATUS
        std::cout << "HBuffer Deconstructor Before Free" <<std::endl;
        if(m_CanFree)delete m_Data;
        std::cout<< "HBuffer Deconstructor After Free" << std::endl;
    #else
        if(m_CanFree)delete m_Data;
    #endif
    }

    /// @brief Frees data if can. Only modifies m_CanFree and m_Data
    inline void Delete() HBUFF_NOEXCEPT{
        if(m_CanFree)delete m_Data;
        m_Data = nullptr;
        m_CanFree = false;
    }
    /// @brief Frees data if buffer is owning and releases after regardless
    inline void Free() HBUFF_NOEXCEPT{  
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
    inline void Release() HBUFF_NOEXCEPT{
        m_Data = nullptr;
        m_Size = 0;
        m_Capacity = 0;
        m_CanFree = false;
        m_CanModify = false;
    }
    /// @brief only changes m_Data pointer without freeing/resizing.
    void SetData(char* data) HBUFF_NOEXCEPT{
        m_Data = data;
    }
    /// @brief changes if we own the data or not
    void SetCanFree(bool value) HBUFF_NOEXCEPT{
        m_CanFree = value;
    }

    /// @brief changes if we are allowed to modify the data or not. without nullptr checks
    void SetCanModify(bool canModify) HBUFF_NOEXCEPT{
        m_CanModify = canModify;
    }

    /// @brief Sets the size of the buffer and reallocates and changes data if param size > m_Capacity
    /// @param size the size to set to
    void SetSize(size_t size) HBUFF_NOEXCEPT{
        if(size > m_Capacity){
            char* newData = new char[size];
            memcpy(newData, m_Data, m_Capacity);
            if(m_CanFree)m_Data = newData;
            m_CanFree = true;
            m_CanModify = true;
            m_Capacity = size;
        }
        m_Size = size;
    }

    /// @brief Reserves the buffer to be atleast param newSize bytes. If newSize <= capacity then no reallocation is done. Else we free/release data and reallocate
    /// @param newSize 
    void Reserve(size_t newSize) HBUFF_NOEXCEPT{
        if(newSize <= m_Capacity)return;
        char* data = new char[newSize];
        memcpy(data, m_Data, m_Capacity);
        if(m_CanFree)delete m_Data;
        m_Data = data;
        m_Capacity = newSize;
        m_CanFree = true;
        m_CanModify = true;
        m_Capacity = newSize;
    }


    /// @brief Reserves param newSize + 1 bytes in memory. With the additional byte being the null terminator.
    void ReserveString(size_t capacity) HBUFF_NOEXCEPT{
        capacity++;
        if(capacity < m_Capacity)return;
        char* data = new char[capacity];
        memcpy(data, m_Data, m_Size);
        memset(data+m_Size, '\0', 1);
        memset(data+capacity - 1, '\0', 1);
        Delete();
        m_Data = data;
        m_Capacity = capacity;
        m_CanModify = true;
        m_CanFree = true;
    }
    
    /// @return returns the character at i without safety checks
    char At(size_t i) const HBUFF_NOEXCEPT{
        return m_Data[i];
    }
    /// @return returns the character at i. returns '\0' if out of range
    char Get(size_t i) const HBUFF_NOEXCEPT{
        if(i < m_Size)return m_Data[i];
        return '\0';
    }

    /// @brief Allocate a copy of data
    static HBuffer CreateCopy(const std::string& string) HBUFF_NOEXCEPT{
        size_t size = string.size();
        size_t newCapacity = size + 1;
        char* data = new char[newCapacity];
        memcpy(data, string.data(), size);
        memset(data + size, '\0', 1);
        return HBuffer(data, size, newCapacity, true, true);
    }
    /// @brief returns a new HBuffer with an ascii encoded base 10 string
    static HBuffer ToString(size_t number) HBUFF_NOEXCEPT{
        HBuffer buffer;
        buffer.ReserveString(5);

        while(number > 0){
            buffer.AppendString((number % 10) + '0');
            number /= 10;
        }
        buffer.Reverse();

        return buffer;
    }

    static HBuffer ToString(float number) HBUFF_NOEXCEPT{
        HBuffer buffer;
        buffer.ReserveString(7);

        uint32_t bits = *reinterpret_cast<uint32_t*>(&number);
        uint8_t sign = bits >> 31;
        uint8_t exponent = ((bits >> 23) & 0xFF) - 127; // Bias correction for IEEE-754

        if(sign == 1)buffer.AppendString('-');
        if(exponent == 0 || exponent == 255){
            buffer.AppendString("Inf");
            return buffer;
        }
        int mantissa = bits & 0x7FFFFF;
        int pow10 = exponent * 30103 / 100000;
        return buffer;
    }


    /// @brief Assumes data in param buffer is a ascii encoded numerical base 10 string.
    /// @param output a pointer to a size_t to store the result
    static void StrictToNumber(const HBuffer& buffer, size_t* output) HBUFF_NOEXCEPT{
        size_t number = 0;

        for(size_t i = 0; i < buffer.GetSize(); i++){
            number *= 10;
            number += buffer.m_Data[i] - '0';
        }

        *output = number;
    }
    /// @brief Makes buffer point to a null terminated string literal
    /// @param str the string to point to
    /// @param canFree do we own this data
    /// @param canModify can we modify this data
    void Assign(const char* str, bool canFree, bool canModify) HBUFF_NOEXCEPT{
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
    void Assign(const char* str, size_t len, bool canFree, bool canModify) HBUFF_NOEXCEPT{
        Free();
        m_Data = const_cast<char*>(str);
        m_Size = len;
        m_Capacity = len;
        m_CanFree = canFree;
        m_CanModify = canModify;
    }


    /// @brief frees and assigns self as a non owning view of param buffer. May modify if param buffer allows modifying
    void Assign(const HBuffer& buffer) HBUFF_NOEXCEPT{
        Free();
        m_Data = buffer.m_Data;
        m_Size = buffer.m_Size;
        m_Capacity = buffer.m_Capacity;
        m_CanModify = buffer.m_CanModify;
        m_CanFree = false;
    }

    /// @brief Frees data and assigns data to param buffer. Then param buffer is released. Essentially making this buffer the owner
    void Assign(HBuffer&& buffer) HBUFF_NOEXCEPT{
        Free();
        m_Data = buffer.m_Data;
        m_Size = buffer.m_Size;
        m_Capacity = buffer.m_Capacity;
        m_CanModify = buffer.m_CanModify;
        m_CanFree = buffer.m_CanFree;
        buffer.Release();
    }

    /// @brief Sets buffer to a non owning view that has the same properties of the param buffer
    /// @param buffer 
    /// @param canFree are we allowing the buffer to now own the data of param buffer? 
    /// @param canModify are we going to allow the buffer to modify the contents of the new data from param buffer?
    void Assign(HBuffer& buffer, bool canFree, bool canModify) HBUFF_NOEXCEPT{
        Free();
        /*
        m_Data = buffer.m_Data;
        m_Size = buffer.m_Size;
        m_Capacity = buffer.m_Capacity;
        m_CanModify = buffer.m_CanModify;
        m_CanFree = false;
        */
        m_Data = buffer.m_Data;
        m_Size = buffer.m_Size;
        m_Capacity = buffer.m_Capacity;
        m_CanFree = canFree;
        m_CanModify = canModify;
    }

    /// @brief We will append the "foods" data to our buffer and the foods data will get released
    void Consume(HBuffer& food) HBUFF_NOEXCEPT{
        Append(food);
        food.Free();
    }
    /// @param offset adds offset to buffer as if +=
    void Consume(size_t from, HBuffer& food) HBUFF_NOEXCEPT{
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

    void Append(const HBuffer& buffer) HBUFF_NOEXCEPT{
        size_t otherSize = buffer.m_Size;
        size_t newSize = m_Size + otherSize;

        if(!m_CanModify || newSize > m_Capacity || !m_Data){
            char* data = new char[newSize];
            memcpy(data, m_Data, m_Size);
            Delete();
            m_Data = data;
            m_Capacity = m_Size;
            m_CanFree = true;
            m_CanModify = true;
        }

        memcpy(m_Data + m_Size, buffer.GetData(), otherSize);
        m_Size = newSize;
    }

    void Append(const char* str) HBUFF_NOEXCEPT{
        size_t strLen = strlen(str);
        size_t newSize = m_Size + strLen;

        if(!m_CanModify || newSize > m_Capacity || !m_Data){
            char* data = new char[newSize];
            memcpy(data, m_Data, m_Size);
            Delete();
            m_Data = data;
            m_CanFree = true;
            m_CanModify = true;
            m_Capacity = newSize;
        }

        memcpy(m_Data + m_Size, str, strLen);
        m_Size = newSize;
    }
    void Append(const char* str, size_t strLen) HBUFF_NOEXCEPT{
        size_t newSize = m_Size + strLen;

        if(!m_CanModify || newSize > m_Capacity || !m_Data){
            char* data = new char[newSize];
            memcpy(data, m_Data, m_Size);
            Delete();
            m_Data = data;
            m_CanFree = true;
            m_CanModify = true;
            m_Capacity = newSize;
        }

        memcpy(m_Data + m_Size, str, strLen);
        m_Size = newSize;
    }

    void Append(char c) HBUFF_NOEXCEPT{
        size_t newSize = m_Size + 1;

        if(!m_CanModify || newSize > m_Capacity || !m_Data){
            char* data = new char[newSize];
            memcpy(data, m_Data, m_Size);
            Delete();
            m_Capacity = newSize;
            m_Data = data;
            m_CanFree = true;
            m_CanModify = true;
        }
        memset(m_Data + m_Size, c, 1);
        m_Size = newSize;
    }

    void Append(const std::string& string){
        size_t strLen = string.size();
        size_t newSize = strLen + m_Size;

        if(!m_CanModify || newSize > m_Capacity || !m_Data){
            char* data = new char[newSize];
            memcpy(data, m_Data, m_Size);
            Delete();
            m_Data = data;
            m_Capacity = newSize;
            m_CanFree = true;
            m_CanModify = true;
        }
        memcpy(m_Data + m_Size, string.data(), strLen);
        m_Size = newSize;
    }

    void AppendString(const HBuffer& buffer) HBUFF_NOEXCEPT{
        size_t otherSize = buffer.m_Size;
        size_t newSize = m_Size + otherSize;
        size_t minCapacity = newSize + 1;

        if(!m_CanModify || minCapacity > m_Capacity || !m_Data){
            char* data = new char[minCapacity];
            memcpy(data, m_Data, m_Size);
            Delete();
            m_Data = data;
            m_Capacity = m_Size;
            m_CanFree = true;
            m_CanModify = true;
        }

        memcpy(m_Data + m_Size, buffer.GetData(), otherSize);
        memset(m_Data + newSize, '\0', 1);
        m_Size = newSize;
    }

    /// @brief Appends a const char* with a specific length and makes sure buffer ends with a null terminator.
    void AppendString(const char* str) HBUFF_NOEXCEPT{
        size_t strLen = strlen(str);
        size_t newSize = m_Size + strLen;
        size_t minCapacity = newSize + 1;

        if(!m_CanModify || minCapacity > m_Capacity || !m_Data){
            char* data = new char[minCapacity];
            memcpy(data, m_Data, m_Size);
            Delete();
            m_Data = data;
            m_CanFree = true;
            m_CanModify = true;
            m_Capacity = minCapacity;
        }

        memcpy(m_Data + m_Size, str, strLen);
        memset(m_Data + newSize, '\0', 1);
        m_Size = newSize;
    }

    /// @brief Appends a const char* with a specific length and makes sure buffer ends with a null terminator.
    void AppendString(const char* str, size_t strLen) HBUFF_NOEXCEPT{
        size_t newSize = m_Size + strLen;
        size_t minCapacity = newSize + 1;

        if(!m_CanModify || minCapacity > m_Capacity || !m_Data){
            char* data = new char[minCapacity];
            memcpy(data, m_Data, m_Size);
            Delete();
            m_Data = data;
            m_CanFree = true;
            m_CanModify = true;
            m_Capacity = minCapacity;
        }

        memcpy(m_Data + m_Size, str, strLen);
        memset(m_Data + newSize, '\0', 1);
        m_Size = newSize;
    }
    
    void AppendString(const std::string& string){
        size_t strLen = string.size();
        size_t newSize = strLen + m_Size;
        size_t minCapacity = newSize + 1;

        if(!m_CanModify || minCapacity > m_Capacity || !m_Data){
            char* data = new char[minCapacity];
            memcpy(data, m_Data, m_Size);
            Delete();
            m_Data = data;
            m_Capacity = minCapacity;
            m_CanFree = true;
            m_CanModify = true;
        }
        memcpy(m_Data + m_Size, string.data(), strLen);
        m_Size = newSize;
    }
    /// @brief Appends a single character to the buffer and also makes sure there is a null terminator
    void AppendString(char c) HBUFF_NOEXCEPT{
        size_t newSize = m_Size + 1;
        size_t minCapacity = newSize + 1;

        if(!m_CanModify || minCapacity> m_Capacity || !m_Data){
            char* data = new char[minCapacity];
            memcpy(data, m_Data, m_Size);
            Delete();
            m_Data = data;
            m_CanFree = true;
            m_CanModify = true;
            m_Capacity = minCapacity;
        }
        memset(m_Data + m_Size, c, 1);
        memset(m_Data + newSize, '\0', 1);
        m_Size = newSize;
    }

    /// @brief allocates a substring of buffer starting at param at with a length of len.
    /// @param at the location in the buffer that will start filling up the substring
    /// @param len the amount of characters to copy to new buffer. If -1 than whole buffer else caps out on buffer
    HBuffer SubString(size_t at, size_t len) const HBUFF_NOEXCEPT{
        HBuffer buffer;
        buffer.m_Size = std::min(len, m_Size);
        buffer.m_Capacity = buffer.m_Size + 1;
        buffer.m_Data = new char[buffer.m_Capacity];

        for(size_t i = 0; i <= buffer.m_Size; i++)
            buffer.m_Data[i] = m_Data[at++];
        buffer.m_Data[buffer.m_Size] = '\0';
        buffer.m_CanFree = true;
        buffer.m_CanModify = true;
        return buffer;
    }
    
    /// @brief sam as substring without null terminator. allocates a subbuffer of buffer starting at param at with a length of len.
    /// @param at the location in the buffer that will start filling up the substring
    /// @param len the amount of characters to copy to new buffer. If -1 than whole buffer. Caps out on buffer size
    HBuffer SubBuffer(size_t at, size_t len) const HBUFF_NOEXCEPT{
        HBuffer buffer;
        buffer.m_Size = std::min(len, m_Size);
        buffer.m_Capacity = buffer.m_Size + 1;
        buffer.m_Data = new char[buffer.m_Capacity];

        size_t i = 0;
        for(i = 0; i < buffer.m_Capacity; i++){
            buffer.m_Data[i] = m_Data[at];
            at++;
        }
        buffer.m_CanFree = true;
        buffer.m_CanModify = true;
        return buffer;
    }
    /// @brief swaps the contents of self with param buff
    /// @param buff
    void Swap(HBuffer& buff) HBUFF_NOEXCEPT{
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
    void Copy(const char* str)HBUFF_NOEXCEPT{
        m_Size = strlen(str);
        if(m_Size < 1){
            //copy of nothing
            //Free();
            return;
        }
        if(!m_Data || !m_CanModify || m_Size > m_Capacity){
            Delete();
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
    void Copy(char* str, size_t size)HBUFF_NOEXCEPT{
        m_Size = size;
        if(m_Size < 1){
            //copy of nothing
            Free();
            return;
        }
        if(!m_Data || !m_CanModify || m_Size > m_Capacity){
            Delete();

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
    void Copy(const std::string& string)HBUFF_NOEXCEPT{
        m_Size = string.size();
        if(m_Size < 1){
            //Free();
            return;
        }
        if(!m_Data || !m_CanModify || m_Size > m_Capacity){
            Delete();
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
    void Copy(const HBuffer& buff)HBUFF_NOEXCEPT{
        m_Size = buff.m_Size;
        if(m_Size < 1)return;
        if(!m_Data || !m_CanModify || m_Size >= m_Capacity){
            Delete();
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
    void CopyString(const char* str)HBUFF_NOEXCEPT{
        m_Size = strlen(str);
        if(m_Size < 1){
            //Free();
            return;
        }
        if(!m_Data || m_CanModify || m_Size >= m_Capacity){
            Delete();
            m_Capacity = m_Size + 1;
            m_Data = new char[m_Capacity];
            memcpy(m_Data, str, m_Size + 1);
            m_CanFree = true;
            m_CanModify = true;
            return;
        }

        memcpy(m_Data, str, m_Size + 1);
    }
    ///@brief The exact same as Copy(char*, size_t) except we add a null terminator at the end to our buffer without including the null terminator in size/capacity. This essentially makes the buffer a string
    /// @param characters the amount of characters to copy into the buffer. Does not include the null terminator
    void CopyString(char* str, size_t characters)HBUFF_NOEXCEPT{
        m_Size = characters;
        if(m_Size < 1){
            //Free();
            return;
        }
        if(!m_Data || m_CanModify || m_Size >= m_Capacity){
            Delete();
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
    void CopyString(const std::string& string) HBUFF_NOEXCEPT{
        m_Size = string.size();
        if(m_Size < 1){
            //Free();
            return;
        }
        if(!m_Data || m_CanModify || m_Size >= m_Capacity){
            Delete();
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
    void CopyString(const HBuffer& buff) HBUFF_NOEXCEPT{
        m_Size = buff.m_Size;
        if(m_Size < 1){
            //Free();
            return;
        }
        if(!m_Data || m_CanModify || m_Size >= m_Capacity){
            Delete();
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
    bool StartsWith(size_t at, const char* str) const HBUFF_NOEXCEPT{
        size_t i = 0, strLen = strlen(str);

        while(true){
            if(str[i] == '\0')return true;
            if(at >= m_Size)return false;
            if(m_Data[at++] != str[i])return false;
        }

        return true;
    }
    bool StartsWith(size_t at, const char* str, size_t len) const HBUFF_NOEXCEPT{
        size_t i = 0;

        while(true){
            if(str[i] == '\0')return true;
            if(at >= m_Size)return false;
            if(m_Data[at++] != str[i])return false;
        }

        return true;
    }
    bool StartsWith(const char* str) const HBUFF_NOEXCEPT{
        size_t i = 0, strLen = strlen(str);

        while(true){
            if(str[i] == '\0')return true;
            if(i == m_Size)return false;
            if(m_Data[i] != str[i])return false;
            i++;
        }

        return true;
    }
    bool StartsWith(const char* str, size_t len) const HBUFF_NOEXCEPT{
        size_t i = 0;

        while(true){
            if(str[i] == '\0')return true;
            if(i == m_Size)return false;
            if(m_Data[i] != str[i])return false;
            i++;
        }

        return true;
    }

    /// @brief Checks if the buffer ends with a certain string excluding the null terminator.
    /// @return returns if the buffer ends with the c string. Returns true in anycase where the buffer has 0 bytes or the string has 0 bytes
    bool EndsWith(const char* str, size_t len) const HBUFF_NOEXCEPT{
        if(m_Size == 0)return false;
        size_t strAt = len;
        if(strAt == 0)return true;
        strAt--;
        size_t i = m_Size - 1;

        while(true){
            if(str[strAt] != m_Data[i])return false;
            if(strAt == 0)break;
            if(i-- == 0)return false;
            i--;
            strAt--;
        }

        return true;
    }
    
    /// @brief Checks if the buffer ends with a certain string excluding the null terminator.
    /// @return returns if the buffer ends with the c string. Returns true in anycase where the buffer has 0 bytes or the string has 0 bytes
    bool EndsWith(const char* str) const HBUFF_NOEXCEPT{
        if(m_Size == 0)return false;
        size_t strAt = strlen(str);
        if(strAt == 0)return true;
        strAt--;
        size_t i = m_Size - 1;

        while(true){
            if(str[strAt] != m_Data[i])return false;
            if(strAt == 0)break;
            if(i-- == 0)return false;
            i--;
            strAt--;
        }

        return true;
    }

    //TODO: POssible rename
    /// @return returns 0 if success return -1 if buffer is out of data and 1 if data doesnt match
    int StrXCmp(const char* str) const HBUFF_NOEXCEPT{
        size_t i = 0;
        while(true){
            if(str[i] == '\n')return 0;
            if(i == m_Size)return -1;
            if(str[i] != m_Data[i])return 1;
            i++;
        }
        return 0;
    }

    void Memset(char byte, size_t len) HBUFF_NOEXCEPT{
        memset(m_Data, byte, len);
    }
    /// @brief Copies contents of buffer into param dest for len bytes
    void Memcpy(void* dest, size_t len) const HBUFF_NOEXCEPT{
        memcpy(dest, m_Data, len);
    }
    /// @brief Attempts to copy contents of buffer from at into dest for len bytes
    void Memcpy(void* dest, size_t at, size_t len) const HBUFF_NOEXCEPT{
        if(at >= m_Size)return;
        memcpy(dest, m_Data + at, len);
    }

    /// @brief Reverses the data inside the array from 0-m_Size. Turns data at 0 into data at m_Size and data at m_Size into data at 0
    void Reverse() HBUFF_NOEXCEPT{
        for(size_t i = 0; i < m_Size / 2; i++){
            size_t reverseIndex = m_Size - 1 - i;
            char temp = m_Data[i];
            m_Data[i] = m_Data[reverseIndex];
            m_Data[reverseIndex] = temp;
        }
    }

    HBuffer GetSafeString() const HBUFF_NOEXCEPT{
        HBuffer buff;
        buff.AppendString(*this);
        return buff;
    }


    /// @brief Allocates a new c string on the heap with the data of the buffer followed by null terminator without modifying the current buffer.
    /// @return if current buffer is not a c string or is too small to be one we allocate new data and return a new buffer. Else we return a view of the current one.
    HBuffer GetSafeCString() const HBUFF_NOEXCEPT{
        if(m_Capacity < 1 || m_Capacity == m_Size){
            size_t newCapacity = m_Capacity + 1;
            char* data = new char[newCapacity];
            memcpy(data, m_Data, m_Size);
            return HBuffer(data, m_Size, newCapacity, true, true);
        }

        return *this;
        /*
        char* data = new char[m_Size + 1];
        memcpy(data, m_Data, m_Size);
        memset(data + m_Size, '\0', 1);
        return data;*/
    }

    /// @brief Makes sure there is a null terminator at the end of the buffer and returns the buffers data. Might of just made this for nothing
    const char* TurnToSafeCString() HBUFF_NOEXCEPT{
        if(m_Capacity <= m_Size){
            m_Capacity = m_Size + 1;
            char* data = new char[m_Capacity];
            memcpy(data, m_Data, m_Size);
            Delete();
            m_Data = data;
            m_CanFree = true;
            m_CanModify = true;
        }

        memset(m_Data + m_Size, '\0', 1);
        return m_Data;
    }
public:
    /// @brief if data is not null returns that data if it is null we return a non allocated "" literal
    const char* GetCStr() const HBUFF_NOEXCEPT{return m_Data ? m_Data : "";}
    /// @brief returns data ptr
    HBUFF_CONSTEXPR bool CanFree() const HBUFF_NOEXCEPT{return m_CanFree;}
    HBUFF_CONSTEXPR bool CanModify() const HBUFF_NOEXCEPT{return m_CanModify;}
    HBUFF_CONSTEXPR char* GetData() const HBUFF_NOEXCEPT{return m_Data;}
    HBUFF_CONSTEXPR size_t GetSize() const HBUFF_NOEXCEPT{return m_Size;}
    HBUFF_CONSTEXPR size_t GetCapacity() const HBUFF_NOEXCEPT{return m_Capacity;}
public:
    /*
    /// @brief Frees data and sets data to a non owning view of param right's data
    HBuffer& operator=(HBuffer& right) HBUFF_NOEXCEPT{
        Free();
        m_Data = right.m_Data;
        m_Size = right.m_Size;
        m_Capacity = right.m_Capacity;
        m_CanModify = right.m_CanModify;
        m_CanFree = false;
        return *this;
    }*/

    char& operator[](size_t at) HBUFF_NOEXCEPT{
        return m_Data[at];
    }
    /// @brief Assigns the current buffers content as a copy of param right's data
    HBuffer& operator=(const HBuffer& right) HBUFF_NOEXCEPT{
        Free();
        m_Data = right.m_Data;
        m_Size = right.m_Size;
        m_Capacity = right.m_Capacity;
        m_CanModify = right.m_CanModify;
        m_CanFree = false;
        return *this;
    }
    
    /// @brief Frees current data if possible and basically does a move constructor with param right
    HBuffer& operator=(HBuffer&& right) HBUFF_NOEXCEPT{
        Free();
        m_Data = right.m_Data;
        m_Size = right.m_Size;
        m_Capacity = right.m_Capacity;
        m_CanFree = right.m_CanFree;
        m_CanModify = right.m_CanModify;
        right.Release();
        return *this;
    }
    /// @brief assigns buffer to a non owning string literal
    HBuffer& operator=(const char* right) HBUFF_NOEXCEPT{
        Free();
        m_Data = const_cast<char*>(right);
        m_Size = strlen(right);
        m_Capacity = m_Size + 1;
        m_CanFree = false;
        m_CanModify = false;
        return *this;
    }

    friend HBuffer operator+(const char* left, const HBuffer& right) noexcept{
        size_t strLen = strlen(left);
        if(strLen + right.m_Size < 1)return HBuffer();
        HBuffer buffer;
        size_t newSize = strLen + right.m_Size;
        buffer.Reserve(newSize + 1);
        buffer.SetSize(newSize);
        char* data = buffer.GetData();
        memcpy(data, left, strLen);
        memcpy(data + strLen, right.m_Data, right.m_Size);
        memset(data + newSize, '\0', 1);
        return buffer;
    }
    /// @brief appends data as ascii strings
    HBuffer& operator+=(const char* right) HBUFF_NOEXCEPT{
        size_t strLen = strlen(right);
        size_t newSize = m_Size + strLen;

        if(!m_CanFree || !m_CanModify || newSize >= m_Capacity){
            char* data = new char[newSize];
            memcpy(m_Data, data, m_Size);
            Delete();
            m_Capacity = newSize;
        }
        memcpy(m_Data + m_Size, right, strLen);
        memset(m_Data, newSize-1,'\0');
        m_Size = newSize;
        return *this;
    }
    
    /// @brief appends data as ascii strings
    HBuffer& operator+=(const HBuffer& right) HBUFF_NOEXCEPT{
        size_t strLen = right.GetSize();
        size_t newSize = m_Size + strLen + 1;

        if(!m_CanFree || !m_CanModify || newSize >= m_Capacity){
            char* data = new char[newSize];
            memcpy(m_Data, data, m_Size);
            Delete();
            m_Capacity = newSize;

        }
        memcpy(m_Data + m_Size, right.GetData(), strLen);
        memset(m_Data, newSize-1,'\0');
        m_Size = newSize;
        return *this;
    }
    /// @brief adds an offset to the vector. If owns data it frees and reallocates. If not then we just increment pointer and change size
    ///HBuffer& operator+=(size_t offset) HBUFF_NOEXCEPT;
    //TODO:maybe possible -= operator if needed

    /// @brief compares if the data inside the buffer matches and not the places in memory.
    HBUFF_CONSTEXPR bool operator==(const HBuffer& right)const HBUFF_NOEXCEPT{
        if(m_Size != right.m_Size)return false;

        //Assuming if it has a size it has valid memory if not then the user created the buffer wrong
        //if(!m_Data || !right.m_Data)return false;

        const char* other = right.m_Data;
        for(size_t i = 0; i < m_Size; i++)
            if(m_Data[i] != other[i])return false;
        return true;
    }
    /// @brief compares if the data inside the buffers are strings and match
    HBUFF_CONSTEXPR bool operator==(const char* str)const HBUFF_NOEXCEPT{
        size_t strLen = 0;
        while(str[strLen] != '\0')strLen++;
        if(m_Size != strLen)return false;

        for(size_t i = 0; i < m_Size; i++)
            if(m_Data[i] != str[i])return false;
        //Never getting called
        return true;
    }
    /// @brief returns if the contents are not equal. If one of the buffers does not have data returns false; else returns if contents match.
    HBUFF_CONSTEXPR bool operator!=(const HBuffer& right)const HBUFF_NOEXCEPT{
        if(m_Size != right.m_Size)return true;
        if(!m_Data || !right.m_Data)return false;

        const char* other = right.m_Data;
        for(size_t i = 0; i < m_Size; i++)
            if(m_Data[i] != other[i])return true;
        return false;
    }
private:
    char* m_Data = nullptr;
    size_t m_Size = 0;
    size_t m_Capacity = 0;
    bool m_CanFree = false;
    bool m_CanModify = false;
};

namespace std {
    template<>
    struct hash<HBuffer> {
        std::size_t operator()(const HBuffer& buff) const noexcept{
            std::size_t hash = 0;
            std::size_t prime = 31; // A small prime number

            char* data = buff.GetData();
            for (size_t i = 0; i < buff.GetSize(); i++)
                hash = hash * prime + data[i];

            return hash;
        }
    };
}

std::ostream& operator<<(std::ostream& os, const HBuffer& buffer) {
    os << buffer.GetCStr();
    return os;
}