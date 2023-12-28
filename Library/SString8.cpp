#include "SString8.h"

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

SString8::SString8(size_type count, CharT ch)
    : SString8(std::string(count, ch)) // creates a temporary.  @TODO To be fixed once we have preallocation of size
{
}

SString8::SString8(const SString8& other, size_type pos, size_type count)
    : m_Storage(other.data() + pos + SString8::check_out_of_range(other, pos), SString8::check_count(other, pos, count))
{
    /*
    basic_string( const basic_string& other,
                  size_type pos, size_type count,
                  const Allocator& alloc = Allocator() );
    Constructs the string with a substring [pos, pos + count) of other.
    If count == npos, if count is not specified, or if the requested substring lasts past the end of the string, the resulting substring is [pos, other.size()).
    */
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
