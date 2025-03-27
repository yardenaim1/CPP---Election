#include <gtest/gtest.h>
#include <file_for_test.h> // Include the file where `plus_for_test` is defined

using namespace testing;

namespace Tests
{

// Test case for the `plus_for_test` function
TEST(PlusFunctionTest, HandlesPositiveNumbers) 
{
    EXPECT_EQ(plus_for_test(1, 2), 3);
    EXPECT_EQ(plus_for_test(100, 200), 300);
}

TEST(PlusFunctionTest, HandlesNegativeNumbers) 
{
    EXPECT_EQ(plus_for_test(-1, -2), -3);
    EXPECT_EQ(plus_for_test(-10, 5), -5);
}

TEST(PlusFunctionTest, HandlesZero) 
{
    EXPECT_EQ(plus_for_test(0, 0), 0);
    EXPECT_EQ(plus_for_test(0, 5), 5);
    EXPECT_EQ(plus_for_test(5, 0), 5);
}

// Main entry point for Google Test
int main2(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

} // namespace Tests