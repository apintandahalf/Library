#include "SString8.h"
#include "SString8Test.h"

#include <iostream>
#include <source_location>
#include <functional>
#include <vector>


namespace
{
int fails = 0;
int tests_failed = 0;
int tests_ran = 0;

#define WRAP(a) \
	do { a } while (false)

#define REPORT \
	const auto src = std::source_location::current(); \
	std::cout << "Test failed: " << src.file_name() << "." << src.function_name() << "." << src.line() << ": " 

#define EXPECT_EQ(a,b) \
	WRAP (if (!((a) == (b))) { REPORT << #a << " == " #b << "\n"; fails += 1; } )

#define EXPECT_NE(a,b) \
	WRAP (if ((a) == (b)) { REPORT << #a << " != " #b << "\n"; fails += 1; } )

#define EXPECT_TRUE(a) \
	WRAP (if (!(a)) { REPORT << "!" << #a "\n"; fails += 1; } )

#define EXPECT_FALSE(a) \
	WRAP (if (a) { REPORT << << #a "\n"; fails += 1; } )

#define ASSERT_EQ(a,b) \
	WRAP (if (!((a) == (b))) { REPORT << #a << " == " #b << "\n"; fails += 1; return; } )

#define ASSERT_NE(a,b) \
	WRAP (if ((a) == (b)) { REPORT << #a << " != " #b << "\n"; fails += 1; return; } )

#define ASSERT_TRUE(a) \
	WRAP (if (!(a)) { REPORT << "!" << #a "\n"; fails += 1; return; } )

#define ASSERT_FALSE(a) \
	WRAP (if (a) { REPORT << << #a "\n"; fails += 1; return; } )

std::vector<std::function<void()>> fns;
bool registerTestFn(std::function<void()> fn)
{
	fns.push_back(fn);
	return true;
}

constexpr auto GREEN_TEXT_START = "\x1B[32m";
constexpr auto RED_TEXT_START   = "\x1B[31m";
constexpr auto COLOUR_TEXT_END  = "\033[0m\t\t";

#define TEST(TestCase) \
	namespace {\
	struct TestStruct##TestCase \
	{ \
		static void Test() \
		{\
		    ++tests_ran; \
			const auto currentFails = fails; \
			std::cout << GREEN_TEXT_START << "Testing " << #TestCase << COLOUR_TEXT_END << "\n"; \
			TestStruct##TestCase::TestBody(); \
			if (currentFails < fails) \
			{ \
				++tests_failed; \
				std::cout << RED_TEXT_START << "Tested " << #TestCase << COLOUR_TEXT_END << "\n"; \
			} \
			else \
			{ \
				std::cout << GREEN_TEXT_START << "Tested " << #TestCase << COLOUR_TEXT_END << "\n"; \
			} \
		} \
		static void TestBody(); \
	};} \
	static const auto sTestStruct##TestCase = registerTestFn(TestStruct##TestCase::Test); \
	void TestStruct##TestCase::TestBody()

int runAllTests()
{
	fails = 0;
	tests_failed = 0;
	tests_ran = 0;

	for (auto& fn : fns)
		fn();

	if (fails)
		std::cout << RED_TEXT_START << "Ran " << tests_ran << " tests and " << tests_failed << " failed" << COLOUR_TEXT_END << "\n";
	else
		std::cout << GREEN_TEXT_START << "Ran " << tests_ran << " tests and none failed" << COLOUR_TEXT_END << "\n";

	return fails;
}

}


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

//CONSTEXPR SString8::SString8(const CharT* s, size_type count)

int SString8Test::Test()
{
	return runAllTests();
}
