#pragma once

#include <cstddef>
#include <string>
#include <string_view>

#define CONSTEXPR // no constexpr or allocators for the time being

/** Abstraction of the storage.  Not intended to be used in isolation */
struct SString8Data
{
    struct Buffer
    {
        char m_Buffer[8];
    };

    struct Storage
    {
        union
        {
            uintptr_t m_pLargeStr;
            Buffer m_Buffer;
        };
        inline Storage() : m_pLargeStr(0) {}
        static_assert(sizeof uintptr_t == sizeof Buffer);
    } m_Storage;

    static inline constexpr auto top = 1ULL << 63U;
    static inline constexpr auto notTop = ~top;

    inline bool isBuffer() const noexcept
    {
        // highest bit is 1 if it is a pointer, 0 if it is a buffer
        return (m_Storage.m_pLargeStr & top) == 0;
    }

    inline void setBuffer() noexcept
    {
        // highest bit is 1 if it is a pointer, 0 if it is a buffer
        m_Storage.m_pLargeStr &= notTop;
    }

    inline void setLarge() noexcept
    {
        // highest bit is 1 if it is a pointer, 0 if it is a buffer
        m_Storage.m_pLargeStr |= top;
    }

    inline char* getAsPtr() noexcept
    {
        return reinterpret_cast<char*>(m_Storage.m_pLargeStr & notTop);
    }
    inline const char* getAsPtr() const noexcept
    {
        return reinterpret_cast<const char*>(m_Storage.m_pLargeStr & notTop);
    }

    inline void swap(SString8Data& rhs) noexcept
    {
        std::swap(m_Storage.m_pLargeStr, rhs.m_Storage.m_pLargeStr);
    }

    size_t size() const noexcept
    {
        if (isBuffer())
            return strlen(m_Storage.m_Buffer.m_Buffer); // std::find ?

        return strlen(getAsPtr());
    }

    inline char* data() noexcept
    {
        if (isBuffer())
            return m_Storage.m_Buffer.m_Buffer;

        return getAsPtr();
    }
    inline const char* data() const noexcept
    {
        if (isBuffer())
            return m_Storage.m_Buffer.m_Buffer;

        return getAsPtr();
    }

    SString8Data() = default;

public:
    SString8Data(const SString8Data& rhs) // test - SString8DataTestConstructorCopy
        : SString8Data(CharStarLenPr{ rhs.data(), rhs.size() })
    {
    }

    SString8Data& operator=(SString8Data rhs) noexcept // test SString8DataTestAssignement
    {
        swap(rhs);
        return *this;
    }

    SString8Data(SString8Data&& rhs) noexcept // test - SString8DataTestConstructorMove
    {
        swap(rhs);
    }

    ~SString8Data() noexcept
    {
        if (!isBuffer())
            delete[] getAsPtr();
    }

    SString8Data(std::string_view rhs) // test - SString8DataTestConstructorStringView
        : SString8Data(CharStarLenPr{ rhs.data(), rhs.size() })
    {
    }

private:
    struct CharStarLenPr
    {
        const char* pStr;
        size_t len;
    };
    SString8Data(CharStarLenPr data)
    {
        const auto [pRhs, len] = data;
        if (len <= 7)
        {
            strncpy(m_Storage.m_Buffer.m_Buffer, pRhs, len);
            m_Storage.m_Buffer.m_Buffer[len] = '\0';
        }
        else
        {
            auto ptr = new char[len + 1];
            strncpy_s(ptr, len + 1, pRhs, len);
            ptr[len] = '\0';
            m_Storage.m_pLargeStr = reinterpret_cast<uintptr_t>(ptr);
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
    CONSTEXPR SString8(const std::string& other, size_type pos); // tested by String8TestConstructorOtherPos

    CONSTEXPR SString8(const SString8& other, size_type pos, size_type count); // tested by String8TestConstructorOtherPosCount
    CONSTEXPR SString8(const std::string& other, size_type pos, size_type count); // tested by String8TestConstructorOtherPosCount

    CONSTEXPR SString8(const CharT* s, size_type count); // tested by String8TestConstructorCharStarCount

    CONSTEXPR SString8(const CharT* s); // tested by String8TestConstructorCharStar

    template<class InputIt>
    SString8(InputIt first, InputIt last) // tested by String8TestConstructorInputItFirstLast
    {
        std::string tempstdstr(first, last);
        auto tempstr = SString8(tempstdstr);
        swap(tempstr);
    }

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

    inline void swap(SString8& rhs) // tested by SString8TestSwap
    {
        m_Storage.swap(rhs.m_Storage);
    }
    inline friend void swap(SString8& lhs, SString8& rhs) // tested by SString8TestSwap
    {
        lhs.swap(rhs);
    }


    // Next up:
    // Then make these functions so far actually work efficiently

    SString8(const SString8& /*rhs*/) = default; // SString8TestConstructorCopy
    SString8& operator=(const SString8& /*rhs*/) = default; // SString8TestAssignment
    SString8(SString8&& /*rhs*/) noexcept = default; // SString8TestConstructorMove
    SString8& operator=(SString8&& /*rhs*/) noexcept = default; // SString8TestAssignmentMove

    ~SString8() noexcept = default;

    // data and length
    CONSTEXPR const CharT* data() const noexcept;
    CONSTEXPR CharT* data() noexcept;

    CONSTEXPR size_type size() const noexcept;
    CONSTEXPR size_type length() const noexcept;

    auto operator<=>(const SString8& rhs) const noexcept // tested by SString8TestSpaceshipEqEq
    {
        const auto thislen = size();
        const auto thatlen = rhs.size();

        if (auto cmp = thislen<=>thatlen; cmp != 0)
            return cmp;

        const auto thisdata = std::string_view(data(), thislen);
        const auto thatdata = std::string_view(rhs.data(), thislen);
        const auto cmp = thisdata <=> thatdata;
        return cmp;
    }
    bool operator==(const SString8& rhs) const noexcept // tested by SString8TestSpaceshipEqEq
    {
        const auto thislen = size();
        const auto thatlen = rhs.size();

        if (thislen != thatlen)
            return false;

        const auto thisdata = data();
        const auto thatdata = rhs.data();

        const auto cmp = strncmp(thisdata, thatdata, thislen);
        return 0 == cmp;
    }

private:

    SString8Data m_Storage;

    inline bool isBuffer() const
    {
        return m_Storage.isBuffer();
    }

    inline void setBuffer()
    {
        m_Storage.setBuffer();
    }

    inline void setLarge()
    {
        m_Storage.setLarge();
    }

    inline char* getAsPtr()
    {
        return m_Storage.getAsPtr();
    }
    inline const char* getAsPtr() const
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
