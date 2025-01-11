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
        : m_Size(strlen(str)), m_Capacity(m_Size), m_Data(const_cast<char*>(str)), m_CanFree(canFree), m_CanModify(canModify){}
    
    
    /// @brief Makes data point to str and gives it a size/capacity to use depending on canModify. Will complete buffer if canFree is true
    /// @param str 
    /// @param len 
    /// @param canFree 
    /// @param canModify decides if the buffer can directly modify data or if it has to make a copy if it needs to edit data
    HBuffer(const char* str, size_t len, bool canFree, bool canModify)HBUFF_NOEXCEPT:m_Size(len), m_Data(const_cast<char*>(str)), m_Capacity(m_Size), m_CanFree(canFree), m_CanModify(canModify){}

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
        m_Capacity = strlen(str);
        m_Size = m_Capacity;
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
        Free();
        std::cout<< "HBuffer Deconstructor After Free" << std::endl;
    #else
        Free();
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


    /// @brief Reserves the buffer to have atleast newSize bytes. Only reallocate if newSize >= capacity.Fills remaining space after size with ' ' and ends with a null terminator
    void ReserveString(size_t newSize) HBUFF_NOEXCEPT{
        if(newSize < m_Capacity)return;
        char* data = new char[newSize + 1];
        memcpy(data, m_Data, m_Capacity);
        memset(data + m_Capacity, ' ', newSize - m_Capacity);
        memset(data, '\0', newSize);
        if(m_CanFree)delete m_Data;
        m_Data = data;
        m_Capacity = newSize;
        m_CanFree = true;
        m_CanModify = true;
        m_Capacity = newSize;
    }
    /*
    /// @brief Retrieves the char at param i. Throws std::out_of_range if i is out of buffer range/size
    char Get(size_t i) const HBUFF_NOEXCEPT;
    */

    /// @return returns the character at i
    char At(size_t i) const HBUFF_NOEXCEPT{
        return m_Data[i];
    }
    /// @return returns the character at i. returns '\0' if out of range
    char Get(size_t i) const HBUFF_NOEXCEPT{
        if(i < m_Size)return m_Data[i];
        return '\0';
    }

    //Assign data to point to a string literal.
    //TODO: maybe capacity


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

    void Append(HBuffer& buffer) HBUFF_NOEXCEPT{
        size_t newSize = m_Size + buffer.m_Size;

        if(!m_CanModify || m_Size >= m_Capacity || !m_Data){
            char* data = new char[newSize];
            memcpy(data, m_Data, m_Size);
            Free();
            m_Data = data;
            m_Capacity = m_Size;
            m_CanFree = true;
            m_CanModify = true;
        }

        memcpy(m_Data, m_Data + m_Size, buffer.m_Size);
        m_Size = newSize;
    }

    /// @brief allocates a substring of buffer starting at param at with a length of len.
    /// @param at the location in the buffer that will start filling up the substring
    /// @param len the amount of characters to copy to new buffer. If -1 than whole buffer else caps out on buffer
    HBuffer SubString(size_t at, size_t len) const HBUFF_NOEXCEPT{
        HBuffer buffer;
        buffer.m_Capacity = std::min(len, m_Size);
        buffer.m_Data = new char[buffer.m_Capacity + 1];

        size_t i = 0;
        for(i = 0; i < buffer.m_Capacity; i++){
            buffer.m_Data[i] = m_Data[at];
            at++;
        }
        buffer.m_Data[buffer.m_Capacity] = '\0';
        buffer.m_Size = i;
        buffer.m_CanFree = true;
        buffer.m_CanModify = true;
        return buffer;
    }
    
    /// @brief sam as substring without null terminator. allocates a subbuffer of buffer starting at param at with a length of len.
    /// @param at the location in the buffer that will start filling up the substring
    /// @param len the amount of characters to copy to new buffer. If -1 than whole buffer. Caps out on buffer size
    HBuffer SubBuffer(size_t at, size_t len) const HBUFF_NOEXCEPT{
        HBuffer buffer;
        buffer.m_Capacity = std::min(len, m_Size);
        buffer.m_Data = new char[buffer.m_Capacity];

        size_t i = 0;
        for(i = 0; i < buffer.m_Capacity; i++){
            buffer.m_Data[i] = m_Data[at];
            at++;
        }
        buffer.m_Size = i;
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
            std::cout<<m_Capacity<<std::endl;
            std::cout<<(const char*)buff.m_Data<<std::endl;
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
        size_t i = 0, strLen = len;

        while(true){
            if(str[i] == '\0')return true;
            if(i == m_Size)return false;
            if(m_Data[i] != str[i])return false;
            i++;
        }

        return true;
    }
    
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

    /// @brief Copies contents of buffer into param dest for len bytes
    void Memcpy(void* dest, size_t len) const HBUFF_NOEXCEPT{
        memcpy(dest, m_Data, len);
    }
    /// @brief Attempts to copy contents of buffer from at into dest for len bytes
    void Memcpy(void* dest, size_t at, size_t len) const HBUFF_NOEXCEPT{
        if(at >= m_Size)return;
        memcpy(dest, m_Data + at, len);
    }
public:
    /// @brief if data is not null returns that data if it is null we return a non allocated "" literal
    const char* GetCStr() const HBUFF_NOEXCEPT{
        if(m_Data)return m_Data;
        return "";
    }
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
    
    /// @brief Assigns the current buffers content as a copy of param right's data
    HBuffer& operator=(const HBuffer& right) HBUFF_NOEXCEPT{
        m_Size = right.m_Size;
        if(!m_Data || !m_CanModify || m_Size >= m_Capacity){
            Delete();
            m_Capacity = m_Size;
            char* data = new char[m_Capacity];
            memcpy(data, right.m_Data, m_Capacity);
            m_Data = data;
            m_CanFree = true;
            m_CanModify = true;
            return *this;
        }
        //Copy data into buff
        memcpy(m_Data, right.m_Data, m_Size);
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
        m_Capacity = strlen(right);
        m_Size = m_Capacity;
        m_CanFree = false;
        m_CanModify = false;
        return *this;
    }
    /// @brief adds an offset to the vector. If owns data it frees and reallocates. If not then we just increment pointer and change size
    ///HBuffer& operator+=(size_t offset) HBUFF_NOEXCEPT;
    //TODO:maybe possible -= operator if needed

    /// @brief compares if the data inside the buffer matches and not the places in memory.
    HBUFF_CONSTEXPR bool operator==(const HBuffer& right)const HBUFF_NOEXCEPT{
        //if(m_Data != right.m_Data)return false;
        //if(m_Size != right.m_Size)return false;
        //for(size_t i = 0; i < m_Size; i++)
        //    if(m_Data[i] != right.m_Data[i])return false;
        //return true;
        if(m_Size != right.m_Size)return false;
        if(!m_Data || !right.m_Data)return false;

        const char* other = right.m_Data;
        for(size_t i = 0; i < m_Size; i++)
            if(m_Data[i] != other[i])return false;
        return true;
    }
    /// @brief compares if the data inside the buffers are strings and match
    HBUFF_CONSTEXPR bool operator==(const char* str)const HBUFF_NOEXCEPT{
        size_t i = 0;
        while(true){
            char c = m_Data[i];
            if(c != str[i++])return false;
            if(c == '\0')return true;
        }
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