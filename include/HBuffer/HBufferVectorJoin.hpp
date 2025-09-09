#pragma once

#include "Core.h"
#include "HBuffer.hpp"

/// TODO: finish because this is not ready to publish

/// @brief A vector of type std::vector<HBuffer>
template <typename Allocator=std::allocator<HBuffer>>
class HBufferVectorJoin{
public:
    HBufferVectorJoin() HBUFF_NOEXCEPT{}
    ~HBufferVectorJoin() HBUFF_NOEXCEPT{}

    bool StartsWith(const char* str, size_t len) const HBUFF_NOEXCEPT{}
    bool StartsWith(size_t at, const char* str) const HBUFF_NOEXCEPT{}
    bool StartsWith(size_t at, const char* str, size_t len) const HBUFF_NOEXCEPT{}

    /// @brief returns the character at param at if inside the buffer else \0
    char Get(size_t at)const HBUFF_NOEXCEPT{
        if(m_Indices.size() < 1)return '\0';
        size_t index = -1;
        size_t totalBefore=0;
        for(index = 0; index < m_Indices.size(); index++){
            size_t total = m_Indices[index];
            if(at >= total){
                totalBefore = total;
                break;
            }
        }
        if(index == -1)return '\0';
        HBuffer& vec = m_Vectors[index];
        size_t relativeAt = at - totalBefore;
        /// TODO: think and maybe change to vec.At()
        return vec.Get(relativeAt);
    }

    /// @brief returns the character reference at the character at.
    /// @brief will segfault if at is outside of the buffers ranges. No Safety Checks.
    char& At(size_t at)const HBUFF_NOEXCEPT{
        size_t index = -1;
        size_t totalBefore=0;
        for(index = 0; index < m_Indices.size(); index++){
            size_t total = m_Indices[index];
            if(at >= total){
                totalBefore = total;
                break;
            }
        }
        HBuffer& vec = m_Vectors[index];
        size_t relativeAt = at - totalBefore;
        /// TODO: think and maybe change to vec.At()
        return vec.At(relativeAt);
    }

    /// @brief creates a single null terminated ascii string starting at at and is size len
    /// @brief at the position to start copying from
    /// @param len the length of the ascii bytes in the new string. will be maxed out at the buffers
    HBuffer SubString(size_t at, size_t len=-1)HBUFF_NOEXCEPT{
        HBuffer string;
        size_t totalLen = 0;
        size_t startIndex;
        size_t totalBefore = -1;
        size_t indicesSize = m_Indices.size();
        for(startIndex = 0; startIndex < indicesSize; startIndex++){
            size_t total = m_Indices[startIndex];
            if(at < total + m_Vectors[startIndex].GetSize()){
                /// Getting max possible string length
                totalBefore = total;
                size_t maxLength = m_Indices[indicesSize - 1] + m_Vectors[indicesSize - 1].GetSize();
                totalLen = std::min(maxLength - at, len);
                string.ReserveString(totalLen);
                break;
            }
        }

        if(totalBefore == -1){
            string.ReserveString(0);
            return std::move(string);
        }

        size_t accumulatedLength = 0;
        for(size_t index = startIndex; index < indicesSize; index++){
            HBuffer& referenceBuffer = m_Vectors[index];
            size_t bufferAt = index == startIndex ? at - totalBefore : 0;
            size_t bufferLen = std::min(referenceBuffer.GetSize() - bufferAt, totalLen - accumulatedLength);
            HBuffer data = referenceBuffer.SubBuffer(bufferAt, bufferLen);
            string.Append(data);
            accumulatedLength += bufferLen;
            if(accumulatedLength >= totalLen)break;
        }
        return string;
    }

    /// TODO: Sub join. Kinda like subpointer on Hbuffer except we dont want to allocate any data except for the m_Vectors and m_Indices
    HBufferVectorJoin SubJoin()const noexcept{
        HBufferVectorJoin join;

        return join;
    }

    template <typename... Args>
    void EmplaceBack(Args&&... args){
        size_t lastIndice = 0;
        size_t vecSize = 0;
        size_t indicesSize = m_Indices.size();
        if(indicesSize > 0){
            size_t indice = indicesSize - 1;
            lastIndice = m_Indices[indice];
            vecSize = m_Vectors[indice].GetSize();
        }
        m_Vectors.emplace_back(std::forward<Args>(args)...);
        m_Indices.emplace_back(lastIndice + vecSize);
    }

    void EmplaceBack(const HBuffer& buffer){
        size_t lastIndice = 0;
        size_t vecSize = 0;
        size_t indicesSize = m_Indices.size();
        if(indicesSize > 0){
            size_t indice = indicesSize - 1;
            lastIndice = m_Indices[indice];
            vecSize = m_Vectors[indice].GetSize();
        }
        m_Vectors.emplace_back(buffer);
        m_Indices.emplace_back(lastIndice + vecSize);
    }
    
    void EmplaceBack(HBuffer&& buffer){
        size_t lastIndice = 0;
        size_t vecSize = 0;
        size_t indicesSize = m_Indices.size();
        if(indicesSize > 0){
            size_t indice = indicesSize - 1;
            lastIndice = m_Indices[indice];
            vecSize = m_Vectors[indice].GetSize();
        }
        m_Vectors.emplace_back(std::move(buffer));
        m_Indices.emplace_back(lastIndice + vecSize);
    }

    void Erase(size_t at)noexcept{
        size_t vectorSize = m_Vectors.size();
        if(vectorSize < 0)return;
        if(at >= vectorSize)return;
        for(size_t i = at + 1; i < m_Vectors.size();i++){
            m_Indices[i] = m_Indices[i - 1];
        }
        m_Vectors.erase(m_Vectors.begin() + at);
        m_Indices.erase(m_Indices.begin() + at);
    }
public:
    std::vector<HBuffer, Allocator>& GetVectors()const noexcept{return (std::vector<HBuffer, Allocator>&)m_Vectors;}
    std::vector<size_t>& GetIndices()const noexcept{return (std::vector<size_t>&)m_Indices;}
private:
    std::vector<HBuffer, Allocator> m_Vectors;
    /// @brief a vector where each node contains the sizes of all vectors before it
    /// Example: vec size 15, vec size 20, vec size 2
    /// Values: 0, 15, 35
    std::vector<size_t> m_Indices;
};