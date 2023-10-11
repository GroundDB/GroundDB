#include <gtest/gtest.h>
#include <memoryPool.hh>
// Demonstrate some basic assertions.
TEST(LRU_TEST, BasicAssertions) {
    mempool::LRUCache<int,  int> lru(1);
    lru.put(1, 1);
    EXPECT_EQ(lru.get(1), 1);
    lru.put(2, 2);
    EXPECT_EQ(lru.get(2), 2);
    EXPECT_EQ(lru.get(1), -1);
}