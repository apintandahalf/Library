// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "SString8Test.h"

int main()
{
    int count = 0;
    count += SString8Test::Test();
    std::cout << count << "\n";
    return count;
}
