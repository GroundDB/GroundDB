#include <gtest/gtest.h>
#include <memoryPool.hh>
// Demonstrate some basic assertions.
TEST(AddTest, BasicAssertions)
{
    EXPECT_EQ(7, add(2, 5));
}