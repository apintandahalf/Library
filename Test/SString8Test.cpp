// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "SString8.h"
#include "SString8Test.h"

#include <string>
#include <cstring>

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

	int String8TestConstructor()
	{
		int count = 0;

		{
			SString8 str1;
			std::string str2;
			if (!areEqual(str1, str2))
				++count;
		}

		{
			{
				const auto str1 = SString8(0, 'A');
				const auto str2 = std::string(0, 'A');
				if (!areEqual(str1, str2))
					++count;
			}
			for (size_t i = 1U; i < 40U; ++i)
			{
				const auto str1 = SString8(i, 'A');
				const auto str2 = std::string(i, 'A');
				if (!areEqual(str1, str2))
					++count;
			}
		}

		return count;
	}
}

int SString8Test::Test()
{
	auto count = String8TestConstructor();
	return count;
}
