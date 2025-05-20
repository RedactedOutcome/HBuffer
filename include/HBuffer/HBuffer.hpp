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
    /// @param size the new buffer size in bytes
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


    /// @brief assigns the size of the buffer to the param size without doing anything else. Doesnt check if its bigger than capacity or if we have valid data or anything.
    /// @brief ;aram the new size in bytes
    void AssignSize(size_t size) HBUFF_NOEXCEPT{
        m_Size = size;
    }

    /// @brief strictly assigns the buffers capacity to the param capacity with no questions asked.
    /// @param capacity the new capacity of the buffer in bytes
    void AssignCapacity(size_t capacity) HBUFF_NOEXCEPT{
        m_Capacity = capacity;
    }
    /// @brief Reserves the buffer to be atleast param newSize bytes. If newSize <= capacity then no reallocation is done. Else we free/release data and reallocate
    /// @param newCapacity the new capacity of the buffer. Only reallocates if newCapacity > m_Capacity 
    void Reserve(size_t newCapacity) HBUFF_NOEXCEPT{
        if(newCapacity <= m_Capacity)return;

        char* data = new char[newCapacity];
        memcpy(data, m_Data, m_Size);
        if(m_CanFree)delete m_Data;
        m_Data = data;
        m_Capacity = newCapacity;
        m_CanFree = true;
        m_CanModify = true;
    }


    /// @brief Reserves param newCapacity + 1 bytes in memory. With the additional byte being the null terminator.
    void ReserveString(size_t newCapacity) HBUFF_NOEXCEPT{
        newCapacity++;
        if(newCapacity < m_Capacity)return;
        m_Capacity = newCapacity;
        char* data = new char[m_Capacity];
        memcpy(data, m_Data, m_Size);
        memset(data + m_Capacity - 1, '\0', 1);
        if(m_CanFree)delete m_Data;
        m_Data = data;
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

    /// @brief returns the character at param at.
    /// @param status true if success, false if failed to retrieve
    /// @param at the position in the buffer to retrieve the byte from
    char Retrieve(bool& status, size_t at) const noexcept{
        if(at > m_Size){
            status = false;
            return 0;
        }

        status = true;
        return m_Data[at];
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
    /// @param len the new length and capacity in bytes
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

    /// @brief Frees current data and assigns new data
    /// @param data the new data to point to
    /// @param len the buffers new length in bytes
    /// @param capacity the buffers new capacity in bytes
    /// @param canFree the buffers ability to free the data
    /// @param canModify the buffers ability to modify its new data
    void Assign(char* data, size_t len, size_t capacity, bool canFree, bool canModify) HBUFF_NOEXCEPT{
        Free();
        m_Data = data;
        m_Size = len;
        m_Capacity = capacity;
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
    void Consume(const HBuffer& food) HBUFF_NOEXCEPT{
        Append(food);
    }
    /// @brief appends other buffers data ontop of current buffer and jumps to from. for example Consume(2, "Hello") if buffer is currently empty then new buffer will be "ello", else it will be buff1 + buff2 with the buffer starting at byte 2
    /// @param from how much bytes to consume. then filled with remaining of current buffer then appends the food
    void Consume(size_t from, HBuffer& food) HBUFF_NOEXCEPT{
        ///TODO: might sometime work
        //Part1 size - 30
        //Part2 size - 50
        //Total size - 80
        //Part1 Change - 0
        //From - 29
        //Desired new size - 80
        size_t otherSize = food.m_Size;
        size_t totalSize = m_Size + otherSize;

        size_t part1Change = std::min(from, m_Size);
        size_t newPart1Size = (m_Size - part1Change);
        size_t part2Change = std::min(from <= m_Size ? 0 : from - m_Size, otherSize);
        size_t newPart2Size = otherSize - part2Change;
        size_t newBuffSize = newPart1Size + newPart2Size;
        
        if(newBuffSize > m_Capacity || !m_CanModify || !m_Data){
            m_Capacity = newBuffSize;
            char* newData = new char[newBuffSize];
            memcpy(newData, m_Data + from, newPart1Size);
            if(m_CanFree)delete m_Data;
            m_Data = newData;
            m_CanFree = true;
            m_CanModify = true;
        }
        else
            memcpy(m_Data, m_Data + from, newPart1Size);
        memcpy(m_Data + newPart1Size, food.m_Data + part2Change, newPart2Size);
        m_Size = newBuffSize;
    }

    void InsertAt(size_t at, const HBuffer& buffer) noexcept{
        size_t otherSize = buffer.GetSize();
        size_t minimumSize = at + otherSize;
        if(minimumSize > m_Capacity || !m_CanModify || !m_Data){
            char* newData = new char[minimumSize];
            memcpy(newData, m_Data, m_Size);
            Delete();
            m_Data = newData;
            m_CanFree = true;
            m_CanModify = true;
            m_Capacity = minimumSize;
        }

        memcpy(m_Data + at, buffer.GetData(), otherSize);
    }
    /// @brief Inserts a null terminated string
    /// @param at the place inside the buffer to insert the string into
    /// @param str the string to copy characters from
    /// @param characters the amount of actual characters excluding the null terminator
    void InsertAt(size_t at, const char* str, size_t characters) noexcept{
        size_t minimumSize = at + characters + 1;
        if(minimumSize > m_Capacity || !m_CanModify || !m_Data){
            char* newData = new char[minimumSize];
            memcpy(newData, m_Data, m_Size);
            Delete();
            m_Data = newData;
            m_CanFree = true;
            m_CanModify = true;
            m_Capacity = minimumSize;
        }

        memcpy(m_Data + at, str, characters);
        memset(m_Data + at + characters, 0, 1
    }
    /// @brief Inserts c into the buffer at param at.
    /// @param at the position to insert the char at. If we cant access the buffer or at >= m_Capacity we reallocate and the new buffers size is at + 1.
    /// @param c the byte to insert at c
    void InsertInt8At(size_t at, int8_t c)HBUFF_NOEXCEPT{
        size_t minimumSize = at + 1;
        if(minimumSize >= m_Capacity || !m_CanModify || !m_Data){
            char* newData = new char[minimumSize];
            memcpy(newData, m_Data, m_Size);
            Delete();
            m_Capacity = minimumSize;
            m_Data = newData;
            m_CanFree = true;
            m_CanModify = true;
        }

        0[m_Data + at] = c;
    }

    /// @brief Inserts c into the buffer at param at.
    /// @param at the position to insert the char at. If we cant access the buffer or at >= m_Capacity we reallocate and the new buffers size is at + 1.
    /// @param c the byte to insert at c
    void InsertInt16At(size_t at, int16_t c)HBUFF_NOEXCEPT{
        if(at + 2 >= m_Capacity || !m_CanModify || !m_Data){
            char* newData = new char[at + 2];
            memcpy(newData, m_Data, m_Size);
            Delete();
            m_Data = newData;
            m_CanFree = true;
            m_CanModify = true;
        }

        char* insertAt = m_Data + at;
        #if HBUFF_ENDIAN_MODE == 0
        insertAt[0] = c & 0xFF;
        insertAt[1] = (c >> 8) & 0xFF;
        #else
        insertAt[0] = (c >> 8) & 0xFF;
        insertAt[1] = c & 0xFF;
        #endif
    }

    /// @brief Inserts c into the buffer at param at.
    /// @param at the position to insert the char at. If we cant access the buffer or at >= m_Capacity we reallocate and the new buffers size is at + 1.
    /// @param c the byte to insert at c
    void InsertInt32At(size_t at, int32_t c)HBUFF_NOEXCEPT{
        if(at + 4 >= m_Capacity || !m_CanModify || !m_Data){
            char* newData = new char[at + 4];
            memcpy(newData, m_Data, m_Size);
            Delete();
            m_Data = newData;
            m_CanFree = true;
            m_CanModify = true;
        }

        char* insertAt = m_Data + at;
        #if HBUFF_ENDIAN_MODE == 0
        insertAt[0] = c & 0xFF;
        insertAt[1] = (c >> 8) & 0xFF;
        insertAt[2] = (c >> 16) & 0xFF;
        insertAt[2] = (c >> 24) & 0xFF;
        #else
        insertAt[0] = (c >> 24) & 0xFF;
        insertAt[1] = (c >> 16) & 0xFF;
        insertAt[2] = (c >> 8) & 0xFF;
        insertAt[3] = c & 0xFF;
        #endif
    }

    void Append(const HBuffer& buffer) HBUFF_NOEXCEPT{
        size_t otherSize = buffer.m_Size;
        size_t newSize = m_Size + otherSize;
        
        if(!m_CanModify || newSize > m_Capacity || !m_Data){
            char* data = new char[newSize];
            memcpy(data, m_Data, m_Size);
            Delete();
            m_Data = data;
            m_Capacity = newSize;
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

        for(size_t i = 0; i < buffer.m_Size; i++)
            buffer.m_Data[i] = m_Data[at++];
        buffer.m_Data[buffer.m_Size] = '\0';
        buffer.m_CanFree = true;
        buffer.m_CanModify = true;
        return buffer;
    }

    /// @brief Allocates a new copy of the buffer up to the size of the buffer
    HBuffer GetCopy() const HBUFF_NOEXCEPT{
        char* data = new char[m_Size];
        memcpy(data, m_Data, m_Size);
        return HBuffer(data, m_Size, true, true);
    }

    /// @brief Allocates a copy of the buffer with a null terminator
    HBuffer GetCopyString() const HBUFF_NOEXCEPT{
        HBuffer buff;
        buff.ReserveString(m_Size);
        buff.Copy(*this);
        return buff;
    }

    /// @brief Create a new HBuffer that points to the same data as the current one but with an offset and or different size
    /// @param allowModify a check to allow this subpointer to modify data as long as the main buffer also can modify this data
    HBuffer SubPointer(size_t at, size_t len, bool allowModify = true) const noexcept{
        return HBuffer(m_Data + at, std::min(m_Size - at, len), false, allowModify && m_CanModify);
    }

    /// @brief sam as substring without null terminator. allocates a subbuffer of buffer starting at param at with a length of len.
    /// @param at the location in the buffer that will start filling up the substring
    /// @param len the amount of characters to copy to new buffer. If -1 than whole buffer. Caps out on buffer size
    HBuffer SubBuffer(size_t at, size_t len) const HBUFF_NOEXCEPT{
        HBuffer buffer;
        //TODO: make sure at isnt greater than buffer size
        buffer.m_Size = std::min(m_Size - at, len);
        buffer.m_Capacity = buffer.m_Size;
        buffer.m_Data = new char[buffer.m_Capacity];

        for(size_t i = 0; i < buffer.m_Size; i++){
            buffer.m_Data[i] = m_Data[at++];
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
    /// @brief makes data at param at point to a copy of the null terminated string literal. Frees and reallocates if no data, cant modify, or strlen > capacity.
    /// @param at the position in the buffer to copy the data to. Will reallocate if at is greater than capacity
    /// @param str the null ternimated string literal we are copying
    void Copy(size_t at, const char* str)HBUFF_NOEXCEPT{
        size_t strLen = strlen(str);
        size_t minimumSize = at + strLen;
        if(minimumSize > m_Size || !m_CanModify || !m_Data){
            m_Capacity = minimumSize;
            char* data = new char[minimumSize];
            memcpy(data, m_Data, m_Size);
            memcpy(data + at, str, strLen);
            Delete();
            m_Data = data;
            m_Size = minimumSize;
            m_CanModify = true;
            m_CanFree = true;
            return;
        }

        memcpy(m_Data + at, const_cast<char*>(str), strLen);
        m_Size = minimumSize > m_Size ? minimumSize : m_Size;
    }
    /// @brief makes data at param at point to a copy of the null terminated string literal. Frees and reallocates if no data, cant modify, or len > capacity.
    /// @param at the position in the buffer to copy the data to. Will reallocate if at is greater than capacity
    /// @param str the null ternimated string literal we are copying
    void Copy(size_t at, char* str, size_t len)HBUFF_NOEXCEPT{
        size_t minimumSize = at + len;
        if(minimumSize > m_Size || !m_CanModify || !m_Data){
            m_Capacity = minimumSize;
            char* data = new char[minimumSize];
            memcpy(data, m_Data, m_Size);
            memcpy(data + at, str, len);
            Delete();
            m_Data = data;
            m_Size = minimumSize;
            m_CanModify = true;
            m_CanFree = true;
            return;
        }
        
        memcpy(m_Data + at, const_cast<char*>(str), len);
        m_Size = minimumSize > m_Size ? minimumSize : m_Size;
    }
    /// @brief makes buffers data point to the content of the null terminated string.
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
    /// @brief makes the buffers content point to a copy of the contents inside the std::string
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
        if(!m_Data || !m_CanModify || m_Size > m_Capacity){
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
            if(m_Data[at] != str[i])return false;
            at++;
            i++;
        }

        return true;
    }
    bool StartsWith(size_t at, const char* str, size_t len) const HBUFF_NOEXCEPT{
        size_t i = 0;

        while(true){
            if(str[i] == '\0')return true;
            if(at >= m_Size)return false;
            if(m_Data[at] != str[i])return false;
            at++;
            i++;
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
        if(len > m_Size)return false;
        for(size_t i = m_Size - len; i < m_Size; i++){
            if(m_Data[i] != str[0])return false;
            str++;
        }
        return true;
    }
    
    /// @brief Checks if the buffer ends with a certain string excluding the null terminator.
    /// @return returns if the buffer ends with the c string. Returns true in anycase where the buffer has 0 bytes or the string has 0 bytes
    bool EndsWith(const char* str) const HBUFF_NOEXCEPT{
        size_t len = strlen(str);
        if(len > m_Size)return false;
        for(size_t i = m_Size - len; i < m_Size; i++){
            if(m_Data[i] != str[0])return false;
            str++;
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
        m_Size = len;
    }
    /// @brief Copies contents of buffer into param dest for len bytes
    void Memcpy(void* src, size_t len) const HBUFF_NOEXCEPT{
        memcpy(m_Data, src, len);
    }
    /// @brief Attempts to copy contents of buffer from at into dest for len bytes
    void Memcpy(void* src, size_t at, size_t len) const HBUFF_NOEXCEPT{
        memcpy(m_Data, reinterpret_cast<void*>(reinterpret_cast<size_t>(src) + at), len);
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
            m_Data = data;
        }
        memcpy(m_Data + m_Size, right, strLen);
        memset(m_Data, static_cast<int>(newSize-1),'\0');
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
        memset(m_Data, static_cast<int>(newSize-1),'\0');
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
        
        return true;
    }
    /// @brief compares if the data inside the buffers are strings and match
    HBUFF_CONSTEXPR bool operator==(char c)const HBUFF_NOEXCEPT{
        if(m_Size != 1)return false;
        return m_Data[0] == c;
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
    /// @brief returns if the contents are not equal. If one of the buffers does not have data returns false; else returns if contents match.
    HBUFF_CONSTEXPR bool operator!=(const char* str)const HBUFF_NOEXCEPT{
        size_t strLen = 0;
        while(str[strLen] != '\0')strLen++;
        if(m_Size != strLen)return true;

        for(size_t i = 0; i < m_Size; i++)
            if(m_Data[i] != str[i])return true;

        return false;
    }
    /// @brief returns if the contents are not equal. If one of the buffers does not have data returns false; else returns if contents match.
    HBUFF_CONSTEXPR bool operator!=(char c)const HBUFF_NOEXCEPT{
        if(m_Size != 1)return true;
        return m_Data[0] != c;
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