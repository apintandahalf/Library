#pragma once

#include <cstddef>

#define CONSTEXPR //constexpr
class SString8
{
public:
    using size_type = size_t;
    using CharT = char;

    // constructors

    SString8(std::nullptr_t) = delete;

    //SString8() noexcept = default; // (until C++20)
    CONSTEXPR SString8() noexcept = default; // (since C++20)

    //SString8(size_type count, CharT ch); // (until C++20)
    CONSTEXPR SString8(size_type count, CharT ch); // (since C++20)

    // for the moment
    SString8(const SString8& /*rhs*/) noexcept = delete;
    SString8& operator=(const SString8& /*rhs*/) noexcept = delete;
    SString8(SString8&& /*rhs*/) noexcept = default;
    SString8& operator=(SString8&& /*rhs*/) noexcept = default;
    ~SString8() noexcept = default;
    // for the moment


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
    struct Storage
    {
        char* m_pRaw = nullptr;

        Storage() noexcept = default;
        Storage(const Storage& /*rhs*/) noexcept = delete;
        Storage& operator=(const Storage& /*rhs*/) noexcept = delete;
        Storage(Storage&& /*rhs*/) noexcept = default;
        Storage& operator=(Storage&& /*rhs*/) noexcept = default;

        ~Storage()
        {
            delete[] m_pRaw;
        }
    }
    m_Storage;

    static CharT* emptyPtr()
    {
        static CharT empty = '\0';
        return &empty;
    }
};

// implementation - move to cpp?
#include <cstring>
CONSTEXPR SString8::SString8(size_type count, CharT ch)
{
    if (count == 0)
        return;
    m_Storage.m_pRaw = new char[count + 1];
    for (size_type i=0; i<count; ++i)
        m_Storage.m_pRaw[i] = ch;
    m_Storage.m_pRaw[count] = '\0';
}

CONSTEXPR const SString8::CharT* SString8::data() const noexcept
{
    return const_cast<SString8*>(this)->data();
}

CONSTEXPR SString8::CharT* SString8::data() noexcept
{
    if (nullptr == m_Storage.m_pRaw)
        return emptyPtr();
    return m_Storage.m_pRaw;
}

CONSTEXPR SString8::size_type SString8::size() const noexcept
{
    if (nullptr == m_Storage.m_pRaw)
        return 0;
    return strlen(m_Storage.m_pRaw);
}

CONSTEXPR SString8::size_type SString8::length() const noexcept
{
    return size();
}
