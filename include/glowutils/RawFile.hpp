#pragma once

#include <cassert>
#include <algorithm>
#include <fstream>

#include <glow/logging.h>

#include <glowutils/RawFile.h>

namespace glow 
{

template<typename T>
RawFile<T>::RawFile(const std::string & filePath)
:   m_filePath(filePath)
,   m_valid(false)
{
    read();
}

template<typename T>
RawFile<T>::~RawFile()
{
}

template<typename T>
bool RawFile<T>::valid() const
{
    return m_valid;
}

template<typename T>
const T * RawFile<T>::data() const
{
    return &m_data.data()[0];
}

template<typename T>
const size_t RawFile<T>::size() const
{
    return m_data.size();
}

template<typename T>
bool RawFile<T>::read()
{
    std::ifstream ifs(m_filePath, std::ios::in | std::ios::binary | std::ios::ate);

    if (!ifs)
    {
        warning() << "Reading from file \"" << m_filePath << "\" failed.";
        return false;
    }

    const size_t size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    m_data.resize(size / sizeof(T));

    ifs.read(reinterpret_cast<char*>(&m_data[0]), size);
    ifs.close();

    m_valid = true;
    return true;
}

} // namespace glow
