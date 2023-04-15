#include "SString8.h"
#include "SString8Test.h"

#include "UnitTest.h"

namespace
{
#define testAreEqual(lhs, rhs) \
	do { \
		EXPECT_EQ(lhs.size(), rhs.size()); \
		if (lhs.size() != rhs.size()) continue; \
		const auto pL = lhs.data(); \
		EXPECT_NE(pL, nullptr); \
		const auto pR = rhs.data(); \
		EXPECT_NE(pR, nullptr); \
		if (!pL || !pR) continue; \
		EXPECT_EQ(0, strcmp(pL, pR)); \
	} while (false)

#define assertAreEqual(lhs, rhs) \
	do { \
		ASSERT_EQ(lhs.size(), rhs.size()); \
		const auto pL = lhs.data(); \
		ASSERT_NE(pL, nullptr); \
		const auto pR = rhs.data(); \
		ASSERT_NE(pR, nullptr); \
		ASSERT_EQ(0, strcmp(pL, pR)); \
	} while (false)

}

TEST(String8TestDefaultConstructor)
{
	SString8 str1;
	std::string str2;
	testAreEqual(str1, str2);
}

TEST(String8TestConstructorCountChar)
{
	{
		const auto str1 = SString8(0, 'A');
		const auto str2 = std::string(0, 'B');
		testAreEqual(str1, str2);
	}
	for (size_t i = 1U; i < 7U; ++i)
	{
		const auto str1 = SString8(i, 'B');
		const auto str2 = std::string(i, 'B');
		testAreEqual(str1, str2);
	}
	for (size_t i = 8U; i < 9U; ++i)
	{
		const auto str1 = SString8(i, 'C');
		const auto str2 = std::string(i, 'C');
		testAreEqual(str1, str2);
	}
	for (size_t i = 9U; i < 40U; ++i)
	{
		const auto str1 = SString8(i, 'D');
		const auto str2 = std::string(i, 'D');
		testAreEqual(str1, str2);
	}
}

TEST(String8TestConstructorStringView)
{
	{
		const std::string_view str;
		const auto str1 = SString8(str);
		const auto str2 = std::string(str);
		testAreEqual(str1, str2);
	}
	{
		const std::string_view str("abcd");
		const auto str1 = SString8(str);
		const auto str2 = std::string(str);
		testAreEqual(str1, str2);
	}
	{
		const std::string_view str("abcdefghijklmnop");
		const auto str1 = SString8(str);
		const auto str2 = std::string(str);
		testAreEqual(str1, str2);
	}
}

TEST(String8TestConstructorString)
{
	{
		const std::string str;
		const auto str1 = SString8(str);
		const auto str2 = std::string(str);
		testAreEqual(str1, str2);
	}
	{
		const std::string str("abcd");
		const auto str1 = SString8(str);
		const auto str2 = std::string(str);
		testAreEqual(str1, str2);
	}
	{
		const std::string str("abcdefghijklmnop");
		const auto str1 = SString8(str);
		const auto str2 = std::string(str);
		testAreEqual(str1, str2);
	}
}

TEST(String8TestAsStringView)
{
	{
		const std::string_view str;
		const auto str1 = SString8(str);
		const auto str2 = std::string(str);
		EXPECT_EQ(str2, str1.asStringView());
		EXPECT_EQ(str1.asStringView(), str2);
	}
	{
		const std::string_view str("abcd");
		const auto str1 = SString8(str);
		const auto str2 = std::string(str);
		EXPECT_EQ(str1.asStringView(), str2);
	}
	{
		const std::string_view str("abcdefghijklmnop");
		const auto str1 = SString8(str);
		const auto str2 = std::string(str);
		EXPECT_EQ(str1.asStringView(), str2);
	}
}

TEST(String8TestConstructorOtherPos)
{
	const std::string stdstr(std::string_view("abcdefghijklmnop"));
	const SString8 sstr(stdstr);
	assertAreEqual(sstr, stdstr);

	for (size_t pos = 0; pos < stdstr.length(); ++pos)
	{
		const auto str1 = SString8(sstr, pos);
		const auto str2 = std::string(stdstr, pos);
		testAreEqual(str1, str2);
	}
}

TEST(String8TestConstructorOtherPosCount)
{
	const std::string stdstr(std::string_view("abcdefghijklmnop"));
	const SString8 sstr(stdstr);
	assertAreEqual(sstr, stdstr);

	for (size_t pos = 0; pos < stdstr.length(); ++pos)
	{
		for (size_t c = 0; c < stdstr.length(); ++c)
		{
			const auto str1 = SString8(sstr, pos, c);
			const auto str2 = std::string(stdstr, pos, c);
			testAreEqual(str1, str2);
		}
	}
}

TEST(String8TestConstructorCharStarCount)
{
	//SString8(const CharT* s, size_type count)
	const std::string stdstr(std::string_view("abcdefghijklmnop"));
	const SString8 sstr(stdstr);
	assertAreEqual(sstr, stdstr);

	const auto pStdData = stdstr.data();
	for (size_t c = 0; c < stdstr.length(); ++c)
	{
		const auto str1 = SString8(pStdData, c);
		const auto str2 = std::string(pStdData, c);
		testAreEqual(str1, str2);
	}
}

TEST(String8TestConstructorCharStar)
{
	//SString8(const CharT* s)
	const std::string stdstr(std::string_view("abcdefghijklmnop"));
	const SString8 sstr(stdstr);
	assertAreEqual(sstr, stdstr);

	for (size_t c = 0; c < stdstr.length(); ++c)
	{
		const auto str0 = std::string(stdstr.data(), c);
		const auto pStdData = str0.data();
		const auto str1 = SString8(pStdData);
		const auto str2 = std::string(pStdData);
		testAreEqual(str1, str2);
	}
}


int SString8Test::Test()
{
	return runAllTests();
}
