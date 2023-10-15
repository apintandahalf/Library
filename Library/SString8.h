#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <bit>

#define CONSTEXPR // no constexpr or allocators for the time being

/**
Abstraction of the storage.  Not intended to be used in isolation

How the string, size and capacity are stored

There are 4 size flavours - buffer (0-7 chars), small (8-255), medium (256-2^15)and large (>2^15).  Note thathese lengths refer to the capacity, not the length of the string actually stored.
It is not expected that large will be used very often as this class is all about storage size, and if you are storing GB of string data then there are probably better classes.
Note that "Byte 7" and "last byte" are synonyms here (as arrays start at byte 0).
Nibbles:
    "lower nibble" is bits 0-3 of a byte, and "upper nibble" is bits 4-7.  
    Thus the decimal number 7, which is 0b00000111 in binary, has 0b0000 for its upper nibble and 0x0111 for its lower nibble.
Heap vs bufffer allocation is determined by bit 7 of byte 7, or bit 63 of the entire 8 bytes. The top bit.  1 is heap, 0 is buffer.
Buffer
    Stores up to 7 characters (excluding null terminator)
    The top bit of byte 7 is 0, and the other 7 bits hold the length, or rather (7-length), which is thus 0 when there are 7 characters.  The other 7 bytes hold the characters.
    No heap allocation.  Null terminator is still present.
    If the string is 0 characters then the buffer is all 0.
    If the string is 1->6 characters then there is a null terminator after the last character, and the last byte contains (7-length) in lower nibble (and 0 in upper nibble), and size() returns (7 - last byte).
    If the character is 7 characters then the last byte is 0 (and so length is 7).
    Capacity is always 7.
Small, Medium and Large
    Stores strings 8 characters and longer (excluding null terminator)
    The top bit of byte 7 is 1.  The address of the char buffer is stored in the rest of the bits.
    A naive implementation would leave it at that and store the size capacity along with the heap allocation, as the first 16 bytes before the string.
    However, there are some bits which are available for use.
    A heap allocation on a 64 bit system will always have the smallest 3 bits set to 0, so these three bits can be used to distinguish between different sizes of heap allocation.
    Futher more, on all current hardware bytes 6 and 7 of heap allocated memory are always 0, so these can be used to store size and capacity if the string is short enough,
    meaning that it is not necessary to go to the allocation itself to determine these values until the size or capacity get big, which is not the use case for SString8.
    Small, Medium and large are distinguished by bits 0 and 1 of byte 0
    Small
        Stores 8 to 255 characters (excluding null terminator)
        Bits 0 and 1 of byte 0 are 0b00
        The remainder of byte 0 and bytes 1-5 store the address of the string.
        Byte 6 contains the length.
        Byte 7 contains the capacity. The top bit is always 1 (to indicate heap allocation), so an odd capacity is increased by 1 and then left shifted by 1.
        size() returns byte 6.
        capacity() returns ( ((byte 7) & 0b01111111) >> 1).
    Medium
        Stores 256 to 2^15 characters (excluding null terminator)
        Bits 0 and 1 of byte 0 are 0b01
        The remainder of byte 0 and bytes 1-5 store the address of the string.
        Bytes 6 and 7 contain the length. The top bit is always 1 (to indicate heap allocation), leaving 15 bits, so the maximum size that can be stored is 2^15.
        The capacity (4 bytes) is stored in the first 4 bytes of the heap allocation.  To ensure that the string is 8 byte alligned, there is also 4 bytes of padding.  Thus the string starts at (allocation address)+8
    Large
        Stores strings greater than 2^15 in length
        Bits 0 and 1 of byte 0 are 0b01
        The remainder of byte 0 and bytes 1-5 store the address of the string.
        Byte 6 and 7 contain the length. The top bit is always 1 (to indicate heap allocation), so the maximum size that can be stored is 2^15.
        Size is contained in the first 8 bytes of the allocation, and capacity is stored in the second 8 bytes of the allocation.  Thus the string starts at (allocation address)+16
*/
struct SString8Data
{
    // Only little endian is supported.  Comment this line out if your compiling at below c++20 and you are confident that your system is little endian
    static_assert(std::endian::native == std::endian::little);
    // Only 64 bit systems are supported.  32 bit is possible but there's probably not much point.  128 bit systems are not common (yet) - will cross that bridge when the time comes
    static_assert(8 == sizeof(char*));

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
        static_assert(sizeof uintptr_t == sizeof Buffer); // make sure that we aren't accidently introducing some padding
    } m_Storage;
    static_assert(sizeof uintptr_t == sizeof m_Storage); // make sure that we aren't accidently introducing some padding

    static inline constexpr auto top = 1ULL << 63U;
    static inline constexpr auto notTop = ~top;

    // highest bit is 1 if it is a pointer, 0 if it is a buffer
    inline bool isPtr()    const noexcept { return (m_Storage.m_pLargeStr & top) == 1; }
    inline bool isBuffer() const noexcept { return !isPtr(); }
    inline bool isSmall()  const noexcept { return isPtr() && (m_Storage.m_pLargeStr & 0b11) == 0b00; }
    inline bool isMedium() const noexcept { return isPtr() && (m_Storage.m_pLargeStr & 0b11) == 0b01; }
    inline bool isLarge()  const noexcept { return isPtr() && (m_Storage.m_pLargeStr & 0b11) == 0b10; }

    // highest bit is 1 if it is a pointer, 0 if it is a buffer
    inline void setBuffer() noexcept { m_Storage.m_pLargeStr &= notTop; }
    inline void setPtr()    noexcept { m_Storage.m_pLargeStr |= top; m_Storage.m_pLargeStr &= 0b00; } // set bottom 2 bits to 0 as well
    inline void setSmall()  noexcept { setPtr(); /* setPtr already sets bottom 2 bits to 0 - no need to do it again */ }
    inline void setMedium() noexcept { setPtr(); m_Storage.m_pLargeStr |= 0b01; }
    inline void setLarge()  noexcept { setPtr(); m_Storage.m_pLargeStr |= 0b10; }

    // strip out the top bit and the lowest 2 bits
    inline       char* getAsPtr()       noexcept { return reinterpret_cast<char*>      (m_Storage.m_pLargeStr & notTop & 0b00); }
    inline const char* getAsPtr() const noexcept { return reinterpret_cast<const char*>(m_Storage.m_pLargeStr & notTop & 0b00); }

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
        : SString8Data(rhs.data(), rhs.size())
    {
    }

    SString8Data& operator=(SString8Data rhs) noexcept // test - SString8DataTestAssignement
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
        if (isPtr())
            delete[] getAsPtr();
    }

    SString8Data(std::string_view rhs) // test - SString8DataTestConstructorStringView
        : SString8Data(rhs.data(), rhs.size())
    {
    }

    /**
    Assumes that len is the number of chars pointed to by pRhs, not including any null terminator.  
    Null terminator not required
    */
    SString8Data(const char* pRhs, size_t len)
    {
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

    CONSTEXPR SString8() noexcept = default; // test - String8TestDefaultConstructor

    SString8(std::string_view str); // test - String8TestConstructorStringView
    SString8(const std::string& str); // test - String8TestConstructorString
    std::string_view asStringView() const; // test - String8TestAsStringView

    CONSTEXPR SString8(size_type count, CharT ch); // test - String8TestConstructorCountChar

    CONSTEXPR SString8(const SString8& other, size_type pos); // test - String8TestConstructorOtherPos
    CONSTEXPR SString8(const std::string& other, size_type pos); // test - String8TestConstructorOtherPos

    CONSTEXPR SString8(const SString8& other, size_type pos, size_type count); // test - String8TestConstructorOtherPosCount
    CONSTEXPR SString8(const std::string& other, size_type pos, size_type count); // test - String8TestConstructorOtherPosCount

    CONSTEXPR SString8(const CharT* s, size_type count); // test - String8TestConstructorCharStarCount

    CONSTEXPR SString8(const CharT* s); // test - String8TestConstructorCharStar

    template<class InputIt>
    SString8(InputIt first, InputIt last) // test - String8TestConstructorInputItFirstLast
    {
        std::string tempstdstr(first, last);
        auto tempstr = SString8(tempstdstr);
        swap(tempstr);
    }

    CONSTEXPR SString8(std::initializer_list<CharT> ilist); // test - String8TestConstructorInitialiserList

#if __cplusplus >= 202002L
    template<class StringViewLike>
        requires std::is_convertible_v<const StringViewLike&, std::basic_string_view<CharT>>
        && !std::is_convertible_v<const StringViewLike&, const CharT*>
    CONSTEXPR explicit SString8(const StringViewLike& t) //  test - String8TestConstructorStringViewLike
    {
        std::string_view str(t);
        SString8 tempstr(str);
        swap(tempstr);
    }

    template<class StringViewLike>
        requires std::is_convertible_v<const StringViewLike&, std::basic_string_view<CharT>>
        && !std::is_convertible_v<const StringViewLike&, const CharT*>
    CONSTEXPR explicit SString8(const StringViewLike& t, size_type pos, size_type n) // test - String8TestConstructorStringViewLikePosN
    {
        std::string_view str(t);
        SString8 tempstr(str.data() + pos, str.data() + pos + n);
        swap(tempstr);
    }
#endif

    inline void swap(SString8& rhs) // test - SString8TestSwap
    {
        m_Storage.swap(rhs.m_Storage);
    }
    inline friend void swap(SString8& lhs, SString8& rhs) // test - SString8TestSwap
    {
        lhs.swap(rhs);
    }

    SString8(const SString8& /*rhs*/) = default; // SString8TestConstructorCopy
    SString8& operator=(const SString8& /*rhs*/) = default; // SString8TestAssignment
    SString8(SString8&& /*rhs*/) noexcept = default; // SString8TestConstructorMove
    SString8& operator=(SString8&& /*rhs*/) noexcept = default; // SString8TestAssignmentMove

    ~SString8() noexcept = default;

    // data and length
    CONSTEXPR const CharT* data() const noexcept; // test - SString8TestData
    CONSTEXPR CharT* data() noexcept; // test - SString8TestData

    CONSTEXPR size_type size() const noexcept; // test - SString8TestSizeLength
    CONSTEXPR size_type length() const noexcept; // test - SString8TestSizeLength

    auto operator<=>(const SString8& rhs) const noexcept // test - SString8TestSpaceshipEqEq
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
    bool operator==(const SString8& rhs) const noexcept // test - SString8TestSpaceshipEqEq
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

    void reserve(size_t new_cap = 0);

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
