#include "SString8.h"

#include "UnitTest.h"


TEST(Data)
{
	{
		SString8Data data;
		EXPECT_TRUE(data.isBuffer());
		EXPECT_FALSE(data.isPtr());
		EXPECT_FALSE(data.isSmall());
		EXPECT_FALSE(data.isMedium());
		EXPECT_FALSE(data.isLarge());
		EXPECT_EQ(data.size(), 0);
	}

	for (const std::string_view str : { "", "1", "1234567", "12345678" })
	{
		SString8Data data(str);
		EXPECT_TRUE(str.length() > 7 || data.isBuffer()) << str;
		EXPECT_EQ(data.size(), str.size()) << str;
		EXPECT_EQ(strcmp(data.data(), str.data()), 0) << str;
		const SString8Data& data2 = data;
		EXPECT_EQ(strcmp(data2.data(), str.data()), 0) << str;
		if (data.isBuffer())
		{
			auto p = data2.data();
			EXPECT_EQ(p, data.m_Storage.m_Buffer.m_Buffer) << str;
		}
		else
		{
			auto p = data2.data();
			EXPECT_NE(reinterpret_cast<uintptr_t>(p), data.m_Storage.m_pLargeStr) << str;
			EXPECT_EQ(p, data.getAsPtr()) << str;
			EXPECT_EQ(p, data2.getAsPtr()) << str;
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
	}
	for (const auto sz : { 8ULL, 255Ull })
	{
		const std::string str(sz, 'a');
		SString8Data data(str.data());
		EXPECT_FALSE(data.isBuffer()) << sz;
		EXPECT_TRUE(data.isPtr()) << sz;
		EXPECT_TRUE(data.isSmall()) << sz;
		EXPECT_FALSE(data.isMedium()) << sz;
		EXPECT_FALSE(data.isLarge()) << sz;
		EXPECT_EQ(sz, data.size()) << sz;
	}
	for (const auto sz : { 256Ull, ((1ULL << 15U) - 1U) })
	{
		const std::string str(sz, 'a');
		SString8Data data(str.data());
		EXPECT_FALSE(data.isBuffer()) << sz;
		EXPECT_TRUE(data.isPtr()) << sz;
		EXPECT_FALSE(data.isSmall()) << sz;
		EXPECT_TRUE(data.isMedium()) << sz;
		EXPECT_FALSE(data.isLarge()) << sz;
		EXPECT_EQ(sz, data.size()) << sz;
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
	}

	const auto strs = { "", "1", "1234567", "12345678" };
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
