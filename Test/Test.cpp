// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define  _CRT_SECURE_NO_WARNINGS 1

#include <iostream>
#include "SString8Test.h"

int main()
{
    int count = 0;
    count += SString8Test::Test();
    std::cout << count << "\n";
    return count;
}


/*
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <array>
int main(int , char** argv)
{
    auto ch = 'U';
    std::array<char, 8> x{};
    strncpy(x.data(), argv[0], 7);
    x[7] = '\0';

    auto result1 = std::find(begin(x), end(x), ch);

    auto d = std::distance(x.begin(), result1);
    auto d2 = static_cast<int>(d);
    return d2;
}
*/