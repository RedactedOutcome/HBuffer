#pragma once

#include "HBuffer.hpp"

class HBufferJoin{
public:
    HBufferJoin() HBUFF_NOEXCEPT{}
    HBufferJoin(HBuffer& buff1, HBuffer& buff2) HBUFF_NOEXCEPT : m_Buffer1(buff1), m_Buffer2(buff2){}
    HBufferJoin(HBuffer&& buff1) HBUFF_NOEXCEPT : m_Buffer1(buff1){}
    HBufferJoin(HBuffer&& buff1, HBuffer&& buff2) HBUFF_NOEXCEPT : m_Buffer1(buff1), m_Buffer2(buff2){}
    HBufferJoin(const HBufferJoin& join)noexcept{
        m_Buffer1 = join.m_Buffer1;
        m_Buffer2 = join.m_Buffer2;
    }
    HBufferJoin(HBufferJoin&& join)noexcept{
        m_Buffer1 = std::move(join.m_Buffer1);
        m_Buffer2 = std::move(join.m_Buffer2);
    }
    ~HBufferJoin(){}

    /// @brief calls Free() on buffers
    void Free() HBUFF_NOEXCEPT{
        m_Buffer1.Free();
        m_Buffer2.Free();
    }
    
    HBuffer SubStringToDelim(char delimeter)const HBUFF_NOEXCEPT{
        const char* str1 = m_Buffer1.GetData();
        const char* str2 = m_Buffer2.GetData();

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
        //TODO: Do substringing inside
        return SubString(0, i);
    }
    HBuffer SubString(size_t pos, size_t len) const HBUFF_NOEXCEPT{
        const char* str1 = m_Buffer1.GetData();
        const char* str2 = m_Buffer2.GetData();

        size_t len1 = m_Buffer1.GetSize();
        size_t len2 = m_Buffer2.GetSize();
        size_t totalLength = len1 + len2;

        if(pos >= totalLength)
            return HBuffer("", 0, 1, false, false);
        
        size_t bufferLeft = totalLength - pos;
        len = std::min(bufferLeft, len);
        //Or maybe out of range exception
        if(len < 1)return HBuffer(nullptr, 0, false, false);
        char* str = new char[len + 1];

        size_t newLen1 = std::min(len, len1 - pos);
        size_t newLen2 = std::min(bufferLeft - newLen1, len - newLen1);
        size_t totalLen = newLen1 + newLen2;
        
        memcpy(str, str1 + pos, newLen1);
        memcpy(str + newLen1, str2 + std::min(len1 - pos, pos), newLen2);
        memset(str + len, '\0', 1);

        return HBuffer(str, len, len + 1, true, true);
    }
    
    HBuffer SubBuffer(size_t pos, size_t len) const HBUFF_NOEXCEPT{
        const char* str1 = m_Buffer1.GetData();
        const char* str2 = m_Buffer2.GetData();

        size_t len1 = m_Buffer1.GetSize();
        size_t len2 = m_Buffer2.GetSize();
        size_t totalLength = len1 + len2;

        if(pos >= totalLength)
            return HBuffer(nullptr, 0, false, false);

        len = std::min(len, totalLength - pos);
        //Or maybe out of range exception
        if(len < 1)return HBuffer(nullptr, 0, false, false);
        char* str = new char[len];
        
        size_t buff1FillSize = 0;
        if(pos < len1){
            //Use first buffer
            buff1FillSize = std::min(len, len1 - pos);
            memcpy(str, str1 + pos, buff1FillSize);
        }
        memcpy(str + buff1FillSize, str2 + (std::max(pos, len1) - len1), std::min(len - buff1FillSize, len2 - pos - buff1FillSize));
        return HBuffer(str, len, true, true);
    }
public:
    bool StartsWith(size_t at, const char* str) const HBUFF_NOEXCEPT{
        size_t len = strlen(str);

        const char* str1 = m_Buffer1.GetData();
        const char* str2 = m_Buffer2.GetData();
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
    bool StartsWith(size_t at, const char* str, size_t len) const HBUFF_NOEXCEPT{
        char* str1 = m_Buffer1.GetData();
        char* str2 = m_Buffer2.GetData();
        size_t len1 = m_Buffer1.GetSize();
        size_t len2 = m_Buffer2.GetSize();
        size_t strPos = 0;
        while(true){
            if(strPos == len)return true;
            if(at >= len1)break;
            if(str[strPos] != str1[at])return false;
            at++;
            strPos++;
        }

        at-=len1;
        while(true){
            if(strPos == len)return true;
            if(at >= len2)return false;
            if(str[strPos] != str2[at])return false;
            at++;
            strPos++;
        }
        return true;
    }
    bool StartsWith(const char* str) const HBUFF_NOEXCEPT{
        size_t len = strlen(str);

        char* str1 = m_Buffer1.GetData();
        char* str2 = m_Buffer2.GetData();
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
    bool StartsWith(const char* str, size_t len) const HBUFF_NOEXCEPT{
        const char* str1 = m_Buffer1.GetData();
        const char* str2 = m_Buffer2.GetData();
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
    
    ///@brief opies from buffers into dest.
    ///@return returns 0 if success
    int Memcpy(void* src, size_t len) const HBUFF_NOEXCEPT{
        char* str1 = m_Buffer1.GetData();
        char* str2 = m_Buffer2.GetData();
        size_t len1 = m_Buffer1.GetSize();
        size_t len2 = m_Buffer2.GetSize();
        size_t i = 0, strPos = 0;
        while(true){
            if(strPos == len)return 0;
            if(i == len1)break;
            str1[i] = static_cast<const char*>(src)[i];
            i++;
            strPos++;
        }

        i=0;
        while(true){
            if(strPos == len)break;
            if(i == len2)return 1;
            str1[i] = static_cast<const char*>(src)[i];
            i++;
            strPos++;
        }
        return 0;
    }
    int Memcpy(void* src, size_t offset, size_t len) const HBUFF_NOEXCEPT{
        char* str1 = m_Buffer1.GetData();
        char* str2 = m_Buffer2.GetData();
        size_t len1 = m_Buffer1.GetSize();
        size_t len2 = m_Buffer2.GetSize();
        size_t i = offset, strPos = 0;
        while(true){
            if(strPos == len)return 0;
            if(i >= len1)break;
            str1[i] = static_cast<const char*>(src)[i];
            i++;
            strPos++;
        }

        i-= len1;
        while(true){
            if(strPos == len)break;
            if(i >= len2)return 1;
            str1[i] = static_cast<const char*>(src)[i];
            i++;
            strPos++;
        }
        return 0;
    }
     ///@brief opies from buffers into dest.
    ///@return returns 0 if success
    int MemcpyTo(void* dest, size_t len) const HBUFF_NOEXCEPT{
        const char* str1 = m_Buffer1.GetData();
        const char* str2 = m_Buffer2.GetData();
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
    int MemcpyTo(void* dest, size_t offset, size_t len) const HBUFF_NOEXCEPT{
        const char* str1 = m_Buffer1.GetData();
        const char* str2 = m_Buffer2.GetData();
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
    /// @brief Compares param str with buffers combined as one to act as string
    int StrCmp(const char* str) const HBUFF_NOEXCEPT{
        size_t len = strlen(str);

        const char* str1 = m_Buffer1.GetData();
        const char* str2 = m_Buffer2.GetData();
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
    
    /// @return returns 0 if success return -1 if buffer is out of data and 1 if data doesnt match
    int StrXCmp(const char* str) const HBUFF_NOEXCEPT{
        size_t len = strlen(str);

        const char* str1 = m_Buffer1.GetData();
        const char* str2 = m_Buffer2.GetData();
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
    int StrXCmp(size_t at, const char* str) const HBUFF_NOEXCEPT{
        size_t len = strlen(str);

        const char* str1 = m_Buffer1.GetData();
        const char* str2 = m_Buffer2.GetData();
        size_t len1 = m_Buffer1.GetSize();
        size_t len2 = m_Buffer2.GetSize();
        size_t i = at, strPos = 0;

        if(i < len1){
            ///Use first buffer
            while(true){
                if(strPos == len)return 0;
                if(i >= len1){
                    //i = 0;
                    break;
                }
                if(str[strPos] != str1[i])return 1;
                i++;
                strPos++;
            }
        }
        i -= len1;
        while(true){
            if(strPos == len)return 0;
            if(i >= len2)return -1;
            if(str[strPos] != str2[i])return 1;
            i++;
            strPos++;
        }
        return 0;
    }
public:
    HBuffer& operator=(const HBufferJoin& right)noexcept{
        m_Buffer1 = right.m_Buffer1;
        m_Buffer2 = right.m_Buffer2;
        return *this;
    }
    HBuffer& operator=(HBufferJoin&& right)noexcept{
        m_Buffer1 = std::move(right.m_Buffer1);
        m_Buffer2 = std::move(right.m_Buffer2);
        return *this;
    }
public:
    /// @brief attempts to get the byte from the join at index i. Does not do safety checks so there is segfault potential.
    /// @return returns the character at i.
    char At(size_t i) const HBUFF_NOEXCEPT{
        if(i <= m_Buffer1.m_Size)return m_Buffer1.m_Data[i];
        return m_Buffer2.m_Data[i - m_Buffer1.m_Size];
    }
    /// @brief same as function At(size_t i) except we have safety checks where if out of range we return '\0'
    /// @return returns the character at i. returns '\0' if out of range of buffers
    char Get(size_t i) const HBUFF_NOEXCEPT{
        if(i <= m_Buffer1.m_Size)return m_Buffer1.m_Data[i];
        if(i >= m_Buffer1.m_Size + m_Buffer2.m_Size)return '\0';
        return m_Buffer2.m_Data[i - m_Buffer1.m_Size];
    }
public:
    HBUFF_CONSTEXPR HBuffer& GetBuffer1() const HBUFF_NOEXCEPT{return (HBuffer&)m_Buffer1;}
    HBUFF_CONSTEXPR HBuffer& GetBuffer2() const HBUFF_NOEXCEPT{return (HBuffer&)m_Buffer2;}
    HBUFF_CONSTEXPR size_t GetSize() const HBUFF_NOEXCEPT{return m_Buffer1.GetSize() + m_Buffer2.GetSize();}
private:
    HBuffer m_Buffer1;
    HBuffer m_Buffer2;
};