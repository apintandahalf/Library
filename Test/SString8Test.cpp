#include "SString8.h"

#include "PintTest.h"
#include <vector>
#include <cstring>
#include <stdexcept>
#include <string_view>

namespace
{
    template<class T1, class T2>
    void testAreEqual(const T1& lhs, const T2& rhs, int line)
    {
        EXPECT_EQ(lhs.size(), rhs.size()) << line;
        if (lhs.size() != rhs.size()) return;
        const auto pL = lhs.data();
        EXPECT_NE(pL, nullptr) << line;
        const auto pR = rhs.data();
        EXPECT_NE(pR, nullptr) << line;
        if (!pL || !pR) return;
        EXPECT_EQ(0, strcmp(pL, pR)) << line;
    }

    template<class T1, class T2>
    void assertAreEqual(const T1& lhs, const T2& rhs, int line)
    {
        ASSERT_EQ(lhs.size(), rhs.size()) << line;
        const auto pL = lhs.data();
        ASSERT_NE(pL, nullptr) << line;
        const auto pR = rhs.data();
        ASSERT_NE(pR, nullptr) << line;
        ASSERT_EQ(0, strcmp(pL, pR)) << line;
    }

    template<class StringType1, class StringType2>
    void testString8TestConstructorOtherPosThrows()
    {
        const StringType2 str("abcdefghijklmnop");
        int count = 0; // expecting 2
        try
        {
            {
                // this should work
                const auto str2 = StringType1(str, str.length());
                count += 2;
            }
            {
                // this should fail
                const auto str2 = StringType1(str, str.length() + 1);
                --count;
                EXPECT_TRUE(false);
            }
        }
        catch (std::out_of_range&)
        {
            EXPECT_EQ(count, 2);
        }
    }
    template<class StringType1, class StringType2>
    void testString8TestConstructorOtherPosCountThrows()
    {
        const StringType2 str("abcdefghijklmnop");
        int count = 0; // expecting 2
        try
        {
            {
                // this should work
                const auto str2 = StringType1(str, str.length(), 1);
                count += 2;
            }
            {
                // this should fail
                const auto str2 = StringType1(str, str.length() + 1, 1);
                --count;
                EXPECT_TRUE(false);
            }
        }
        catch (std::out_of_range&)
        {
            EXPECT_EQ(count, 2);
        }
    }
    template<class StringType1, class StringType2>
    void testString8TestConstructorStringViewLikePosNThrows()
    {
        const StringType2 str("abcdefghijklmnop");
        int count = 0; // expecting 2
        try
        {
            {
                // this should work
                const auto str2 = StringType1(str, str.length(), 1);
                count += 2;
            }
            {
                // this should fail
                const auto str2 = StringType1(str, str.length() + 1, 1);
                --count;
                EXPECT_TRUE(false);
            }
        }
        catch (std::out_of_range&)
        {
            EXPECT_EQ(count, 2);
        }
    }
    template<class StringType>
    void testSString8TestEmbeddedNulls()
    {
        EXPECT_EQ(StringType("\0").length(), 0);
        EXPECT_EQ(StringType("\0a").length(), 0);
        EXPECT_EQ(StringType("a\0").length(), 1);
        EXPECT_EQ(StringType("\0", 1).length(), 1);
        EXPECT_EQ(StringType("\0a", 2).length(), 2);
        EXPECT_EQ(StringType("a\0", 2).length(), 2);
        EXPECT_EQ(StringType("abcdefgab\0cd").length(), 9);
        EXPECT_EQ(StringType("abcdefgab\0cd", 12).length(), 12);
        {
            using namespace std::string_literals;
            EXPECT_EQ(StringType("ab\0\0cde"s).length(), 7);
        }
    }
}

TEST(String8TestConstructorCountChar)
{
    {
        const auto str1 = SString8(0, 'A');
        const auto str2 = std::string(0, 'B');
        testAreEqual(str1, str2, __LINE__);
    }
    for (size_t i = 1U; i < 7U; ++i)
    {
        const auto str1 = SString8(i, 'B');
        const auto str2 = std::string(i, 'B');
        testAreEqual(str1, str2, __LINE__);
    }
    for (size_t i = 8U; i < 9U; ++i)
    {
        const auto str1 = SString8(i, 'C');
        const auto str2 = std::string(i, 'C');
        testAreEqual(str1, str2, __LINE__);
    }
    for (size_t i = 9U; i < 40U; ++i)
    {
        const auto str1 = SString8(i, 'D');
        const auto str2 = std::string(i, 'D');
        testAreEqual(str1, str2, __LINE__);
    }
}

TEST(String8TestConstructorStringView)
{
    {
        const std::string_view str;
        const auto str1 = SString8(str);
        const auto str2 = std::string(str);
        testAreEqual(str1, str2, __LINE__);
    }
    {
        const std::string_view str("abcd");
        const auto str1 = SString8(str);
        const auto str2 = std::string(str);
        testAreEqual(str1, str2, __LINE__);
    }
    {
        const std::string_view str("abcdefghijklmnop");
        const auto str1 = SString8(str);
        const auto str2 = std::string(str);
        testAreEqual(str1, str2, __LINE__);
        auto pStr1 = str1.data();
        auto pStr2 = str2.data();
        EXPECT_EQ(0, strcmp(pStr1, pStr2));
    }
}

TEST(String8TestConstructorString)
{
    {
        const std::string str;
        const auto str1 = SString8(str);
        const auto str2 = std::string(str);
        testAreEqual(str1, str2, __LINE__);
    }
    {
        const std::string str("abcd");
        const auto str1 = SString8(str);
        const auto str2 = std::string(str);
        testAreEqual(str1, str2, __LINE__);
    }
    {
        const std::string str("abcdefghijklmnop");
        const auto str1 = SString8(str);
        const auto str2 = std::string(str);
        testAreEqual(str1, str2, __LINE__);
    }
}

TEST(String8Testoperatotstdstringview)
{
    {
        const std::string_view str;
        const auto str1 = SString8(str);
        const auto str2 = std::string(str);
        EXPECT_EQ(str2, str1.operator std::string_view());
        EXPECT_EQ(str1.operator std::string_view(), str2);
    }
    {
        const std::string_view str("abcd");
        const auto str1 = SString8(str);
        const auto str2 = std::string(str);
        EXPECT_EQ(str1.operator std::string_view(), str2);
    }
    {
        const std::string_view str("abcdefghijklmnop");
        const auto str1 = SString8(str);
        const auto str2 = std::string(str);
        EXPECT_EQ(str1.operator std::string_view(), str2);
    }
}

TEST(String8TestConstructorOtherPos)
{
    testString8TestConstructorOtherPosThrows<std::string, std::string>();
    testString8TestConstructorOtherPosThrows<SString8, std::string>();
    testString8TestConstructorOtherPosThrows<SString8, SString8>();

    const std::string stdstr(std::string_view("abcdefghijklmnop"));

    const SString8 sstr(stdstr);
    assertAreEqual(sstr, stdstr, __LINE__);

    for (size_t pos = 0; pos < stdstr.length(); ++pos)
    {
        const auto str1 = SString8(sstr, pos);
        const auto str2 = SString8(stdstr, pos);
        const auto str3 = std::string(stdstr, pos);
        testAreEqual(str1, str2, __LINE__);
        testAreEqual(str1, str3, __LINE__);
    }
}

TEST(String8TestConstructorOtherPosCount)
{
    testString8TestConstructorOtherPosCountThrows<std::string, std::string>();
    testString8TestConstructorOtherPosCountThrows<SString8, std::string>();
    testString8TestConstructorOtherPosCountThrows<SString8, SString8>();

    const std::string stdstr(std::string_view("abcdefghijklmnop"));
    const SString8 sstr(stdstr);
    assertAreEqual(sstr, stdstr, __LINE__);

    for (size_t pos = 0; pos < stdstr.length(); ++pos)
    {
        for (size_t c = 0; c < stdstr.length(); ++c)
        {
            const auto str1 = SString8(sstr, pos, c);
            const auto str2 = SString8(stdstr, pos, c);
            const auto str3 = std::string(stdstr, pos, c);
            testAreEqual(str1, str2, __LINE__);
            testAreEqual(str1, str3, __LINE__);
        }
    }
}

TEST(String8TestConstructorCharStarCount)
{
    //SString8(const CharT* s, size_type count)
    const std::string stdstr(std::string_view("abcdefghijklmnop"));
    const SString8 sstr(stdstr);
    assertAreEqual(sstr, stdstr, __LINE__);

    const auto pStdData = stdstr.data();
    for (size_t c = 0; c < stdstr.length(); ++c)
    {
        const auto str1 = SString8(pStdData, c);
        const auto str2 = std::string(pStdData, c);
        testAreEqual(str1, str2, __LINE__);
    }
}

TEST(String8TestConstructorCharStar)
{
    //SString8(const CharT* s)
    const std::string stdstr(std::string_view("abcdefghijklmnop"));
    const SString8 sstr(stdstr);
    assertAreEqual(sstr, stdstr, __LINE__);

    for (size_t c = 0; c < stdstr.length(); ++c)
    {
        const auto str0 = std::string(stdstr.data(), c);
        const auto pStdData = str0.data();
        const auto str1 = SString8(pStdData);
        const auto str2 = std::string(pStdData);
        testAreEqual(str1, str2, __LINE__);
    }
}

TEST(String8TestConstructorInputItFirstLast)
{
    char mutable_c_str[] = "abcdefghijklmnop";
    const std::string str1(std::begin(mutable_c_str) + 8, std::end(mutable_c_str) - 1);
    const SString8 str2(std::begin(mutable_c_str) + 8, std::end(mutable_c_str) - 1);
    testAreEqual(str1, str2, __LINE__);
}

TEST(String8TestConstructorInitialiserList)
{
    const std::initializer_list<char> ilist = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm' };
    const std::string str1(ilist);
    const SString8 str2(ilist);
    testAreEqual(str1, str2, __LINE__);
}


#if __cplusplus >= 202002L

    struct StringViewLike
    {
        std::string m_Str;
        StringViewLike(std::string str) : m_Str(str) {}
        StringViewLike(const char* str) : m_Str(str) {}
        operator std::string_view() const { return m_Str.operator std::basic_string_view<char, std::char_traits<char>>(); }
        operator const char* () const = delete;
        size_t length() const { return m_Str.length(); }
        size_t size() const { return m_Str.size(); }
    };

    TEST(String8TestConstructorStringViewLike)
    {
        // can throw
        const StringViewLike str("abcdefghijklmnop");
        const std::string str1(str);
        const SString8 str2(str);
        testAreEqual(str1, str2, __LINE__);
        auto pStr1 = str1.data();
        auto pStr2 = str2.data();
        EXPECT_EQ(0, strcmp(pStr1, pStr2));
    }

    TEST(String8TestConstructorStringViewLikePosN)
    {
        testString8TestConstructorStringViewLikePosNThrows<std::string, StringViewLike>();
        testString8TestConstructorStringViewLikePosNThrows<SString8, StringViewLike>();

        const StringViewLike svl("abcdefghijklmnop");

        for (size_t pos = 0; pos < svl.length(); ++pos)
        {
            for (size_t c = 0; c < svl.length(); ++c)
            {
                const auto str1 = SString8(svl, pos, c);
                const auto str2 = std::string(svl, pos, c);
                testAreEqual(str1, str2, __LINE__);
            }
        }
    }

#endif


TEST(String8TestCppReferenceConstructorTests)
{
    {
        std::string s1;
        SString8 s2;
        testAreEqual(s1, s2, __LINE__);
    }
    {
        std::string s1(4, '=');
        SString8 s2(4, '=');
        testAreEqual(s1, s2, __LINE__);
    }
    {
        std::string const other3("Exemplary");
        std::string s1(other3, 0, other3.length() - 1);
        SString8 s2(other3, 0, other3.length() - 1);
        testAreEqual(s1, s2, __LINE__);
    }
    {
        std::string const other4("Mutatis Mutandis");
        std::string s1(other4, 8);
        SString8 s2(other4, 8);
        testAreEqual(s1, s2, __LINE__);
    }
    {
        std::string s1("C-style string", 7);
        SString8 s2("C-style string", 7);
        testAreEqual(s1, s2, __LINE__);
    }
    {
        std::string s1("C-style\0string");
        SString8 s2("C-style\0string");
        testAreEqual(s1, s2, __LINE__);
    }
    {
        char mutable_c_str[] = "another C-style string";
        std::string s1(std::begin(mutable_c_str) + 8, std::end(mutable_c_str) - 1);
        SString8 s2(std::begin(mutable_c_str) + 8, std::end(mutable_c_str) - 1);
        testAreEqual(s1, s2, __LINE__);
    }
    {
        std::string const other8("Exemplar");
        std::string s1(other8);
        SString8 s2(other8);
        testAreEqual(s1, s2, __LINE__);
    }
    {
        std::string s1(std::string("C++ by ") + std::string("example"));
        SString8 s2(std::string("C++ by ") + std::string("example"));
        testAreEqual(s1, s2, __LINE__);
    }
    {
        std::string s1({ 'C', '-', 's', 't', 'y', 'l', 'e' });
        SString8 s2({ 'C', '-', 's', 't', 'y', 'l', 'e' }); // @TODO - check this - currently going via std::string
        testAreEqual(s1, s2, __LINE__);
    }
    {
        std::string s1(3, static_cast<char>(std::toupper('a')));
        SString8 s2(3, static_cast<char>(std::toupper('a')));
        testAreEqual(s1, s2, __LINE__);
    }
}

TEST(SString8TestSwap)
{
    const auto strs = { "", "1", "1234567", "1234568" };
    for (const std::string_view str1 : strs)
    {
        for (const std::string_view str2 : strs)
        {
            SString8 s1(str1);
            SString8 s2(str2);
            EXPECT_EQ(strcmp(s1.data(), str1.data()), 0);
            EXPECT_EQ(strcmp(s2.data(), str2.data()), 0);
            s1.swap(s2);
            EXPECT_EQ(strcmp(s1.data(), str2.data()), 0);
            EXPECT_EQ(strcmp(s2.data(), str1.data()), 0);
        }
    }
    for (const std::string_view str1 : strs)
    {
        for (const std::string_view str2 : strs)
        {
            SString8 s1(str1);
            SString8 s2(str2);
            EXPECT_EQ(strcmp(s1.data(), str1.data()), 0);
            EXPECT_EQ(strcmp(s2.data(), str2.data()), 0);
            swap(s1, s2);
            EXPECT_EQ(strcmp(s1.data(), str2.data()), 0);
            EXPECT_EQ(strcmp(s2.data(), str1.data()), 0);
        }
    }
}

TEST(SString8TestConstructorCopy)
{
    const std::string_view shorttext = "abcdefg";
    const std::string_view longtext = "abcdefghijklmnopqrstuvwxyz";

    for (const auto& txt : { longtext })
    {
        for (auto i = 0U; i < txt.size(); ++i)
        {
            std::string_view sub(txt.data(), txt.data() + i);
            SString8 str1(sub);
            SString8 str2(str1);
            auto pStr1 = str1.data();
            auto pStr2 = str2.data();
            EXPECT_EQ(0, strcmp(pStr1, pStr2));
        }
    }
}

TEST(SString8TestAssignment)
{
    const std::string_view shorttext = "abcdefg";
    const std::string_view longtext = "abcdefghijklmnopqrstuvwxyz";

    for (const auto& txt : { shorttext })
    {
        for (auto i = 0U; i < txt.size(); ++i)
        {
            std::string_view sub(txt.data(), txt.data() + i);
            std::string str(sub);
            SString8 str1(sub);
            SString8 str2(longtext);
            str2 = str1;
            auto pStr0 = str.data();
            auto pStr1 = str1.data();
            auto pStr2 = str2.data();
            EXPECT_EQ(0, strcmp(pStr0, pStr2));
            EXPECT_EQ(0, strcmp(pStr1, pStr2));
        }
    }
    for (const auto& txt : { longtext })
    {
        for (auto i = shorttext.size(); i < txt.size(); ++i)
        {
            std::string_view sub(txt.data(), txt.data() + i);
            std::string str(sub);
            SString8 str1(sub);
            SString8 str2(longtext);
            str2 = str1;
            auto pStr0 = str.data();
            auto pStr1 = str1.data();
            auto pStr2 = str2.data();
            EXPECT_EQ(0, strcmp(pStr0, pStr2));
            EXPECT_EQ(0, strcmp(pStr1, pStr2));
        }
    }
}

TEST(SString8TestConstructorMove)
{
    const std::string_view shorttext = "abcdefg";
    const std::string_view longtext = "abcdefghijklmnopqrstuvwxyz";

    for (const auto& txt : { shorttext })
    {
        for (auto i = 0U; i < txt.size(); ++i)
        {
            std::string_view sub(txt.data(), txt.data() + i);
            std::string str(sub);
            SString8 str1(sub);
            SString8 str2(std::move(str1));
            auto pStr1 = str.data();
            auto pStr2 = str2.data();
            EXPECT_EQ(0, strcmp(pStr1, pStr2));
        }
    }
    for (const auto& txt : { longtext })
    {
        for (auto i = shorttext.size(); i < txt.size(); ++i)
        {
            std::string_view sub(txt.data(), txt.data() + i);
            std::string str(sub);
            SString8 str1(sub);
            SString8 str2(std::move(str1));
            auto pStr1 = str.data();
            auto pStr2 = str2.data();
            EXPECT_EQ(0, strcmp(pStr1, pStr2));
        }
    }
}

TEST(SString8TestAssignmentMove)
{
    const std::string_view shorttext = "abcdefg";
    const std::string_view longtext = "abcdefghijklmnopqrstuvwxyz";

    for (const auto& txt : { shorttext })
    {
        for (auto i = 0U; i < txt.size(); ++i)
        {
            std::string_view sub(txt.data(), txt.data() + i);
            std::string str(sub);
            SString8 str1(sub);
            SString8 str2(longtext);
            str2 = std::move(str1);
            auto pStr1 = str.data();
            auto pStr2 = str2.data();
            EXPECT_EQ(0, strcmp(pStr1, pStr2));
        }
    }
    for (const auto& txt : { longtext })
    {
        for (auto i = shorttext.size(); i < txt.size(); ++i)
        {
            std::string_view sub(txt.data(), txt.data() + i);
            std::string str(sub);
            SString8 str1(sub);
            SString8 str2(longtext);
            str2 = std::move(str1);
            auto pStr1 = str.data();
            auto pStr2 = str2.data();
            EXPECT_EQ(0, strcmp(pStr1, pStr2));
        }
    }
}

TEST(SString8TestSpaceshipEqEq)
{
    auto test = [](const std::string& str1, const std::string& str2)
        {
            const SString8 str81(str1);
            const SString8 str82(str2);

            auto test2 = [](const std::string& s1, const std::string& s2, const SString8& s81, const SString8& s82)
                {
                    EXPECT_EQ(s1 == s2, s81 == s82) << s1 << " " << s2 << " " << s81 << " " << s82;
                    EXPECT_EQ(s1 != s2, s81 != s82) << s1 << " " << s2 << " " << s81 << " " << s82;
                    EXPECT_EQ(s1 < s2, s81 < s82) << s1 << " " << s2 << " " << s81 << " " << s82;
                    EXPECT_EQ(s1 <= s2, s81 <= s82) << s1 << " " << s2 << " " << s81 << " " << s82;
                    EXPECT_EQ(s1 > s2, s81 > s82) << s1 << " " << s2 << " " << s81 << " " << s82;
                    EXPECT_EQ(s1 >= s2, s81 >= s82) << s1 << " " << s2 << " " << s81 << " " << s82;
                };
            test2(str1, str2, str81, str82);
            test2(str2, str1, str82, str81);
        };
    {
        {
            test(std::string(), std::string());
            test(std::string(), std::string(1, 'a'));
            test(std::string(1, 'a'), std::string());
            test(std::string(1, 'a'), std::string(1, 'a'));
            test(std::string(1, 'a'), std::string(2, 'a'));
            test(std::string(2, 'a'), std::string(1, 'a'));
            test(std::string(1, 'b'), std::string(2, 'a'));
            test(std::string(1, 'b'), std::string(2, 'c'));
        }
        size_t len1 = 0;
        size_t len2 = 0;
        while (len1++ < 34)
        {
            const std::string str1(len1, 'b');

            while (len2++ < 34)
            {
                test(str1, std::string(len2, 'a'));
                test(str1, std::string(len2, 'c'));
            }
        }
    }
}

TEST(SString8TestData)
{
    for (size_t len1 = 0; len1 < 34; ++len1)
    {
        const std::string str1(len1, 'a');
        const SString8 str81(str1);
        EXPECT_EQ(0, strcmp(str1.data(), str81.data()));
        EXPECT_EQ(0, str1.compare(str81.data()));
        EXPECT_EQ(0, str1.compare(const_cast<SString8&>(str81).data()));
    }
}

TEST(SString8TestSizeLength)
{
    for (size_t len1 = 0; len1 < 34; ++len1)
    {
        const std::string str1(len1, 'a');
        const SString8 str81(str1);
        EXPECT_EQ(len1, str81.size());
        EXPECT_EQ(len1, str81.length());
        if (len1 <= 7)
            EXPECT_EQ(7, str81.capacity()) << len1 << " " << str81.capacity();
        else
            EXPECT_TRUE(len1 == str81.capacity() || len1 + 1 == str81.capacity()) << len1 << " " << str81.capacity();
    }
}


TEST(SString8TestEmbeddedNulls)
{
    testSString8TestEmbeddedNulls<std::string>();
    testSString8TestEmbeddedNulls<SString8>();
}

TEST(SString8Testreserve)
{
    const auto sizes = { 0ULL, 7Ull, 8ULL, 9ULL, 253Ull , 254Ull, 255Ull , 256Ull, ((1ULL << 15U) - 1U), (1ULL << 15U), ((1ULL << 15U) + 1U) };
    for (const auto sz : sizes)
    {
        SString8 data;
        data.reserve(sz);
        EXPECT_EQ(0, data.size()) << sz;
        const auto cap = data.capacity();
        if (sz <= 7)
            EXPECT_EQ(7, cap) << sz;
        else
            EXPECT_TRUE(sz == cap || sz + 1 == cap) << sz << " " << cap;
    }

    for (const auto sz1 : sizes)
    {
        for (const auto sz2 : sizes)
        {
            const std::string data(sz1, 'a');
            const SString8 data1(data);
            SString8 data2(data1);
            data2.reserve(sz2);
            auto p1 = data1.data();
            auto p2 = data2.data();
            EXPECT_EQ(0, strcmp(p1, p2)) << sz1 << " " << sz2;
            const auto cap1 = data1.capacity();
            const auto cap2 = data2.capacity();
            const auto szz1 = data1.size();
            const auto szz2 = data2.size();
            EXPECT_EQ(szz2, szz1) << sz1 << " " << sz2;
            EXPECT_GE(cap2, cap1) << sz1 << " " << sz2;
            EXPECT_GE(cap2, sz1) << sz1 << " " << sz2;
            EXPECT_GE(cap2, sz2) << sz1 << " " << sz2;
        }
    }
}

TEST(SString8OperatorEqCharStar)
{
    std::vector<std::string> strs{ "", "1", "1234567", "12345678" };
    strs.reserve(strs.size() + 16);

    for (const auto shft : { 7U, 8U, 15U, 16U })
    {
        for (auto i = 0; i < 4; ++i)
        {
            auto tot_len = (1ULL << shft) + static_cast<unsigned>(i) - 2U;
            auto str1 = std::string("1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ");
            auto str2 = std::string(tot_len - str1.size(), 'a');
            strs.emplace_back(std::move(str1 + str2));
        }
    }

    for (const auto& str1 : strs)
    {
        const auto sz1 = str1.size();
        for (const auto& str2 : strs)
        {
            SString8 data(str1);
            EXPECT_EQ(data.size(), str1.size()) << sz1;
            data = str2.data();
            EXPECT_EQ(data.size(), str2.size()) << sz1 << " " << str2.size();
        }
    }
}

TEST(SString8OperatorEqInitializerList)
{
    std::vector<std::initializer_list<char>> strs{ {}, { '1' }, {'1','2','3','4','5','6','7'} , {'1','2','3','4','5','6','7','8'} };

    for (const auto& str1 : strs)
    {
        const auto sz1 = str1.size();
        for (const auto& str2 : strs)
        {
            SString8 data(str1);
            EXPECT_EQ(data.size(), str1.size()) << sz1;
            data = str2;
            EXPECT_EQ(data.size(), str2.size()) << sz1 << " " << str2.size();
        }
    }
}

TEST(String8TestOperatorEqStringViewLike)
{
    std::vector<StringViewLike> strs{ "", "1", "1234567", "12345678" };

    strs.reserve(strs.size() + 16);

    for (const auto shft : { 7U, 8U, 15U, 16U })
    {
        for (auto i = 0; i < 4; ++i)
        {
            auto tot_len = (1ULL << shft) + static_cast<unsigned>(i) - 2U;
            auto str1 = std::string("1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ");
            auto str2 = std::string(tot_len - str1.size(), 'a');
            strs.emplace_back(std::move(str1 + str2));
        }
    }

    for (const auto& str1 : strs)
    {
        const auto sz1 = str1.size();
        for (const auto& str2 : strs)
        {
            SString8 data(str1);
            EXPECT_EQ(data.size(), str1.size()) << sz1;
            data = str2;
            EXPECT_EQ(data.size(), str2.size()) << sz1 << " " << str2.size();
        }
    }
}
