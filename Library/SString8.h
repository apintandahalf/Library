#pragma once

#include <cstddef>
#include <string>
#include <string_view>

#define CONSTEXPR // I'm not doing constexpr or allocators for the time being

struct SString8Data
{
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
        inline Storage() : m_Large{} {}
        union
        {
            Large m_Large;
            Buffer m_Buffer;
        };
    } m_Storage;

    static constexpr auto top = 1ULL << 63U;
    static constexpr auto notTop = ~top;

    inline bool isBuffer() const
    {
        // highest bit is 1 if it is a pointer, 0 if it is a buffer
        auto b = (m_Storage.m_Large.m_pStr & top) == 0;
        return b;
    }

    inline void setBuffer()
    {
        // highest bit is 1 if it is a pointer, 0 if it is a buffer
        m_Storage.m_Large.m_pStr &= notTop;
    }

    inline void setLarge()
    {
        // highest bit is 1 if it is a pointer, 0 if it is a buffer
        m_Storage.m_Large.m_pStr |= top;
    }

    inline char* getAsPtr()
    {
        return reinterpret_cast<char*>(m_Storage.m_Large.m_pStr & notTop);
    }
    inline const char* getAsPtr() const
    {
        return const_cast<SString8Data*>(this)->getAsPtr();
    }

    inline void swap(SString8Data& rhs)
    {
        const auto tmp = m_Storage.m_Large.m_pStr;
        m_Storage.m_Large.m_pStr = rhs.m_Storage.m_Large.m_pStr;
        rhs.m_Storage.m_Large.m_pStr = tmp;
    }

    size_t size() const
    {
        if (isBuffer())
        {
            // std::find ?
            return strlen(m_Storage.m_Buffer.m_Buffer);
        }
        else
        {
            const auto ptr = getAsPtr();
            const auto len = strlen(ptr);
            return len;
        }
    }

    inline const char* data() const noexcept
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

    inline char* data() noexcept
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

    SString8Data() = default;
    SString8Data(const SString8Data& rhs)
    {
        const auto len = rhs.size();
        if (len <= 7)
        {
            m_Storage = rhs.m_Storage;
        }
        else
        {
            auto ptr = new char[len + 1];
            strncpy_s(ptr, len + 1, rhs.getAsPtr(), len);
            ptr[len] = '\0';
            m_Storage.m_Large.m_pStr = reinterpret_cast<uintptr_t>(ptr);
            setLarge();
            // size and capacity ?
        }
    }
    SString8Data& operator=(SString8Data rhs)
    {
        swap(rhs);
    }

    SString8Data(SString8Data&& rhs)
    {
        swap(rhs);
    }
    SString8Data& operator=(SString8Data&& rhs)
    {
        swap(rhs);
    }

    ~SString8Data()
    {
        if (!isBuffer())
            delete[] getAsPtr();
    }

    SString8Data(std::string_view rhs)
    {
        const auto len = rhs.size();
        if (len <= 7)
        {
            strncpy(m_Storage.m_Buffer.m_Buffer, rhs.data(), len);
            m_Storage.m_Buffer.m_Buffer[len] = '\0';
            // size and capacity ?
        }
        else
        {
            auto ptr = new char[len + 1];
            strncpy(ptr, rhs.data(), len);
            ptr[len] = '\0';
            m_Storage.m_Large.m_pStr = reinterpret_cast<uintptr_t>(ptr);
            setLarge();
            // size and capacity ?
        }
    }
};

class SString8
{
public:
    using size_type = size_t;
    using CharT = char;

    // constructors

    SString8(std::nullptr_t) = delete;

    CONSTEXPR SString8() noexcept = default; // tested by String8TestDefaultConstructor

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

#if __cplusplus >= 202002L
    template<class StringViewLike>
        requires std::is_convertible_v<const StringViewLike&, std::basic_string_view<CharT>>
        && !std::is_convertible_v<const StringViewLike&, const CharT*>
    CONSTEXPR explicit SString8(const StringViewLike& t) //  tested by String8TestConstructorStringViewLike
    {
        std::string_view str(t);
        SString8 tempstr(str);
        swap(tempstr);
    }

    template<class StringViewLike>
        requires std::is_convertible_v<const StringViewLike&, std::basic_string_view<CharT>>
        && !std::is_convertible_v<const StringViewLike&, const CharT*>
    CONSTEXPR explicit SString8(const StringViewLike& t, size_type pos, size_type n) // tested by String8TestConstructorStringViewLikePosN
    {
        std::string_view str(t);
        SString8 tempstr(str.data() + pos, str.data() + pos + n);
        swap(tempstr);
    }
#endif

    void swap(SString8& rhs) // @TOTEST
    {
        m_Storage.swap(rhs.m_Storage);
    }
    friend void swap(SString8& lhs, SString8& rhs) // @TOTEST
    {
        lhs.swap(rhs);
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

    //~SString8() noexcept = default;

    // data and length
    CONSTEXPR const CharT* data() const noexcept;
    CONSTEXPR CharT* data() noexcept;

    CONSTEXPR size_type size() const noexcept;
    CONSTEXPR size_type length() const noexcept;

private:

    SString8Data m_Storage;

    inline bool isBuffer() const
    {
        return m_Storage.isBuffer();
    }
    void setBuffer()
    {
        m_Storage.setBuffer();
    }

    void setLarge()
    {
        m_Storage.setLarge();
    }

    char* getAsPtr()
    {
        return m_Storage.getAsPtr();
    }
    const char* getAsPtr() const
    {
        return m_Storage.getAsPtr();
    }

    /* 
    Unused?
    static CharT* emptyPtr()
    {
        static CharT empty = '\0';
        return &empty;
    }*/
};

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

CONSTEXPR SString8::SString8(const SString8& other, size_type pos)
{
    // Constructs the string with a substring [pos, pos + count) of other. If count == npos, if count is not specified, or if the requested substring lasts past the end of the string, the resulting substring is [pos, other.size())
    std::string tempstdstr(std::string(other.asStringView()), pos);
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

template<class InputIt>
CONSTEXPR SString8::SString8(InputIt first, InputIt last)
{
    std::string tempstdstr(first, last);
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
