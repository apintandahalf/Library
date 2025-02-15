#include "SString8.h"

#include <algorithm>

SString8Detail::SString8Data::SString8Data(size_t count, char ch)
{
    if (count <= 7)
    {
        for (size_t i = 0; i < count; ++i)
            m_Storage.m_Buffer.m_Buffer[i] = ch;
        m_Storage.m_Buffer.m_Buffer[7] = static_cast<char>(7 - count);
    }
    else
    {
        auto cap = calcCapacity(count);

        const auto offset = (cap <= max_size_small) ? 0U : (cap <= fifeteen_bites_set) ? 8U : 16U;
        auto ptr = new char[cap + offset + 1];
        auto p = ptr + offset;
        std::for_each(p, p + count, [ch](char& c) { c = ch; });
        p[count] = '\0';
        m_Storage.m_pLargeStr = reinterpret_cast<uintptr_t>(ptr);
        if (cap <= max_size_small)
            setSmall(count, cap);
        else if (cap <= fifeteen_bites_set)
            setMedium(count, cap);
        else
            setLarge(count, cap);
    }
}

SString8::operator std::string_view() const
{
    return { data(), length() };
}

SString8::SString8(std::string_view str)
    : m_Storage(str)
{
}

SString8::SString8(const std::string& str)
    : m_Storage(str.data(), str.size())
{
}

SString8::SString8(size_t count, char ch)
    : m_Storage(count, ch)
{
}

SString8::SString8(const SString8& other, size_type pos, size_type count)
    : m_Storage(other.data() + pos + SString8::check_out_of_range(other, pos), SString8::check_count(other, pos, count))
{
}

SString8::SString8(const std::string& other, size_type pos, size_type count)
    : m_Storage(other.data() + pos + SString8::check_out_of_range(other, pos), SString8::check_count(other, pos, count))
{
}

SString8::SString8(const SString8& other, size_type pos)
    : m_Storage(other.data() + pos + SString8::check_out_of_range(other, pos), other.length()-pos)
{
}

SString8::SString8(const std::string& other, size_type pos)
    : m_Storage(other.data() + pos + SString8::check_out_of_range(other, pos), other.length() - pos)
{
}

SString8::SString8(const CharT* s, size_type count)
    : m_Storage(s, count)
{
}

SString8::SString8(const CharT* s)
    : m_Storage(s, strlen(s))
{
}

SString8::SString8(std::initializer_list<CharT> ilist)
    : SString8(ilist.begin(), ilist.size())
{
}

SString8& SString8::operator=(const CharT* s)
{
    // TODO - a bit inefficient as we have worked out the storage type to get the ptr and capacity, and then we do the same again in setSize
    const auto newlen = strlen(s);
    const auto [oldptr, oldcap] = m_Storage.getDataAndCap();
    if (newlen <= oldcap)
    {
        strcpy(oldptr, s);
        m_Storage.setSize(newlen);
    }
    else
    {
        m_Storage.allocateWithDeallocate(s, newlen);
    }
    return *this;
}

SString8& SString8::operator=(CharT ch)
{
    // TODO - a bit inefficient as we have worked out the storage type to get the ptr and capacity, and then we do the same again in setSize
    const auto [oldptr, oldcap] = m_Storage.getDataAndCap();
    ASSERT(oldcap > 2);
    oldptr[0] = ch;
    oldptr[1] = '\0';
    m_Storage.setSize(1);
    return *this;
}

SString8& SString8::operator=(std::initializer_list<CharT> ilist)
{
    // TODO - a bit inefficient as we have worked out the storage type to get the ptr and capacity, and then we do the same again in setSize
    const auto newlen = ilist.size();
    const auto [oldptr, oldcap] = m_Storage.getDataAndCap();
    if (newlen <= oldcap)
    {
        size_t i = 0;
        for (const auto ch : ilist)
        {
            oldptr[i] = ch;
            ++i;
        }
        ASSERT(i == newlen);
        oldptr[newlen] = '\0';
        m_Storage.setSize(newlen);
    }
    else
    {
        m_Storage.allocateWithDeallocate(ilist.begin(), newlen);
    }
    return *this;
}

const SString8::CharT* SString8::data() const noexcept
{
    return m_Storage.data();
}

SString8::CharT* SString8::data() noexcept
{
    return m_Storage.data();
}

SString8::size_type SString8::size() const noexcept
{
    return m_Storage.size();
}

SString8::size_type SString8::length() const noexcept
{
    return size();
}

SString8::size_type SString8::capacity() const noexcept
{
    return m_Storage.capacity();
}

void SString8::reserve(SString8::size_type new_cap)
{
    return m_Storage.reserve(new_cap);
}
