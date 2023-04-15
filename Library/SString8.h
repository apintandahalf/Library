#pragma once

#include <cstddef>
#include <string>
#include <string_view>

#define CONSTEXPR // I'm not doing constexpr or allocators for the time being
class SString8
{
public:
    using size_type = size_t;
    using CharT = char;

    // constructors

    SString8(std::nullptr_t) = delete;

    CONSTEXPR SString8() noexcept; // tested by String8TestDefaultConstructor

    SString8(std::string_view str); // tested by String8TestConstructorStringView
    SString8(const std::string& str); // tested by String8TestConstructorString
    std::string_view asStringView() const; // tested by String8TestAsStringView

    CONSTEXPR SString8(size_type count, CharT ch); // tested by String8TestConstructorCountChar

    CONSTEXPR SString8(const SString8& other, size_type pos); // tested by String8TestConstructorOtherPos

    CONSTEXPR SString8(const SString8& other, size_type pos, size_type count); // tested by String8TestConstructorOtherPosCount

    CONSTEXPR SString8(const CharT* s, size_type count); // tested by String8TestConstructorCharStarCount

    CONSTEXPR SString8(const CharT* s); // tested by String8TestConstructorCharStar

    template<class InputIt> SString8(InputIt first, InputIt last); // tested by String8TestConstructorInputItFirstLast

    CONSTEXPR SString8(std::initializer_list<CharT> ilist); // tested by String8TestConstructorInitialiserList

    template<class StringViewLike>
        requires std::is_convertible_v<const StringViewLike&, std::basic_string_view<CharT>>
        && !std::is_convertible_v<const StringViewLike&, const CharT*>
    CONSTEXPR explicit SString8(const StringViewLike& t) //  tested by String8TestConstructorStringViewLike
    {
        std::string tempstdstr(t);
        auto tempstr = SString8(tempstdstr);
        std::swap(*this, tempstr);
    }

    template<class StringViewLike>
        requires std::is_convertible_v<const StringViewLike&, std::basic_string_view<CharT>>
        && !std::is_convertible_v<const StringViewLike&, const CharT*>
    CONSTEXPR explicit SString8(const StringViewLike& t, size_type pos, size_type n) // tested by String8TestConstructorStringViewLikePosN
    {
        std::string tempstdstr(t, pos, n);
        auto tempstr = SString8(tempstdstr);
        std::swap(*this, tempstr);
    }

    // Next up:
    // Rule of 5 (copy constructor by value with swap)
    // Swap
    // Spaceship
    // ==, !=
    // Then make these functions so far actually work efficiently

    //////////////
    // for the moment
    //////////////
    SString8(const SString8& /*rhs*/) noexcept = delete;
    SString8& operator=(const SString8& /*rhs*/) noexcept = delete;
    SString8(SString8&& /*rhs*/) noexcept = default;
    SString8& operator=(SString8&& /*rhs*/) noexcept = default;
    //////////////
    // for the moment
    //////////////

    ~SString8() noexcept
    {
        if (!isBuffer())
            delete getAsPtr();
    }

    // data and length
    CONSTEXPR const CharT* data() const noexcept;
    CONSTEXPR CharT* data() noexcept;

    CONSTEXPR size_type size() const noexcept;
    CONSTEXPR size_type length() const noexcept;

private:

    static constexpr auto top = 1ULL << 63U;
    static constexpr auto notTop = ~top;

    bool isBuffer() const
    {
        // highest bit is 1 if it is a pointer, 0 if it is a buffer
        auto b = (m_Storage.m_Large.m_pStr & top) == 0;
        return b;
    }
    void setBuffer()
    {
        // highest bit is 1 if it is a pointer, 0 if it is a buffer
        m_Storage.m_Large.m_pStr &= notTop;
    }

    void setLarge()
    {
        // highest bit is 1 if it is a pointer, 0 if it is a buffer
        m_Storage.m_Large.m_pStr |= top;
    }

    char* getAsPtr()
    {
        return reinterpret_cast<char*>(m_Storage.m_Large.m_pStr & notTop);
    }
    const char* getAsPtr() const
    {
        return const_cast<SString8*>(this)->getAsPtr();
    }

    struct Large
    {
        uintptr_t m_pStr = 0;
    };

    struct Buffer
    {
        char m_Buffer[8];
    };

    struct Storage
    {
        Storage() : m_Large{} {}
        union
        {
            Large m_Large;
            Buffer m_Buffer;
        };
    } m_Storage;


    /*struct Storage
    {
        char* m_pRaw = nullptr;

        Storage() noexcept = default;
        Storage(const Storage& ) noexcept = delete;
        Storage& operator=(const Storage& ) noexcept = delete;
        Storage(Storage&& ) noexcept = default;
        Storage& operator=(Storage&&) noexcept = default;

    }
    m_Storage;*/

    static CharT* emptyPtr()
    {
        static CharT empty = '\0';
        return &empty;
    }
};

// implementation - move to cpp?
#include <cstring>
#include <string>

CONSTEXPR SString8::SString8() noexcept
{
    m_Storage.m_Large.m_pStr = 0;
}

std::string_view SString8::asStringView() const
{
    return { data(), length() };
}

CONSTEXPR SString8::SString8(std::string_view str)
{
    const auto len = str.length();
    if (len <= 7)
    {
        m_Storage.m_Large.m_pStr = 0;
        memcpy(m_Storage.m_Buffer.m_Buffer, str.data(), len);
        // size and capacity ?
    }
    else
    {
        auto ptr = new char[len + 1];
        strncpy_s(ptr, len + 1, str.data(), len);
        ptr[len] = '\0';
        m_Storage.m_Large.m_pStr = reinterpret_cast<uintptr_t>(ptr);
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
        m_Storage.m_Large.m_pStr = 0;
        for (size_type i = 0; i < count; ++i)
            m_Storage.m_Buffer.m_Buffer[i] = ch;
        // size and capacity ?
    }
    else
    {
        auto ptr = new char[count + 1];
        for (size_type i = 0; i < count; ++i)
            ptr[i] = ch;
        ptr[count] = '\0';
        m_Storage.m_Large.m_pStr = reinterpret_cast<uintptr_t>(ptr);
        setLarge();
        // size and capacity ?
    }
}

CONSTEXPR SString8::SString8(const SString8& other, size_type pos, size_type count)
{
    // Constructs the string with a substring [pos, pos + count) of other. If count == npos, if count is not specified, or if the requested substring lasts past the end of the string, the resulting substring is [pos, other.size())
    std::string tempstdstr(std::string(other.asStringView()), pos, count);
    auto tempstr = SString8(tempstdstr);
    std::swap(*this, tempstr);
}

CONSTEXPR SString8::SString8(const SString8& other, size_type pos)
{
    // Constructs the string with a substring [pos, pos + count) of other. If count == npos, if count is not specified, or if the requested substring lasts past the end of the string, the resulting substring is [pos, other.size())
    std::string tempstdstr(std::string(other.asStringView()), pos);
    auto tempstr = SString8(tempstdstr);
    std::swap(*this, tempstr);
}

CONSTEXPR SString8::SString8(const CharT* s, size_type count)
{
    std::string tempstdstr(s, count);
    auto tempstr = SString8(tempstdstr);
    std::swap(*this, tempstr);
}

CONSTEXPR SString8::SString8(const CharT* s)
{
    std::string tempstdstr(s);
    auto tempstr = SString8(tempstdstr);
    std::swap(*this, tempstr);
}

template<class InputIt>
CONSTEXPR SString8::SString8(InputIt first, InputIt last)
{
    std::string tempstdstr(first, last);
    auto tempstr = SString8(tempstdstr);
    std::swap(*this, tempstr);
}

CONSTEXPR SString8::SString8(std::initializer_list<CharT> ilist)
{
    std::string tempstdstr(ilist);
    auto tempstr = SString8(tempstdstr);
    std::swap(*this, tempstr);
}


CONSTEXPR const SString8::CharT* SString8::data() const noexcept
{
    if (isBuffer())
    {
        return m_Storage.m_Buffer.m_Buffer;
    }
    else
    {
        return getAsPtr();
    }
}

CONSTEXPR SString8::CharT* SString8::data() noexcept
{
    if (isBuffer())
    {
        return m_Storage.m_Buffer.m_Buffer;
    }
    else
    {
        return getAsPtr();
    }
}

CONSTEXPR SString8::size_type SString8::size() const noexcept
{
    if (isBuffer())
    {
        // std::find ?
        return strlen(m_Storage.m_Buffer.m_Buffer);
    }
    else
    {
        return strlen(getAsPtr());
    }
}

CONSTEXPR SString8::size_type SString8::length() const noexcept
{
    return size();
}
