#pragma once

#include <cstddef>

#define CONSTEXPR //constexpr - only valid for C++17
class SString8
{
public:
    using size_type = size_t;
    using CharT = char;

    // constructors

    SString8(std::nullptr_t) = delete;

    //SString8() noexcept = default; // (until C++20)
    CONSTEXPR SString8() noexcept; // (since C++20)

    //SString8(size_type count, CharT ch); // (until C++20)
    CONSTEXPR SString8(size_type count, CharT ch); // (since C++20)

    // for the moment
    SString8(const SString8& /*rhs*/) noexcept = delete;
    SString8& operator=(const SString8& /*rhs*/) noexcept = delete;
    SString8(SString8&& /*rhs*/) noexcept = default;
    SString8& operator=(SString8&& /*rhs*/) noexcept = default;
    // for the moment

    ~SString8() noexcept
    {
        if (!isBuffer())
            delete getAsPtr();
    }

    // data and length
    //const CharT* data() const noexcept; //(since C++11) // (until C++20)
    CONSTEXPR const CharT* data() const noexcept; // (since C++20)
    //CharT* data() noexcept; // (since C++17) // (until C++20)
    CONSTEXPR CharT* data() noexcept; // (since C++20)

    //size_type size() const noexcept; // (since C++11) // (until C++20)
    CONSTEXPR size_type size() const noexcept; // (since C++20)
    //size_type length() const noexcept; // (since C++11) // (until C++20)
    CONSTEXPR size_type length() const noexcept; // (since C++20)

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
        uintptr_t m_pStr;
    };

    struct Buffer
    {
        char m_Buffer[8];
    };

    struct Raw
    {
        char m_Raw[8];
    };

    struct Storage
    {
        union
        {
            Large m_Large;
            Buffer m_Buffer;
            Raw m_Raw;
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

CONSTEXPR SString8::SString8() noexcept
{
    m_Storage.m_Large.m_pStr = 0;
}

CONSTEXPR SString8::SString8(size_type count, CharT ch)
{
    if (count <= 7)
    {
        m_Storage.m_Large.m_pStr = 0;
        for (size_type i = 0; i < count; ++i)
            m_Storage.m_Buffer.m_Buffer[i] = ch;
        m_Storage.m_Buffer.m_Buffer[count] = '\0';
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
