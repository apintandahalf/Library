// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "SString8.h"
#include "SString8Test.h"


namespace
{
	bool areEqual(const SString8& lhs, const std::string& rhs)
	{
		if (lhs.size() != rhs.size())
			return false;

		const auto pL = lhs.data();
		const auto pR = rhs.data();
		if (pL == nullptr || pR == nullptr)
			return pL == nullptr && pR == nullptr;

		if (0 != strcmp(pL, pR))
			return false;

		return true;
	}

	int String8TestDefaultConstructor()
	{
		int count = 0;

		{
			SString8 str1;
			std::string str2;
			if (!areEqual(str1, str2))
				++count;
		}
		return count;
	}

	int String8TestConstructorCountChar()
	{
		int count = 0;

		{
			const auto str1 = SString8(0, 'A');
			const auto str2 = std::string(0, 'A');
			if (!areEqual(str1, str2))
				++count;
		}
		for (size_t i = 1U; i < 7U; ++i)
		{
			const auto str1 = SString8(i, 'A');
			const auto str2 = std::string(i, 'A');
			if (!areEqual(str1, str2))
				++count;
		}
		for (size_t i = 8U; i < 9U; ++i)
		{
			const auto str1 = SString8(i, 'A');
			const auto str2 = std::string(i, 'A');
			if (!areEqual(str1, str2))
				++count;
		}
		for (size_t i = 9U; i < 40U; ++i)
		{
			const auto str1 = SString8(i, 'A');
			const auto str2 = std::string(i, 'A');
			if (!areEqual(str1, str2))
				++count;
		}

		return count;
	}

	int String8TestConstructorStringView()
	{
		int count = 0;

		{
			const std::string_view str;
			const auto str1 = SString8(str);
			const auto str2 = std::string(str);
			if (!areEqual(str1, str2))
				++count;
		}
		{
			const std::string_view str("abcd");
			const auto str1 = SString8(str);
			const auto str2 = std::string(str);
			if (!areEqual(str1, str2))
				++count;
		}
		{
			const std::string_view str("abcdefghijklmnop");
			const auto str1 = SString8(str);
			const auto str2 = std::string(str);
			if (!areEqual(str1, str2))
				++count;
		}

		return count;
	}

	int String8TestConstructorString()
	{
		int count = 0;

		{
			const std::string str;
			const auto str1 = SString8(str);
			const auto str2 = std::string(str);
			if (!areEqual(str1, str2))
				++count;
		}
		{
			const std::string str("abcd");
			const auto str1 = SString8(str);
			const auto str2 = std::string(str);
			if (!areEqual(str1, str2))
				++count;
		}
		{
			const std::string str("abcdefghijklmnop");
			const auto str1 = SString8(str);
			const auto str2 = std::string(str);
			if (!areEqual(str1, str2))
				++count;
		}

		return count;
	}

	int String8TestAsStringView()
	{
		int count = 0;

		{
			const std::string_view str;
			const auto str1 = SString8(str);
			const auto str2 = std::string(str);
			if (str1.asStringView() != str2.operator std::basic_string_view<char, std::char_traits<char>>())
				++count;
		}
		{
			const std::string_view str("abcd");
			const auto str1 = SString8(str);
			const auto str2 = std::string(str);
			if (str1.asStringView() != str2.operator std::basic_string_view<char, std::char_traits<char>>())
				++count;
		}
		{
			const std::string_view str("abcdefghijklmnop");
			const auto str1 = SString8(str);
			const auto str2 = std::string(str);
			if (str1.asStringView() != str2.operator std::basic_string_view<char, std::char_traits<char>>())
				++count;
		}

		return count;
	}

	int String8TestConstructorOtherPos()
	{
		const std::string stdstr(std::string_view("abcdefghijklmnop"));
		const SString8 sstr(stdstr);
		if (!areEqual(sstr, stdstr))
			return 1;

		int count = 0;

		for (size_t pos = 0; pos < stdstr.length(); ++pos)
		{
			const auto str1 = SString8(sstr, pos);
			const auto str2 = std::string(stdstr, pos);
			if (!areEqual(str1, str2))
				++count;
		}

		return count;
	}

	int String8TestConstructorOtherPosCount()
	{
		const std::string stdstr(std::string_view("abcdefghijklmnop"));
		const SString8 sstr(stdstr);
		if (!areEqual(sstr, stdstr))
			return 1;

		int count = 0;

		for (size_t pos = 0; pos < stdstr.length(); ++pos)
		{
			for (size_t c = 0; c < stdstr.length(); ++c)
			{
				const auto str1 = SString8(sstr, pos, c);
				const auto str2 = std::string(stdstr, pos, c);
				if (!areEqual(str1, str2))
					++count;
			}
		}

		return count;
	}

	//CONSTEXPR SString8::SString8(const CharT* s, size_type count)

}

int SString8Test::Test()
{
	auto count = 0;
	
	count += String8TestDefaultConstructor();
	count += String8TestConstructorCountChar();
	count += String8TestConstructorStringView();
	count += String8TestConstructorString();
	count += String8TestAsStringView();
	count += String8TestConstructorOtherPos();
	count += String8TestConstructorOtherPosCount();

	return count;
}
