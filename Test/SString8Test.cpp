#include "SString8.h"
#include "SString8Test.h"

#include "UnitTest.h"

#include<iostream>

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

TEST(String8TestConstructorInputItFirstLast)
{
	char mutable_c_str[] = "abcdefghijklmnop";
	const std::string str1(std::begin(mutable_c_str) + 8, std::end(mutable_c_str) - 1);
	const SString8 str2(std::begin(mutable_c_str) + 8, std::end(mutable_c_str) - 1);
	testAreEqual(str1, str2);
}

TEST(String8TestConstructorInitialiserList)
{
	const std::initializer_list<char> ilist = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm' };
	const std::string str1(ilist);
	const SString8 str2(ilist);
	testAreEqual(str1, str2);
}


namespace
{
	struct StringViewLike
	{
		std::string m_Str;
		StringViewLike(std::string str) : m_Str(str) {}
		operator std::string_view() const { return m_Str.operator std::basic_string_view<char, std::char_traits<char>>(); }
		operator const char* () const = delete;
		size_t length() const { return m_Str.length(); }
	};
}

TEST(String8TestConstructorStringViewLike)
{
	const StringViewLike str("abcdefghijklmnop");
	const std::string str1(str);
	const SString8 str2(str);
	testAreEqual(str1, str2);
	auto pStr1 = str1.data();
	auto pStr2 = str2.data();
	EXPECT_EQ(0, strcmp(pStr1, pStr2));
}

TEST(String8TestConstructorStringViewLikePosN)
{
	const StringViewLike svl("abcdefghijklmnop");

	for (size_t pos = 0; pos < svl.length(); ++pos)
	{
		for (size_t c = 0; c < svl.length(); ++c)
		{
			const auto str1 = SString8(svl, pos, c);
			const auto str2 = std::string(svl, pos, c);
			testAreEqual(str1, str2);
		}
	}
}



TEST(CppReferenceConstructorTests)
{
	{
		std::string s1;
		SString8 s2;
		testAreEqual(s1, s2);
	}
	{
		std::string s1(4, '=');
		SString8 s2(4, '=');
		testAreEqual(s1, s2);
	}
	{
		std::string const other3("Exemplary");
		std::string s1(other3, 0, other3.length() - 1);
		SString8 s2(other3, 0, other3.length() - 1);
		testAreEqual(s1, s2);
	}
	{
		std::string const other4("Mutatis Mutandis");
		std::string s1(other4, 8);
		SString8 s2(other4, 8);
		testAreEqual(s1, s2);
	}
	{
		std::string s1("C-style string", 7);
		SString8 s2("C-style string", 7);
		testAreEqual(s1, s2);
	}
	{
		std::string s1("C-style\0string");
		SString8 s2("C-style\0string");
		testAreEqual(s1, s2);
	}
	{
		char mutable_c_str[] = "another C-style string";
		std::string s1(std::begin(mutable_c_str) + 8, std::end(mutable_c_str) - 1);
		SString8 s2(std::begin(mutable_c_str) + 8, std::end(mutable_c_str) - 1);
		testAreEqual(s1, s2);
	}
	{
		std::string const other8("Exemplar");
		std::string s1(other8);
		SString8 s2(other8);
		testAreEqual(s1, s2);
	}
	{
		std::string s1(std::string("C++ by ") + std::string("example"));
		SString8 s2(std::string("C++ by ") + std::string("example"));
		testAreEqual(s1, s2);
	}
	{
		std::string s1({ 'C', '-', 's', 't', 'y', 'l', 'e' });
		SString8 s2({ 'C', '-', 's', 't', 'y', 'l', 'e' }); // @TODO - check this - currently going via std::string
		testAreEqual(s1, s2);
	}
	{
		std::string s1(3, static_cast<char>(std::toupper('a')));
		SString8 s2(3, static_cast<char>(std::toupper('a')));
		testAreEqual(s1, s2);
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

	{
		const auto longStr = new char[9] {'1','2','3','4','5','6','7','8','\0'};

		SString8Data data;
		data.m_Storage.m_Large.m_pStr = reinterpret_cast<uintptr_t>(longStr);
		EXPECT_EQ(reinterpret_cast<char*>(data.m_Storage.m_Large.m_pStr), longStr);

		data.setLarge();
		EXPECT_FALSE(data.isBuffer());
		EXPECT_NE(reinterpret_cast<char*>(data.m_Storage.m_Large.m_pStr), longStr);
		EXPECT_EQ(data.getAsPtr(), longStr);
		const auto pStr = data.getAsPtr();
		EXPECT_EQ(0, strcmp(pStr, longStr));
	}
	std::string_view shorttext = "abcdefg";
	std::string_view longtext = "abcdefghijklmnopqrstuvwxyz";
	{
		for (const auto& txt : { longtext })
		{
			for (auto i = 0U; i < txt.size(); ++i)
			{
				std::basic_string_view sub(txt.data(), txt.data() + i);
				std::string substr(sub);
				SString8Data str(sub);
				auto pStr1 = substr.data();
				auto pStr2 = str.data();
				EXPECT_EQ(0, strcmp(pStr1, pStr2));
			}
		}
	}
	{
		for (const auto& txt : { longtext })
		{
			for (auto i = 0U; i < txt.size(); ++i)
			{
				std::basic_string_view sub(txt.data(), txt.data() + i);
				SString8Data str1(sub);
				SString8Data str2(str1);
				auto pStr1 = str1.data();
				auto pStr2 = str2.data();
				EXPECT_EQ(0, strcmp(pStr1, pStr2));
			}
		}
	}
	{
		for (const auto& txt : { shorttext })
		{
			for (auto i = 0U; i < txt.size(); ++i)
			{
				std::basic_string_view sub(txt.data(), txt.data() + i);
				std::string str(sub);
				SString8Data str1(sub);
				SString8Data str2(std::move(str1));
				auto pStr1 = str.data();
				auto pStr2 = str2.data();
				EXPECT_EQ(0, strcmp(pStr1, pStr2));
			}
		}
		for (const auto& txt : { longtext })
		{
			for (auto i = shorttext.size(); i < txt.size(); ++i)
			{
				std::basic_string_view sub(txt.data(), txt.data() + i);
				std::string str(sub);
				SString8Data str1(sub);
				SString8Data str2(std::move(str1));
				auto pStr1 = str.data();
				auto pStr2 = str2.data();
				EXPECT_EQ(0, strcmp(pStr1, pStr2));
			}
		}
	}
}

int SString8Test::Test()
{
	{
#pragma warning( push )
#pragma warning( disable : 4127)
		if      (__cplusplus == 202101L) std::cout << "C++23\n";
		else if (__cplusplus == 202002L) std::cout << "C++20\n";
		else if (__cplusplus == 201703L) std::cout << "C++17\n";
		else if (__cplusplus == 201402L) std::cout << "C++14\n";
		else if (__cplusplus == 201103L) std::cout << "C++11\n";
		else if (__cplusplus == 199711L) std::cout << "C++98\n";
		else std::cout << "pre-standard C++." << __cplusplus << "\n";
#pragma warning( pop ) 
	}

	return runAllTests();

}
