#include "SString8.h"

// implementation - move to cpp?
#include <cstring>
#include <string>

//CONSTEXPR SString8::SString8() noexcept
//{
//    m_Storage.m_Large.m_pStr = 0;
//}

std::string_view SString8::asStringView() const
{
    return { data(), length() };
}

CONSTEXPR SString8::SString8(std::string_view str)
{
    const auto len = str.length();
    if (len <= 7)
    {
        m_Storage.m_Storage.m_Large.m_pStr = 0;
        memcpy(m_Storage.m_Storage.m_Buffer.m_Buffer, str.data(), len);
        // size and capacity ?
    }
    else
    {
        auto ptr = new char[len + 1];
        strncpy_s(ptr, len + 1, str.data(), len);
        ptr[len] = '\0';
        m_Storage.m_Storage.m_Large.m_pStr = reinterpret_cast<uintptr_t>(ptr);
        setLarge();
        // size and capacity ?
    }
}

CONSTEXPR SString8::SString8(const std::string& str)
    : SString8(str.operator std::basic_string_view<char, std::char_traits<char>>())
{
}

CONSTEXPR SString8::SString8(size_type count, CharT ch)
{
    if (count <= 7)
    {
        m_Storage.m_Storage.m_Large.m_pStr = 0;
        for (size_type i = 0; i < count; ++i)
            m_Storage.m_Storage.m_Buffer.m_Buffer[i] = ch;
        // size and capacity ?
    }
    else
    {
        auto ptr = new char[count + 1];
        for (size_type i = 0; i < count; ++i)
            ptr[i] = ch;
        ptr[count] = '\0';
        m_Storage.m_Storage.m_Large.m_pStr = reinterpret_cast<uintptr_t>(ptr);
        setLarge();
        // size and capacity ?
    }
}

CONSTEXPR SString8::SString8(const SString8& other, size_type pos, size_type count)
{
    // Constructs the string with a substring [pos, pos + count) of other. If count == npos, if count is not specified, or if the requested substring lasts past the end of the string, the resulting substring is [pos, other.size())
    std::string tempstdstr(std::string(other.asStringView()), pos, count);
    auto tempstr = SString8(tempstdstr);
    swap(tempstr);
}

CONSTEXPR SString8::SString8(const std::string& other, size_type pos, size_type count)
{
    // Constructs the string with a substring [pos, pos + count) of other. If count == npos, if count is not specified, or if the requested substring lasts past the end of the string, the resulting substring is [pos, other.size())
    std::string tempstdstr(other, pos, count);
    auto tempstr = SString8(tempstdstr);
    swap(tempstr);
}

CONSTEXPR SString8::SString8(const SString8& other, size_type pos)
{
    // Constructs the string with a substring [pos, pos + count) of other. If count == npos, if count is not specified, or if the requested substring lasts past the end of the string, the resulting substring is [pos, other.size())
    std::string tempstdstr(std::string(other.asStringView()), pos);
    auto tempstr = SString8(tempstdstr);
    swap(tempstr);
}

CONSTEXPR SString8::SString8(const std::string& other, size_type pos)
{
    // Constructs the string with a substring [pos, pos + count) of other. If count == npos, if count is not specified, or if the requested substring lasts past the end of the string, the resulting substring is [pos, other.size())
    std::string tempstdstr(other, pos);
    auto tempstr = SString8(tempstdstr);
    swap(tempstr);
}

CONSTEXPR SString8::SString8(const CharT* s, size_type count)
{
    std::string tempstdstr(s, count);
    auto tempstr = SString8(tempstdstr);
    swap(tempstr);
}

CONSTEXPR SString8::SString8(const CharT* s)
{
    std::string tempstdstr(s);
    auto tempstr = SString8(tempstdstr);
    swap(tempstr);
}

CONSTEXPR SString8::SString8(std::initializer_list<CharT> ilist)
{
    std::string tempstdstr(ilist);
    auto tempstr = SString8(tempstdstr);
    swap(tempstr);
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
