#pragma once

#include <bit>
#include <string_view>
#include <limits>
#include <cstring>

// assert
#ifdef _DEBUG
#include <cstdlib>
#include <iostream>

inline void do_assert(bool expr, const char* expression)
{
    if (!expr)
    {
        // handle failed assertion
        std::cout << expression << "\n" << std::flush;
        std::abort();
    }
}
#endif

#ifdef _DEBUG
#define ASSERT(Expr) \
    do_assert(Expr, #Expr)
#else
#define ASSERT(Expr) 
#endif

namespace SString8Detail
{
    /**
    Abstraction of the SString8 storage.  Not intended to be used in isolation.
    Everything is public because it is assumed that all access (other than for testing) is done through SString8.

    How the string, size and capacity are stored

    There are 4 size flavours - buffer (0-7 chars), small (8-254), medium (255-2^15)and large (>2^15).  Note thathese lengths refer to the capacity, not the length of the string actually stored.
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
            Stores 8 to 254 characters (excluding null terminator)
            Bits 0 and 1 of byte 0 are 0b00
            The remainder of byte 0 and bytes 1-5 store the address of the string.
            Byte 6 contains the length.
            Byte 7 contains the capacity. The top bit is always 1 (to indicate heap allocation), so an odd capacity is increased by 1 and then left shifted by 1.
            size() returns byte 6.
            capacity() returns ( ((byte 7) & 0b01111111) >> 1).
        Medium
            Stores 255 to 2^15 characters (excluding null terminator)
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
        // a char is one byte
        static_assert(1 == sizeof(char));
        // and each byte has 8 bits
        static_assert(8 == CHAR_BIT);
        
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
            inline Storage()
                : m_Buffer{ 0,0,0,0,0,0,0,'\7' }
            {}
            static_assert(sizeof uintptr_t == sizeof Buffer); // make sure that we aren't accidently introducing some padding
        } m_Storage;
        static_assert(sizeof uintptr_t == sizeof m_Storage); // make sure that we aren't accidently introducing some padding

        static inline constexpr auto top = 1ULL << 63U;
        static inline constexpr auto notTop = ~top;
        static inline constexpr auto small_lower_bits = 0b00ULL;
        static inline constexpr auto medium_lower_bits = 0b01ULL;
        static inline constexpr auto large_lower_bits = 0b10ULL;
        static inline constexpr auto small_bits = top | small_lower_bits;
        static inline constexpr auto medium_bits = top | medium_lower_bits;
        static inline constexpr auto large_bits = top | large_lower_bits;
        static inline constexpr auto fifeteen_bites_set = 0x7FFFULL;
        static inline constexpr auto max_size_small = 254ULL;

        static inline constexpr auto lowest_two_bits_zero = ~0b11ULL;
        static inline constexpr auto not_top_two_bytes_or_bottom_two_bits = 0x00ULL
            | (0xFFULL << 40)
            | (0xFFULL << 32)
            | (0xFFULL << 24)
            | (0xFFULL << 16)
            | (0xFFULL << 8)
            | 0b11111100;
        //notTop & lowest_two_bits_zero;

        /** Calculate actual capacity from desired.  Actual will never be less than desired */
        static size_t calcCapacity(size_t desired_cap)
        {
            // small strings get an even capacity
            if ((desired_cap > 7) && (desired_cap <= max_size_small) && ((desired_cap & 1) != 0))
                return desired_cap + 1;
            return desired_cap;
        }

        // highest bit is 1 if it is a pointer, 0 if it is a buffer
        inline bool isPtr() const noexcept
        {
            return (m_Storage.m_pLargeStr & top) != 0;
        }
        enum class StorageType { BUFFER, SMALL, MEDIUM, LARGE };
        inline bool isBuffer() const noexcept { return !isPtr(); }
        inline bool isSmall()  const noexcept { return isPtr() && (m_Storage.m_pLargeStr & 0b11) == small_lower_bits; }
        inline bool isMedium() const noexcept { return isPtr() && (m_Storage.m_pLargeStr & 0b11) == medium_lower_bits; }
        inline bool isLarge()  const noexcept { return isPtr() && (m_Storage.m_pLargeStr & 0b11) == large_lower_bits; }
        [[nodiscard]] StorageType getStorageType() const
        {
            // @todo speed this up
            if (isBuffer())
                return StorageType::BUFFER;
            if (isSmall())
                return StorageType::SMALL;
            if (isMedium())
                return StorageType::MEDIUM;
            ASSERT(isLarge());
            return StorageType::LARGE;
        }

        // highest bit is 1 if it is a pointer, 0 if it is a buffer
        inline void setBuffer() noexcept { m_Storage.m_Buffer.m_Buffer[7] &= 0b01111111; }
        inline void clearPtrSizeBits() noexcept { m_Storage.m_Buffer.m_Buffer[0] &= 0b11111100; }
        inline void setSmall(size_t len, size_t cap)  noexcept
        { // 6 length, 7 capacity
            clearPtrSizeBits();
            *reinterpret_cast<uint8_t*>(&m_Storage.m_Buffer.m_Buffer[6]) = static_cast<uint8_t>(len);
            *reinterpret_cast<uint8_t*>(&m_Storage.m_Buffer.m_Buffer[7]) = static_cast<uint8_t>(cap >> 1U);
            m_Storage.m_pLargeStr |= small_bits;
        }
        inline void setMedium(size_t len, size_t cap) noexcept
        {
            // 6 and 7 length
            clearPtrSizeBits();
            auto pCap = reinterpret_cast<uint64_t*>(reinterpret_cast<char*>(m_Storage.m_pLargeStr));
            *pCap = cap;
            auto pLen = reinterpret_cast<uint16_t*>(&m_Storage.m_Buffer.m_Buffer[6]);
            *pLen = static_cast<uint16_t>(len);
            m_Storage.m_pLargeStr |= medium_bits;
        }
        inline void setLarge(size_t len, size_t cap)  noexcept
        {
            clearPtrSizeBits();
            auto pLen = reinterpret_cast<uint64_t*>(reinterpret_cast<char*>(m_Storage.m_pLargeStr));
            *pLen = len;
            auto pCap = reinterpret_cast<uint64_t*>(8U + reinterpret_cast<char*>(m_Storage.m_pLargeStr));
            *pCap = cap;
            m_Storage.m_pLargeStr |= large_bits;
        }

        // strip out the top bit and the lowest 2 bits
        inline       char* getAsPtr()       noexcept
        {
            return reinterpret_cast<char*>(m_Storage.m_pLargeStr & not_top_two_bytes_or_bottom_two_bits);
        }
        inline const char* getAsPtr() const noexcept
        {
            return reinterpret_cast<const char*>(m_Storage.m_pLargeStr & not_top_two_bytes_or_bottom_two_bits);
        }

        inline void swap(SString8Data& rhs) noexcept
        {
            std::swap(m_Storage.m_pLargeStr, rhs.m_Storage.m_pLargeStr);
        }

        size_t size() const noexcept
        {
            switch (getStorageType())
            {
            case StorageType::BUFFER:
                return 7 - m_Storage.m_Buffer.m_Buffer[7]; // std::find ?
            case StorageType::SMALL:
                return *reinterpret_cast<const uint8_t*>(&m_Storage.m_Buffer.m_Buffer[6]);
            case StorageType::MEDIUM:
                return *reinterpret_cast<const uint16_t*>(&m_Storage.m_Buffer.m_Buffer[6]) & ~(1U << 15U); // remove the top bit
            case StorageType::LARGE:
                auto ptr = getAsPtr();
                auto pLen = reinterpret_cast<const uint64_t*>(reinterpret_cast<const char*>(ptr));
                return *pLen;
            }
            ASSERT(false);
            return 0;
        }

        // assumes that it is already in the correct size format
        void setSize(size_t sz) noexcept
        {
            switch (getStorageType())
            {
            case StorageType::BUFFER:
            {
                ASSERT(sz <= 7);
                m_Storage.m_Buffer.m_Buffer[7] = static_cast<char>(7 - sz);
                break;
            }
            case StorageType::SMALL:
            {
                auto pLen = reinterpret_cast<uint8_t*>(&m_Storage.m_Buffer.m_Buffer[6]);
                *pLen = static_cast<uint8_t>(sz);
                break;
            }
            case StorageType::MEDIUM:
            {
                auto pLen = reinterpret_cast<uint16_t*>(&m_Storage.m_Buffer.m_Buffer[6]);
                *pLen = static_cast<uint16_t>(sz | (1U << 15U));
                break;
            }
            case StorageType::LARGE:
            {
                auto ptr = getAsPtr();
                auto pLen = reinterpret_cast<uint64_t*>(reinterpret_cast<char*>(ptr));
                *pLen = sz;
                break;
            }
            default:
                ASSERT(false);
            }
        }

        size_t capacity() const noexcept
        {
            switch (getStorageType())
            {
            case StorageType::BUFFER:
            {
                return 7;
            }
            case StorageType::SMALL:
            {
                size_t sz = (*reinterpret_cast<const uint8_t*>(&m_Storage.m_Buffer.m_Buffer[7])) & ~(1U << 7U); // remove the top bit
                return sz << 1U; // its an even number stored shifted right 1 to make room for the ptr indicator
            }
            case StorageType::MEDIUM:
            {
                const auto ptr = getAsPtr();
                auto uintPtr = reinterpret_cast<const uint64_t*>(reinterpret_cast<const char*>(ptr));
                return *uintPtr;
            }
            case StorageType::LARGE:
            {
                auto ptr = getAsPtr();
                auto pCap = reinterpret_cast<const uint64_t*>(8U + reinterpret_cast<const char*>(ptr));
                return *pCap;
            }
            }
            ASSERT(false);
            return 0;
        }

        std::pair<size_t, size_t> getSizeAndCap() const
        {
            return { size(), capacity() }; // @todo - speed this up to avoid multiple calls to isSmall etc
        }

        std::pair<char*, size_t> getDataAndSize()
        {
            return { data(), size() }; // @todo - speed this up to avoid multiple calls to isSmall etc
        }

        std::pair<char*, size_t> getDataAndCap()
        {
            return { data(), capacity() }; // @todo - speed this up to avoid multiple calls to isSmall etc
        }

        std::pair<const char*, size_t> getDataAndSize() const
        {
            return { data(), size() }; // @todo - speed this up to avoid multiple calls to isSmall etc
        }

        inline char* data() noexcept
        {
            if (isBuffer())
                return m_Storage.m_Buffer.m_Buffer;
            if (isSmall())
                return getAsPtr();
            if (isMedium())
                return 8U + getAsPtr();
            return 16U + getAsPtr();
        }

        inline const char* data() const noexcept
        {
            if (isBuffer())
                return m_Storage.m_Buffer.m_Buffer;
            if (isSmall())
                return getAsPtr();
            if (isMedium())
                return 8U + getAsPtr();
            return 16U + getAsPtr();
        }

        SString8Data() = default;

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

        /** Assumes that we are doing a heap allocation not a buffer storage. Does not deallocate */
        void allocatePtr(const char* pRhs, size_t len, size_t cap)
        {
            const auto offset = (cap <= max_size_small) ? 0U : (cap <= fifeteen_bites_set) ? 8U : 16U;
            auto ptr = new char[cap + offset + 1];
            strncpy_s(ptr + offset, len + 1, pRhs, len);
            ptr[len + offset] = '\0';
            m_Storage.m_pLargeStr = reinterpret_cast<uintptr_t>(ptr);
            if (cap <= max_size_small)
                setSmall(len, cap);
            else if (cap <= fifeteen_bites_set)
                setMedium(len, cap);
            else
                setLarge(len, cap);
        }

        void allocate(const char* pRhs, size_t len)
        {
            if (len <= 7)
            {
                strncpy(m_Storage.m_Buffer.m_Buffer, pRhs, len);
                m_Storage.m_Buffer.m_Buffer[len] = '\0';
                m_Storage.m_Buffer.m_Buffer[7] = static_cast<char>(7 - len);
            }
            else
            {
                const auto cap = calcCapacity(len);
                allocatePtr(pRhs, len, cap);
            }
        }

        void allocateWithDeallocate(const char* pRhs, size_t len)
        {
            if (isPtr())
                delete[] getAsPtr();
            allocate(pRhs, len);
        }

        /**
        Assumes that len is the number of chars pointed to by pRhs, not including any null terminator.
        If this is not true then bad things will happen.
        Null terminator not required.
        */
        SString8Data(const char* pRhs, size_t len)
        {
            allocate(pRhs, len);
        }

        /** if the requested amount is bigger than what is currently available, then do a heap allocation to the new capacity, copying across and then deleting the old string */
        void reserve(size_t new_cap)
        {
            if (new_cap <= capacity())
                return;
            // small strings get an even capacity
            new_cap = calcCapacity(new_cap);

            auto oldPtr = getAsPtr();
            const auto bWasPtr = isPtr();
            auto [pOld, sz] = getDataAndSize();
            allocatePtr(pOld, sz, new_cap);
            if (bWasPtr)
                delete[] oldPtr;
        }

        /** Allocate enough space for count+1 (including potentially even capacity and the null terminator)
           then write in count copies of ch, and null terminate */
        SString8Data(size_t count, char ch);
    };
} // namespace detail

#include <cstddef>
#include <string>
#include <stdexcept>
#include <ostream>

/**
An alternative to std::string which is only 8 bytes in size as opposed to the usual 24-32 bytes.
Only valid on a 64 bit  system.
Otimised for very short strings - has a 7 byte buffer (as opposed to the more usual 15 or 23)
It does support the full range of string sizes, but once the string is bigger than 254 bytes it will be less performant than a std::string (though still smaller)
No documentation as it is the same as std::string
*/
class SString8
{
public:
    using size_type = size_t;
    using CharT = char;

    // constructors

    SString8(std::nullptr_t) = delete;

    SString8() noexcept = default; // test - String8TestDefaultConstructor

    SString8(std::string_view str); // test - String8TestConstructorStringView
    SString8(const std::string& str); // test - String8TestConstructorString
    operator std::string_view() const; // test - String8Testoperatotstdstringview

    SString8(size_type count, CharT ch); // test - String8TestConstructorCountChar

    SString8(const SString8& other, size_type pos); // test - String8TestConstructorOtherPos
    SString8(const std::string& other, size_type pos); // test - String8TestConstructorOtherPos

    SString8(const SString8& other, size_type pos, size_type count); // test - String8TestConstructorOtherPosCount
    SString8(const std::string& other, size_type pos, size_type count); // test - String8TestConstructorOtherPosCount

    SString8(const CharT* s, size_type count); // test - String8TestConstructorCharStarCount

    SString8(const CharT* s); // test - String8TestConstructorCharStar

    template<class InputIt>
    SString8(InputIt first, InputIt last) // test - String8TestConstructorInputItFirstLast
    {
        std::string tempstdstr(first, last);
        auto tempstr = SString8(tempstdstr);
        swap(tempstr);
    }

    SString8(std::initializer_list<CharT> ilist); // test - String8TestConstructorInitialiserList

#if __cplusplus >= 202002L
    template<class StringViewLike>
        requires std::is_convertible_v<const StringViewLike&, std::basic_string_view<CharT>>
        && !std::is_convertible_v<const StringViewLike&, const CharT*>
    explicit SString8(const StringViewLike& t) //  test - String8TestConstructorStringViewLike
    {
        std::string_view str(t);
        SString8 tempstr(str);
        swap(tempstr);
    }

    template<class StringViewLike>
        requires std::is_convertible_v<const StringViewLike&, std::basic_string_view<CharT>>
        && !std::is_convertible_v<const StringViewLike&, const CharT*>
    explicit SString8(const StringViewLike& t, size_type pos, size_type n) // test - String8TestConstructorStringViewLikePosN
    {
        std::string_view str(t);
        const auto count = SString8::check_count(str, pos, n);
        SString8 tempstr(str.data() + pos, str.data() + pos + count);
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

    const CharT* data() const noexcept; // test - SString8TestData
    CharT* data() noexcept; // test - SString8TestData
    size_type size() const noexcept; // test - SString8TestSizeLength
    size_type length() const noexcept; // test - SString8TestSizeLength
    size_type capacity() const noexcept;

    friend auto operator<=>(const SString8& lhs, const SString8& rhs) noexcept // test - SString8TestSpaceshipEqEq
    {
        const auto thisdata = std::string_view(lhs.data(), lhs.size());
        const auto thatdata = std::string_view(rhs.data(), rhs.size());
        const auto cmp = thisdata <=> thatdata;
        return cmp;
    }
    friend bool operator==(const SString8& lhs, const SString8& rhs) noexcept // test - SString8TestSpaceshipEqEq
    {
        const auto thislen = lhs.size();

        if (thislen != rhs.size())
            return false;

        return 0 == strncmp(lhs.data(), rhs.data(), thislen);
    }

    friend std::ostream& operator<<(std::ostream& os, const SString8& str)
    {
        return os << str.data();
    }

    void reserve(size_type new_cap = 0); //SString8Testreserve

    SString8& operator=(const CharT* s);
    SString8& operator=(CharT ch);
    SString8& operator=(std::initializer_list<CharT> ilist);
#if __cplusplus >= 202002L
    template<class StringViewLike>
    SString8& operator=(const StringViewLike& t)
    {
        std::string_view str(t);
        SString8 tempstr(str);
        return this->operator=(std::move(tempstr));
    }
#endif
    SString8& operator=(std::nullptr_t) = delete;

private:
    SString8Detail::SString8Data m_Storage;

    template<typename StringType>
    inline static size_t check_out_of_range(const StringType& other, size_type pos)
    {
        if (pos > other.length())
            throw std::out_of_range("pos > other.length()");
        return 0;
    }
    template<typename StringType>
    inline static size_t check_count(const StringType& other, size_type pos, size_type count)
    {
        check_out_of_range(other, pos);

        if (count == std::string::npos)
            return other.size() - pos;
        if (count + pos > other.length())
            return other.size() - pos;
        return count;
    }


};
