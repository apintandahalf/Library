#include "SString8.h"

#include "PintTest.h"

#include <type_traits>
#include <cstdint>
#include <cstring>

using namespace SString8Detail;

TEST(SString8DataTestData)
{
    {
        SString8Data data;
        EXPECT_TRUE(data.isBuffer());
        EXPECT_FALSE(data.isPtr());
        EXPECT_FALSE(data.isSmall());
        EXPECT_FALSE(data.isMedium());
        EXPECT_FALSE(data.isLarge());
        EXPECT_EQ(data.size(), 0);
        EXPECT_EQ(data.capacity(), 7);
    }

    std::vector<std::string> strs{ "", "1", "1234567", "12345678" };
    strs.reserve(strs.size() + 16);

    for (const auto shft : {7U, 8U, 15U, 16U})
    {
        for (auto i = 0; i < 4; ++i)
        {
            auto tot_len = (1ULL << shft) + static_cast<unsigned>(i) - 2U;
            auto str1 = std::string("1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ");
            auto str2 = std::string(tot_len - str1.size(), 'a');
            strs.emplace_back(std::move(str1 + str2));
        }
    }

    for (const std::string_view str : strs)
    {
        SString8Data data(str);
        {
            const auto len = str.length();
            const auto b = data.isBuffer();
            EXPECT_TRUE(len > 7 || b) << str.size();
        }
        {
            const auto len1 = data.size();
            const auto len2 = str.size();
            EXPECT_EQ(len1, len2) << str.size();
        }
        EXPECT_EQ(strcmp(data.data(), str.data()), 0) << str.size();
        const SString8Data& data2 = data;
        EXPECT_EQ(strcmp(data2.data(), str.data()), 0) << str.size();
        if (data.isBuffer())
        {
            auto p = data2.data();
            EXPECT_EQ(p, data.m_Storage.m_Buffer.m_Buffer) << str.size();
            EXPECT_EQ(data.capacity(), 7);
        }
        else
        {
            auto p = data2.data();
            EXPECT_NE(reinterpret_cast<uintptr_t>(p), data.m_Storage.m_pLargeStr) << str.size();
            if (data.getStorageType() >= SString8Data::StorageType::MEDIUM)
            {
                EXPECT_GT(p, data.getAsPtr()) << str.size();
                EXPECT_GT(p, data2.getAsPtr()) << str.size();
            }
            else
            {
                EXPECT_EQ(p, data.getAsPtr()) << str.size();
                EXPECT_EQ(p, data2.getAsPtr()) << str.size();
            }
            EXPECT_TRUE(data.getStorageType() >= SString8Data::StorageType::SMALL);
            if (data.getStorageType() == SString8Data::StorageType::SMALL)
            {
                if (str.size() % 2 == 0)
                    EXPECT_EQ(data2.capacity(), data2.size()) << str.size();
                else
                    EXPECT_EQ(data2.capacity(), data2.size() + 1) << str.size();
            }
            else
            {
                EXPECT_EQ(data2.capacity(), data2.size()) << str.size();
            }
        }
        EXPECT_TRUE(true);
    }

    for (const std::string_view str1 : strs)
    {
        for (const std::string_view str2 : strs)
        {
            SString8Data data(str1);
            data = str2;
            EXPECT_EQ(data.size(), str2.size());
        }
    }

    for (const auto sz : { 0ULL, 7Ull })
    {
        const std::string str(sz, 'a');
        SString8Data data(str.data());
        EXPECT_TRUE(data.isBuffer()) << sz;
        EXPECT_FALSE(data.isPtr()) << sz;
        EXPECT_FALSE(data.isSmall()) << sz;
        EXPECT_FALSE(data.isMedium()) << sz;
        EXPECT_FALSE(data.isLarge()) << sz;
        EXPECT_EQ(data.size(), sz) << sz;
        EXPECT_EQ(data.capacity(), 7);
    }
    for (const auto sz : { 8ULL, 9ULL, 253Ull , 254Ull })
    {
        const std::string str(sz, 'a');
        SString8Data data(str.data());
        EXPECT_FALSE(data.isBuffer()) << sz;
        EXPECT_TRUE(data.isPtr()) << sz;
        EXPECT_TRUE(data.isSmall()) << sz;
        EXPECT_FALSE(data.isMedium()) << sz;
        EXPECT_FALSE(data.isLarge()) << sz;
        EXPECT_EQ(sz, data.size()) << sz;
        EXPECT_TRUE(data.capacity() == sz || data.capacity() == sz+1) << data.capacity() << " " << sz;
    }
    for (const auto sz : { 255Ull, ((1ULL << 15U) - 1U) })
    {
        const std::string str(sz, 'a');
        SString8Data data(str.data());
        EXPECT_FALSE(data.isBuffer()) << sz;
        EXPECT_TRUE(data.isPtr()) << sz;
        EXPECT_FALSE(data.isSmall()) << sz;
        EXPECT_TRUE(data.isMedium()) << sz;
        EXPECT_FALSE(data.isLarge()) << sz;
        EXPECT_EQ(sz, data.size()) << sz;
        EXPECT_EQ(data.capacity(), sz);
    }
    for (const auto sz : { (1ULL << 15U) })
    {
        const std::string str(sz, 'a');
        SString8Data data(str.data());
        EXPECT_FALSE(data.isBuffer()) << sz;
        EXPECT_TRUE(data.isPtr()) << sz;
        EXPECT_FALSE(data.isSmall()) << sz;
        EXPECT_FALSE(data.isMedium()) << sz;
        EXPECT_TRUE(data.isLarge()) << sz;
        EXPECT_EQ(sz, data.size()) << sz;
        EXPECT_EQ(data.capacity(), sz);
    }

    for (const std::string_view str1 : strs)
    {
        for (const std::string_view str2 : strs)
        {
            SString8Data data1(str1);
            SString8Data data2(str2);
            EXPECT_EQ(strcmp(data1.data(), str1.data()), 0) << str1 << str2;
            EXPECT_EQ(strcmp(data2.data(), str2.data()), 0) << str1 << str2;
            data1.swap(data2);
            EXPECT_EQ(strcmp(data1.data(), str2.data()), 0) << str1 << str2;
            EXPECT_EQ(strcmp(data2.data(), str1.data()), 0) << str1 << str2;
        }
    }
}

TEST(SString8DataTestgetSizeAndCap)
{
    for (const auto sz : { 0ULL, 7Ull, 8ULL, 9ULL, 253Ull , 254Ull, 255Ull , 256Ull, ((1ULL << 15U) - 1U), (1ULL << 15U), ((1ULL << 15U) + 1U) })
    {
        const std::string str(sz, 'a');
        const SString8Data data(str.data());
        const auto& [szz, cap] = data.getSizeAndCap();
        EXPECT_EQ(szz, data.size()) << sz;
        EXPECT_TRUE(cap == data.capacity()) << sz;
    }
}

TEST(SString8DataTestgetDataAndSize)
{
    for (const auto sz : { 0ULL, 7Ull, 8ULL, 9ULL, 253Ull , 254Ull, 255Ull , 256Ull, ((1ULL << 15U) - 1U), (1ULL << 15U), ((1ULL << 15U) + 1U) })
    {
        const std::string str(sz, 'a');
        {
            SString8Data data(str.data());
            const auto& [ptr, szz] = data.getDataAndSize();
            EXPECT_EQ(ptr, data.data()) << sz;
            EXPECT_EQ(szz, data.size()) << sz;
        }
        {
            const SString8Data data(str.data());
            const auto& [ptr, szz] = data.getDataAndSize();
            EXPECT_EQ(ptr, data.data()) << sz;
            EXPECT_EQ(szz, data.size()) << sz;
        }
    }
}

TEST(SString8DataTestreserve)
{
    const auto sizes = { 0ULL, 7Ull, 8ULL, 9ULL, 253Ull , 254Ull, 255Ull , 256Ull, ((1ULL << 15U) - 1U), (1ULL << 15U), ((1ULL << 15U) + 1U) };
    for (const auto sz : sizes)
    {
        SString8Data data;
        data.reserve(sz);
        const auto& [szz, cap] = data.getSizeAndCap();
        EXPECT_EQ(0, szz) << sz;
        if (sz <= 7)
            EXPECT_EQ(7, cap) << sz;
        else if (data.isSmall())
            EXPECT_TRUE(sz == cap || sz+1 == cap) << sz << " " << cap;
        else
            EXPECT_EQ(sz, cap) << sz;
    }

    for (const auto sz1 : sizes)
    {
        for (const auto sz2 : sizes)
        {
            const std::string data(sz1, 'a');
            const SString8Data data1(data);
            SString8Data data2(data1);
            data2.reserve(sz2);
            auto p1 = data1.data();
            auto p2 = data2.data();
            EXPECT_EQ(0, strcmp(p1, p2)) << sz1 << " " << sz2;
            const auto& [szz1, cap1] = data1.getSizeAndCap();
            const auto& [szz2, cap2] = data2.getSizeAndCap();
            EXPECT_EQ(szz2, szz1) << sz1 << " " << sz2;
            EXPECT_GE(cap2, cap1) << sz1 << " " << sz2;
            EXPECT_GE(cap2, sz1) << sz1 << " " << sz2;
            EXPECT_GE(cap2, sz2) << sz1 << " " << sz2;
        }
    }
}

namespace
{
    std::string_view shorttext = "abcdefg";
    std::string_view longtext = "abcdefghijklmnopqrstuvwxyz";
}
TEST(SString8DataTestConstructorStringView)
{
    const auto& txt = longtext;
    {
        for (auto i = 0U; i < txt.size(); ++i)
        {
            std::string_view sub(txt.data(), txt.data() + i);
            std::string substr(sub);
            SString8Data str(sub);
            auto pStr1 = substr.data();
            auto pStr2 = str.data();
            EXPECT_EQ(0, strcmp(pStr1, pStr2));
        }
    }
}

TEST(SString8DataTestConstructorCopy)
{
    const auto& txt = longtext;
    {
        for (auto i = 0U; i < txt.size(); ++i)
        {
            std::string_view sub(txt.data(), txt.data() + i);
            SString8Data str1(sub);
            SString8Data str2(str1);
            auto pStr1 = str1.data();
            auto pStr2 = str2.data();
            EXPECT_EQ(0, strcmp(pStr1, pStr2));
        }
    }
}

TEST(SString8DataTestConstructorMove)
{
    {
        const auto& txt = shorttext;
        for (auto i = 0U; i < txt.size(); ++i)
        {
            std::string_view sub(txt.data(), txt.data() + i);
            std::string str(sub);
            SString8Data str1(sub);
            SString8Data str2(std::move(str1));
            auto pStr1 = str.data();
            auto pStr2 = str2.data();
            EXPECT_EQ(0, strcmp(pStr1, pStr2));
        }
    }
    {
        const auto& txt = longtext;
        for (auto i = shorttext.size(); i < txt.size(); ++i)
        {
            std::string_view sub(txt.data(), txt.data() + i);
            std::string str(sub);
            SString8Data str1(sub);
            SString8Data str2(std::move(str1));
            auto pStr1 = str.data();
            auto pStr2 = str2.data();
            EXPECT_EQ(0, strcmp(pStr1, pStr2));
        }
    }
}

TEST(SString8DataTestAssignement)
{
    {
        const auto& txt = shorttext;
        for (auto i = 0U; i < txt.size(); ++i)
        {
            std::string_view sub(txt.data(), txt.data() + i);
            std::string str(sub);
            SString8Data str1(sub);
            SString8Data str2(longtext);
            str2 = str1;
            auto pStr0 = str.data();
            auto pStr1 = str1.data();
            auto pStr2 = str2.data();
            EXPECT_EQ(0, strcmp(pStr0, pStr2));
            EXPECT_EQ(0, strcmp(pStr1, pStr2));
        }
    }
    {
        const auto& txt = longtext;
        for (auto i = shorttext.size(); i < txt.size(); ++i)
        {
            std::string_view sub(txt.data(), txt.data() + i);
            std::string str(sub);
            SString8Data str1(sub);
            SString8Data str2(longtext);
            str2 = str1;
            auto pStr0 = str.data();
            auto pStr1 = str1.data();
            auto pStr2 = str2.data();
            EXPECT_EQ(0, strcmp(pStr0, pStr2));
            EXPECT_EQ(0, strcmp(pStr1, pStr2));
        }
    }
}

TEST(SString8DataTestAssignementMove)
{
    {
        const auto& txt = shorttext;
        for (auto i = 0U; i < txt.size(); ++i)
        {
            std::string_view sub(txt.data(), txt.data() + i);
            std::string str(sub);
            SString8Data str1(sub);
            SString8Data str2(longtext);
            str2 = std::move(str1);
            auto pStr1 = str.data();
            auto pStr2 = str2.data();
            EXPECT_EQ(0, strcmp(pStr1, pStr2));
        }
    }
    {
        const auto& txt = longtext;
        for (auto i = shorttext.size(); i < txt.size(); ++i)
        {
            std::string_view sub(txt.data(), txt.data() + i);
            std::string str(sub);
            SString8Data str1(sub);
            SString8Data str2(longtext);
            str2 = std::move(str1);
            auto pStr1 = str.data();
            auto pStr2 = str2.data();
            EXPECT_EQ(0, strcmp(pStr1, pStr2));
        }
    }
}

TEST(SString8DataTestGetSetPtr)
{
    {
        SString8Data data;
        EXPECT_TRUE(data.isBuffer());

        const auto shortStr = "1234567";
        strcpy(data.m_Storage.m_Buffer.m_Buffer, shortStr);
        EXPECT_EQ(0, strcmp(data.m_Storage.m_Buffer.m_Buffer, shortStr));
    }
}
