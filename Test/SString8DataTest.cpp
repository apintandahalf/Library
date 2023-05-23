#include "SString8.h"

#include "UnitTest.h"


TEST(Data)
{
	{
		SString8Data data;
		EXPECT_TRUE(data.isBuffer());
		data.setBuffer();
		EXPECT_TRUE(data.isBuffer());
		data.setLarge();
		EXPECT_FALSE(data.isBuffer());
		data.setBuffer();
		EXPECT_TRUE(data.isBuffer());
	}
	for (const std::string_view str : { "", "1", "1234567", "1234568" })
	{
		SString8Data data(str);
		EXPECT_TRUE(data.isBuffer());
		EXPECT_EQ(data.size(), str.size());
		EXPECT_EQ(strcmp(data.data(), str.data()), 0);
		const SString8Data& data2 = data;
		EXPECT_EQ(strcmp(data2.data(), str.data()), 0);
		if (data.isBuffer())
		{
			auto p = data2.data();
			EXPECT_EQ(p, data.m_Storage.m_Buffer.m_Buffer);
		}
		else
		{
			auto p = data2.data();
			EXPECT_NE(reinterpret_cast<uintptr_t>(p), data.m_Storage.m_Large.m_pStr);
			EXPECT_EQ(p, data.getAsPtr());
			EXPECT_EQ(p, data2.getAsPtr());
		}
	}
	const auto strs = { "", "1", "1234567", "1234568" };
	for (const std::string_view str1 : strs)
	{
		for (const std::string_view str2 : strs)
		{
			SString8Data data1(str1);
			SString8Data data2(str2);
			EXPECT_EQ(strcmp(data1.data(), str1.data()), 0);
			EXPECT_EQ(strcmp(data2.data(), str2.data()), 0);
			data1.swap(data2);
			EXPECT_EQ(strcmp(data1.data(), str2.data()), 0);
			EXPECT_EQ(strcmp(data2.data(), str1.data()), 0);
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
	for (const auto& txt : { longtext })
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
	for (const auto& txt : { longtext })
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
	for (const auto& txt : { shorttext })
	{
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
	for (const auto& txt : { longtext })
	{
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
	for (const auto& txt : { shorttext })
	{
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
	for (const auto& txt : { longtext })
	{
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
	for (const auto& txt : { shorttext })
	{
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
	for (const auto& txt : { longtext })
	{
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

	{
		const auto longStr = new char[9] {'1', '2', '3', '4', '5', '6', '7', '8', '\0'};

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
}
