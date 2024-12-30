#include <PintTest.h>

#include <string_view>
#include <vector>

int main(int argc, const char* argv[])
{
    return PintTest::runAllTests(std::vector<std::string_view>(argv, argv + argc));
}
