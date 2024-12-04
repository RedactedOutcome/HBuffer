#include "pch.h"
#include "HBuffer.h"
#include "Core/Logger.h"

HBuffer::HBuffer():m_Data(nullptr), m_Size(0), m_Capacity(0), m_CanFree(false){}
HBuffer::HBuffer(const char* str, bool canFree, bool canModify):m_Size(strlen(str)),
m_Data(const_cast<char*>(str)), m_CanFree(canFree), m_CanModify(canModify){}
HBuffer::HBuffer(const char* str, size_t len, bool canFree, bool canModify):m_Size(len), m_Data(const_cast<char*>(str)), m_Capacity(m_Size), m_CanFree(canFree), m_CanModify(canModify){}
HBuffer::HBuffer(const HBuffer& buffer):m_Data(buffer.m_Data), m_Size(buffer.m_Size), m_Capacity(m_Size), m_CanFree(false), m_CanModify(buffer.m_CanModify){
    //We assume @param buffer owns the data and will manage it properly
}
HBuffer::HBuffer(HBuffer&& buff):m_Data(buff.m_Data), m_Size(buff.m_Size), m_Capacity(buff.m_Capacity), m_CanFree(buff.m_CanFree), m_CanModify(buff.m_CanModify){
    buff.Release();
}
HBuffer::HBuffer(const std::string& str){
    m_Capacity = strlen(str.c_str());
    m_Size = m_Capacity;
    m_Data = new char[m_Capacity];
    m_CanFree = true;
    m_CanModify = true;

    memcpy(m_Data, str.c_str(), m_Size);
}
HBuffer::~HBuffer(){
    Free();
}

void HBuffer::SetSize(size_t size) noexcept{
    m_Size = size;
}
void HBuffer::Free(){
    if(m_CanFree){
        #ifdef HBUFFER_TRACK_ALLOCATIONS
        CORE_DEBUG("Freeing HBuffer with size {0}", m_Size);
        #endif
        delete m_Data;
    }
    m_Data = nullptr;
    m_Size = 0;
    m_Capacity = 0;
    m_CanFree = false;
    m_CanModify = false;
}

void HBuffer::Release(){
    m_Data = nullptr;
    m_Capacity = 0;
    m_Size = 0;
    m_CanFree = false;
    m_CanModify = false;
}

char HBuffer::At(size_t i) const noexcept{
    if(i < m_Size)return m_Data[i];
    return '\0';
}

void HBuffer::Swap(HBuffer& buff){
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

void HBuffer::Assign(HBuffer& buffer, bool owns, bool canModify){
    Free();
    m_Data = buffer.m_Data;
    m_Size = buffer.m_Size;
    m_Capacity = buffer.m_Capacity;
    m_CanFree = owns;
    m_CanModify = canModify;
    if(owns)buffer.m_CanFree = false;
}
void HBuffer::Assign(const char* str, bool canFree, bool canModify){
    Free();
    m_Data = const_cast<char*>(str);
    m_Capacity = strlen(str);
    m_Size = m_Capacity;
    m_CanFree = canFree;
    m_CanModify = canModify;
}
void HBuffer::Assign(const char* str, size_t len, bool canFree, bool canModify){
    Free();
    m_Data = const_cast<char*>(str);
    m_Capacity = len;
    m_Size = m_Capacity;
    m_CanFree = canFree;
    m_CanModify = canModify;
}

#pragma region Copies 
void HBuffer::Copy(const char* str){
    m_Size = strlen(str);
    if(m_Size > m_Capacity || !m_CanModify){
        Free();
        
        #ifdef HBUFFER_TRACK_ALLOCATIONS
        CORE_DEBUG("Allocating HBuffer with size {0}", m_Size);
        #endif
        m_Data = new char[m_Size];
        m_Capacity = m_Size;
        m_CanFree = true;
        m_CanModify = true;
    }
    memcpy(m_Data, str, m_Size);
}
void HBuffer::Copy(char* str, size_t size){
    m_Size = size;
    if(m_Size == 0)return;

    if(m_Size > m_Capacity || !m_CanModify){
        //Reallocate
        Free();
        
        #ifdef HBUFFER_TRACK_ALLOCATIONS
        CORE_DEBUG("Allocating HBuffer with size {0}", m_Size);
        #endif
        m_Data = new char[m_Size];
        m_Capacity = m_Size;
        m_CanFree = true;
        m_CanModify = true;
    }
    memcpy(m_Data, str, m_Size);
}
void HBuffer::Copy(const std::string& string){
    m_Size = string.size();
    if(m_Size == 0)return;

    if(m_Size > m_Capacity || !m_CanModify){
        Free();
        
        #ifdef HBUFFER_TRACK_ALLOCATIONS
        CORE_DEBUG("Allocating HBuffer with size {0}", m_Size);
        #endif

        m_Data = new char[m_Size];
        m_Capacity = m_Size;
        m_CanFree = true;
        m_CanModify = true;
    }
    memcpy(m_Data, string.c_str(), m_Size);
}

void HBuffer::Copy(const HBuffer& buff){
    if(buff.m_Data == nullptr)return;
    size_t newSize = buff.GetSize();
    if(newSize > m_Capacity || !m_CanModify){
        Free();
        
        #ifdef HBUFFER_TRACK_ALLOCATIONS
        CORE_DEBUG("Allocating HBuffer with size {0}", m_Size);
        #endif
        m_Capacity = newSize;
        m_Data = new char[m_Capacity];
        m_CanFree = true;
        m_CanModify = true;
    }
    memcpy(m_Data, buff.GetData(), newSize);
    m_Size = newSize;
}

void HBuffer::CopyString(const char* str){
    m_Size = strlen(str);
    if(m_Size > m_Capacity || !m_CanModify){
        Free();
        
        #ifdef HBUFFER_TRACK_ALLOCATIONS
        CORE_DEBUG("Allocating HBuffer with size {0}", m_Size);
        #endif
        m_Data = new char[m_Size + 1];
        m_Capacity = m_Size;
        m_CanFree = true;
        m_CanModify = true;
    }
    memcpy(m_Data, str, m_Size);
    m_Data[m_Size] = '\0';
}
void HBuffer::CopyString(char* str, size_t size){
    m_Size = size;
    if(m_Size == 0)return;

    if(m_Size > m_Capacity || !m_CanModify){
        //Reallocate
        Free();
        
        #ifdef HBUFFER_TRACK_ALLOCATIONS
        CORE_DEBUG("Allocating HBuffer with size {0}", m_Size);
        #endif
        m_Data = new char[m_Size + 1];
        m_Capacity = m_Size;
        m_CanFree = true;
        m_CanModify = true;
    }
    memcpy(m_Data, str, m_Size);
    m_Data[m_Size] = '\0';
}
void HBuffer::CopyString(const std::string& string){
    m_Size = string.size();
    if(m_Size == 0)return;

    if(m_Size > m_Capacity || !m_CanModify){
        Free();
        
        #ifdef HBUFFER_TRACK_ALLOCATIONS
        CORE_DEBUG("Allocating HBuffer with size {0}", m_Size);
        #endif

        m_Data = new char[m_Size + 1];
        m_Capacity = m_Size;
        m_CanFree = true;
        m_CanModify = true;
    }
    memcpy(m_Data, string.c_str(), m_Size);
    m_Data[m_Size] = '\0';
}

void HBuffer::CopyString(const HBuffer& buff){
    if(buff.m_Data == nullptr)return;
    size_t newSize = buff.GetSize();
    if(newSize > m_Capacity || !m_CanModify){
        Free();
        
        #ifdef HBUFFER_TRACK_ALLOCATIONS
        CORE_DEBUG("Allocating HBuffer with size {0}", m_Size);
        #endif
        m_Capacity = newSize;
        m_Data = new char[m_Capacity + 1];
        m_CanFree =true;
        m_CanModify = true;
    }
    memcpy(m_Data, buff.GetData(), newSize);
    m_Size = newSize;
    m_Data[m_Size] = '\0';
}
#pragma endregion
void HBuffer::Consume(HBuffer& food){
    Append(food);
    food.Free();
}

void HBuffer::Consume(size_t from, HBuffer& food){
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
    }else{
        memcpy(m_Data, m_Data + from, newBuff1Size);
    }
    m_Size = newBuffSize;
    memcpy(m_Data + newBuff1Size, food.m_Data + std::min(from - newBuff1Size, from), newBuff2Size);
    food.Free();
}

void HBuffer::Append(HBuffer& buffer){
    if(!buffer.m_Data || buffer.m_Size < 1)return;

    size_t buffSize = buffer.m_Size;
    size_t newSize = buffSize + m_Size;

    if(newSize > m_Capacity || !m_CanModify){
        //Reallocate
        char* newData = new char[newSize];
        memcpy(newData, m_Data, m_Size);
        //newData[m_Size] = '\0';
        if(m_CanFree)delete m_Data;

        m_Data = newData;
        m_Capacity = newSize;
        m_CanFree = true;
        m_CanModify = true;
    }
    memcpy(m_Data + m_Size, buffer.m_Data, buffSize);
    m_Size = newSize;
}
const char* HBuffer::GetCStr() const noexcept{
    if(m_Data)return m_Data;
    return "";
}

bool HBuffer::StartsWith(size_t at, const char* str) const noexcept{
    size_t i = 0, strLen = strlen(str);

    while(true){
        if(str[i] == '\0')return true;
        if(at >= m_Size)return false;
        if(m_Data[i] != str[i])return false;
        i++;
    }

    return true;
}
bool HBuffer::StartsWith(const char* str) const noexcept{
    size_t i = 0, strLen = strlen(str);

    while(true){
        if(str[i] == '\0')return true;
        if(i == m_Size)return false;
        if(m_Data[i] != str[i])return false;
        i++;
    }

    return true;
}
bool HBuffer::StartsWith(const char* str, size_t len) const noexcept{
    size_t i = 0;

    while(true){
        if(i == len)return true;
        if(i == m_Size)return false;
        if(m_Data[i] != str[i])return false;
        i++;
    }

    return true;
}

int HBuffer::StrXCmp(const char* str) const noexcept{
    size_t i = 0;
    while(true){
        if(str[i] == '\n')return 0;
        if(i == m_Size)return -1;
        if(str[i] != m_Data[i])return 1;
        i++;
    }
    return 0;
}

void HBuffer::Memcpy(void* dest, size_t len) const noexcept{
    memcpy(dest, m_Data, len);
}

void HBuffer::Memcpy(void* dest, size_t at, size_t len)const noexcept{
    if(at >= m_Size)return;
    memcpy(dest, m_Data + at, len);
}

HBuffer& HBuffer::operator=(const HBuffer& right) noexcept{
    Free();
    m_Data = right.m_Data;
    m_Capacity = right.m_Capacity;
    m_Size = right.m_Size;
    m_CanFree = false;
    m_CanModify = right.m_CanModify;
    return *this;
}
HBuffer& HBuffer::operator=(HBuffer&& right) noexcept{
    Free();
    m_Data = right.m_Data;
    m_Capacity = right.m_Capacity;
    m_Size = right.m_Size;
    m_CanFree = right.m_CanFree;
    m_CanModify = right.m_CanModify;
    right.Release();
    return *this;
}
HBuffer& HBuffer::operator=(const char* right) noexcept{
    Free();
    m_Data = const_cast<char*>(right);
    m_Size = strlen(right);
    //can free set to false be free
    m_CanModify = false;
    return *this;
}


constexpr bool HBuffer::operator==(const HBuffer& right)const noexcept{
    if(m_Size != right.m_Size)return false;

    for(size_t i = 0; i < m_Size; i++)
        if(m_Data[i] != right.m_Data[i])return false;
    return true;
}
constexpr bool HBuffer::operator==(const char* str) const noexcept{
    size_t len = 0;
    size_t i = 0;
    while(str[i++] != '\0')len++;

    if(len != m_Size)return false;
    for(i = 0; i < len;i ++)
        if(m_Data[i] != str[i])return false;
    return true;
}
constexpr bool HBuffer::operator!=(const HBuffer& right)const noexcept{
    if(m_Size != right.m_Size)return true;

    for(size_t i = 0; i < m_Size; i++)
        if(m_Data[i] != right.m_Data[i])return true;
    return false;
}
HBuffer& HBuffer::operator+=(size_t offset) noexcept{
    offset = std::min(offset, m_Size);
    if(offset < 1)return *this;

    if(m_CanFree){
        /*
        Explanation
        If this buffer owns that data then we reallocate data and fil it with the last data
        We do this so we dont have unfreed pointers/memory leaks
        */

        //Capacity is now set to size incase we dont want all that extra data
        m_Capacity = m_Size - offset;
        if(m_Capacity < 1){
            Free();
            return *this;
        }

        char* data = new char[m_Capacity];
        memcpy(data, m_Data + offset, m_Capacity);
        if(m_CanFree)delete m_Data;
        m_Size = m_Capacity;
        m_Data = data;
        m_CanFree = true;
        m_CanModify = true;
        return *this;
    }
    m_Data+= offset;
    m_Capacity-=offset;
    m_Size -= offset;
    if(m_Capacity == 0)Release();
    return *this;
}