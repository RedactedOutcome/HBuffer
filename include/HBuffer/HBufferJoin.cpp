#include "pch.h"
#include "HBufferJoin.h"
#include "Core/Logger.h"

HBufferJoin::HBufferJoin(){}
HBufferJoin::HBufferJoin(HBuffer& buff1, HBuffer& buff2):
    m_Buffer1(buff1),
    m_Buffer2(buff2){}
HBufferJoin::HBufferJoin(HBuffer&& buff1):
    m_Buffer1(buff1){}
HBufferJoin::~HBufferJoin(){}

void HBufferJoin::Free(){
    m_Buffer1.Free();
    m_Buffer2.Free();
}

HBuffer HBufferJoin::SubStringToDelim(char delimeter)const{
    const char* str1 = m_Buffer1.GetCStr();
    const char* str2 = m_Buffer2.GetCStr();

    size_t len1 = m_Buffer1.GetSize();
    size_t len2 = m_Buffer2.GetSize();
    size_t totalLength = len1 + len2;

    size_t i = 0;
    while(true){
        if(i == len1)break;
        if(str1[i] == delimeter)return SubString(0, i);
        i++;
    }

    i = 0;
    while(true){
        if(i == len2)break;
        if(str2[i] == delimeter)break;
        i++;
    }

    return SubString(0, i);
}
HBuffer HBufferJoin::SubString(size_t pos, size_t len) const{
    const char* str1 = m_Buffer1.GetCStr();
    const char* str2 = m_Buffer2.GetCStr();

    size_t len1 = m_Buffer1.GetSize();
    size_t len2 = m_Buffer2.GetSize();
    size_t totalLength = len1 + len2;

    if(pos >= totalLength)
        return HBuffer(nullptr, 0, false, false);

    len = std::min(len, totalLength - pos);
    //Or maybe out of range exception
    if(len < 1)return HBuffer(nullptr, 0, false, false);

    #ifdef HBUFFER_TRACK_ALLOCATIONS
    CORE_DEBUG("HBufferJoin Allocating {0} bytes", len + 1);
    #endif
    char* str = new char[len + 1];

    size_t buff1FillSize = 0;
    if(pos < len1){
        //Use first buffer
        buff1FillSize = std::min(len, len1 - pos);
        memcpy(str, str1 + pos, buff1FillSize);
    }
    memcpy(str + buff1FillSize, str2 + (std::max(pos, len1) - len1), std::min(len - buff1FillSize, len2 - pos - buff1FillSize));
    memset(str + len, '\0', 1);
    return HBuffer(str, len, true);
}
#pragma region MEMCPY
int HBufferJoin::Memcpy(void* dest, size_t len) const{
    const char* str1 = m_Buffer1.GetCStr();
    const char* str2 = m_Buffer2.GetCStr();
    size_t len1 = m_Buffer1.GetSize();
    size_t len2 = m_Buffer2.GetSize();
    size_t i = 0, strPos = 0;
    while(true){
        if(strPos == len)return 0;
        if(i == len1)break;
        static_cast<char*>(dest)[i] = str1[i];
        i++;
        strPos++;
    }

    i=0;
    while(true){
        if(strPos == len)break;
        if(i == len2)return 1;
        static_cast<char*>(dest)[i] = str2[i];
        i++;
        strPos++;
    }
    return 0;
}
int HBufferJoin::Memcpy(void* dest, size_t offset, size_t len) const{
    const char* str1 = m_Buffer1.GetCStr();
    const char* str2 = m_Buffer2.GetCStr();
    size_t len1 = m_Buffer1.GetSize();
    size_t len2 = m_Buffer2.GetSize();
    size_t i = offset, strPos = 0;
    while(true){
        if(strPos == len)return 0;
        if(i >= len1)break;
        static_cast<char*>(dest)[strPos] = str1[i];
        i++;
        strPos++;
    }

    i-= len1;
    while(true){
        if(strPos == len)break;
        if(i >= len2)return 1;
        static_cast<char*>(dest)[strPos] = str2[i];
        i++;
        strPos++;
    }
    return 0;
}

#pragma endregion
bool HBufferJoin::StartsWith(size_t at, const char* str) const noexcept{
    size_t len = strlen(str);

    const char* str1 = m_Buffer1.GetCStr();
    const char* str2 = m_Buffer2.GetCStr();
    size_t len1 = m_Buffer1.GetSize();
    size_t len2 = m_Buffer2.GetSize();
    size_t i = at, strPos = 0;
    while(true){
        if(strPos == len)return true;
        if(i >= len1)break;
        if(str[strPos] != str1[i])return false;
        i++;
        strPos++;
    }

    i-=len1;
    while(true){
        if(strPos == len)return true;
        if(i >= len2)return false;
        if(str[strPos] != str2[i])return false;
        i++;
        strPos++;
    }
    return true;
}
bool HBufferJoin::StartsWith(size_t at, const char* str, size_t len) const noexcept{
    const char* str1 = m_Buffer1.GetCStr();
    const char* str2 = m_Buffer2.GetCStr();
    size_t len1 = m_Buffer1.GetSize();
    size_t len2 = m_Buffer2.GetSize();
    size_t i = at, strPos = 0;
    while(true){
        if(strPos == len)return true;
        if(i >= len1)break;
        if(str[strPos] != str1[i])return false;
        i++;
        strPos++;
    }

    i-=len1;
    while(true){
        if(strPos == len)return true;
        if(i >= len2)return false;
        if(str[strPos] != str2[i])return false;
        i++;
        strPos++;
    }
    return true;
}
bool HBufferJoin::StartsWith(const char* str) const noexcept{
    size_t len = strlen(str);
    
    const char* str1 = m_Buffer1.GetCStr();
    const char* str2 = m_Buffer2.GetCStr();
    size_t len1 = m_Buffer1.GetSize();
    size_t len2 = m_Buffer2.GetSize();
    size_t i = 0, strPos = 0;
    while(true){
        if(strPos == len)return true;
        if(i == len1)break;
        if(str[strPos] != str1[i])return false;
        i++;
        strPos++;
    }

    i=0;
    while(true){
        if(strPos == len)return true;
        if(i == len2)return false;
        if(str[strPos] != str2[i])return false;
        i++;
        strPos++;
    }
    return true;
}

bool HBufferJoin::StartsWith(const char* str, size_t len) const noexcept{
    const char* str1 = m_Buffer1.GetCStr();
    const char* str2 = m_Buffer2.GetCStr();
    size_t len1 = m_Buffer1.GetSize();
    size_t len2 = m_Buffer2.GetSize();
    size_t i = 0, strPos = 0;
    while(true){
        if(strPos == len)return true;
        if(i == len1)break;
        if(str[strPos] != str1[i])return false;
        i++;
        strPos++;
    }

    i=0;
    while(true){
        if(strPos == len)return true;
        if(i == len2)return false;
        if(str[strPos] != str2[i])return false;
        i++;
        strPos++;
    }

    return true;
}
int HBufferJoin::StrCmp(const char* str) const noexcept{
    size_t len = strlen(str);

    const char* str1 = m_Buffer1.GetCStr();
    const char* str2 = m_Buffer2.GetCStr();
    size_t len1 = m_Buffer1.GetSize();
    size_t len2 = m_Buffer2.GetSize();
    size_t i = 0, strPos = 0;

    while(true){
        if(strPos == len)return 0;
        if(i == len1)break;
        if(str[strPos] != str1[i]){
            return str1[i] - str[strPos];
        }
        i++;
        strPos++;
    }

    i=0;
    while(true){
        if(strPos == len)return 0;
        if(i == len2 || str[strPos] != str2[i]){
            return str2[i] - str[strPos];
        }
        i++;
        strPos++;
    }
    return 0;
}
int HBufferJoin::StrXCmp(const char* str) const noexcept{
    size_t len = strlen(str);

    const char* str1 = m_Buffer1.GetCStr();
    const char* str2 = m_Buffer2.GetCStr();
    size_t len1 = m_Buffer1.GetSize();
    size_t len2 = m_Buffer2.GetSize();
    size_t i = 0, strPos = 0;

    while(true){
        if(strPos == len)return 0;
        if(i == len1)break;
        if(str[strPos] != str1[i])return 1;
        i++;
        strPos++;
    }

    i=0;
    while(true){
        if(strPos == len)return 0;
        if(i == len2)return -1;
        if(str[strPos] != str2[i])return 1;
        i++;
        strPos++;
    }
    return 0;
}
int HBufferJoin::StrXCmp(size_t at, const char* str) const noexcept{
    size_t len = strlen(str);

    const char* str1 = m_Buffer1.GetCStr();
    const char* str2 = m_Buffer2.GetCStr();
    size_t len1 = m_Buffer1.GetSize();
    size_t len2 = m_Buffer2.GetSize();
    size_t i = at, strPos = 0;
    
    while(true){
        if(strPos == len)return 0;
        if(i >= len1)break;
        if(str[strPos] != str1[i])return 1;
        i++;
        strPos++;
    }

    i=0;
    while(true){
        if(strPos == len)return 0;
        if(i >= len2)return -1;
        if(str[strPos] != str2[i])return 1;
        i++;
        strPos++;
    }
    return 0;
}
HBufferJoin& HBufferJoin::operator+=(size_t i){
    size_t size = m_Buffer1.GetSize();
    m_Buffer1+=i;
    if(m_Buffer1.GetSize() < 1){
        //Swap
        m_Buffer1.Swap(m_Buffer2);
        m_Buffer1+=(std::max(i, size) - size);
    }
    else{
        m_Buffer2+=(std::max(i, size) - size);
    }
    return *this;
}
char HBufferJoin::At(size_t i) const noexcept{
    size_t size1 = m_Buffer1.GetSize();
    size_t total = size1 + m_Buffer2.GetSize();
    if(i >= total)return '\0';
    if(i < size1)return m_Buffer1.m_Data[i];
    return m_Buffer2.m_Data[i - size1];
}