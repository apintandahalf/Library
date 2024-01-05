#include <UnitTest.h>
#include <UnitTest2.h>

int main()
{
    return UTest::runAllTests() + UTest2::runAllTests();
}
