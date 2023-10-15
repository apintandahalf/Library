#include "SString8.h"

#include <cstring>
#include <string>

//CONSTEXPR SString8::SString8() noexcept
//{
//    m_Storage.m_pLargeStr = 0;
//}

std::string_view SString8::asStringView() const
{
    return { data(), length() };
}

CONSTEXPR SString8::SString8(std::string_view str)
    : m_Storage(str)
{
}

CONSTEXPR SString8::SString8(const std::string& str)
    : m_Storage(str.data(), str.size())
{
}

CONSTEXPR SString8::SString8(size_type count, CharT ch)
    : SString8(std::string(count, ch)) // creates a temporary.  @TODO To be fixed once we have preallocation of size
{
}

CONSTEXPR SString8::SString8(const SString8& other, size_type pos, size_type count)
    : m_Storage(other.data()+pos, count)
{
}

CONSTEXPR SString8::SString8(const std::string& other, size_type pos, size_type count)
    : m_Storage(other.data() + pos, count)
{
}

CONSTEXPR SString8::SString8(const SString8& other, size_type pos)
    : m_Storage(other.data() + pos, other.length()-pos)
{
}

CONSTEXPR SString8::SString8(const std::string& other, size_type pos)
    : m_Storage(other.data() + pos, other.length() - pos)
{
}

CONSTEXPR SString8::SString8(const CharT* s, size_type count)
    : m_Storage(s, count)
{
}

CONSTEXPR SString8::SString8(const CharT* s)
    : m_Storage(s, strlen(s))
{
}

CONSTEXPR SString8::SString8(std::initializer_list<CharT> ilist)
    : SString8(ilist.begin(), ilist.size())
{
}

CONSTEXPR const SString8::CharT* SString8::data() const noexcept
{
    return m_Storage.data();
}

CONSTEXPR SString8::CharT* SString8::data() noexcept
{
    return m_Storage.data();
}

CONSTEXPR SString8::size_type SString8::size() const noexcept
{
    return m_Storage.size();
}

CONSTEXPR SString8::size_type SString8::length() const noexcept
{
    return size();
}

void SString8::reserve(size_t /*new_cap*/)
{
    // @TODO
}
