#include <gtest/gtest.h>

#include "CircularBuffer.h"

TEST(CircularBufferTest, CreatesWithZeroCapacity) {
    CircularBuffer buf;
    buf.push_back('a');
    buf.push_back('a');
    EXPECT_TRUE(buf.empty());
}

TEST(CircularBufferTest, CreatesWithSpecifiedCapacity) {
    CircularBuffer buf(5);
    EXPECT_EQ(buf.capacity(), 5);
}

TEST(CircularBufferTest, CreatesWithSpecifiedCapacityAndSpecifiedItems) {
    CircularBuffer buf(5, 'a');
    EXPECT_EQ(buf.capacity(), 5);
    EXPECT_EQ(buf.size(), 5);
}

TEST(CircularBufferTest, CanAccessElements_WithoutBoundChecking) {
    CircularBuffer buf(5, '0');
    buf.push_back('0');

    for (int i = 0; i < 5; ++i) {
        buf[i] = static_cast<char>(i);
    }
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(buf[i], i);
    }
}

TEST(CircularBufferTest, CanAccessElements_WithBoundChecking) {
    CircularBuffer buf(5, '0');
    buf.push_back('0');

    for (int i = 0; i < 5; ++i) {
        buf.at(i) = static_cast<char>(i);
    }
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(buf.at(i), i);
    }

    EXPECT_THROW(buf.at(45), std::out_of_range);
}

TEST(CircularBufferTest, FrontAndBackWorks) {
    CircularBuffer buf(5, '0');
    buf.push_back('0');
    EXPECT_EQ(buf.front(), '0');
    EXPECT_EQ(buf.back(), '0');
}

TEST(CircularBufferTest, FrontAndBackWorksConst) {
    const CircularBuffer buf(5, '0');
    EXPECT_EQ(buf.front(), '0');
    EXPECT_EQ(buf.back(), '0');
}

TEST(CircularBufferTest, ResizeWorks) {
    CircularBuffer buf(5, '0');
    buf.resize(10, '1');
    EXPECT_EQ(buf.capacity(), 10);
    EXPECT_EQ(buf[0], '0');
    EXPECT_EQ(buf[4], '0');
    EXPECT_EQ(buf[5], '1');
    EXPECT_EQ(buf[9], '1');
}

TEST(CircularBufferTest, PushFrontWorks) {
    CircularBuffer buf(3, '0');
    buf.push_front('1');
    buf.push_front('2');
    EXPECT_EQ(buf[0], '2');
    EXPECT_EQ(buf[1], '1');
    EXPECT_EQ(buf[2], '0');
}

TEST(CircularBufferTest, PushBackWorks) {
    CircularBuffer buf(3, '0');
    buf.push_back('1');
    buf.push_back('2');
    EXPECT_EQ(buf[0], '0');
    EXPECT_EQ(buf[1], '1');
    EXPECT_EQ(buf[2], '2');
}

TEST(CircularBufferTest, PopBackWorks) {
    CircularBuffer buf = {'1', '2', '3'};
    buf.pop_back();
    EXPECT_EQ(buf.size(), 2);
    buf.pop_back();
    EXPECT_EQ(buf.size(), 1);
    EXPECT_EQ(buf[0], '1');
}

TEST(CircularBufferTest, PopFrontWorks) {
    CircularBuffer buf = {'1', '2', '3'};
    buf.pop_front();
    EXPECT_EQ(buf.size(), 2);
    buf.pop_front();
    EXPECT_EQ(buf.size(), 1);
    EXPECT_EQ(buf[0], '3');
}

TEST(CircularBufferTest, CreatesWithInitializerList) {
    CircularBuffer buf = {'1', '2', '3'};
    EXPECT_EQ(buf[0], '1');
    EXPECT_EQ(buf[1], '2');
    EXPECT_EQ(buf[2], '3');
}

TEST(CircularBufferTest, LinearizeWorks) {
    CircularBuffer buf = {'1', '2', '3', '4'};
    buf.push_front('0');
    buf.push_front('0');
    EXPECT_FALSE(buf.is_linearized());
    buf.linearize();
    EXPECT_TRUE(buf.is_linearized());
    EXPECT_EQ(buf[0], '0');
    EXPECT_EQ(buf[1], '0');
    EXPECT_EQ(buf[2], '1');
    EXPECT_EQ(buf[3], '2');
}

TEST(CircularBufferTest, RotateWorks) {
    CircularBuffer buf = {'1', '2', '3', '4'};
    buf.push_front('0');
    buf.push_front('0');
    buf.rotate(3);
    EXPECT_EQ(buf[0], '2');
    EXPECT_EQ(buf[1], '0');
    EXPECT_EQ(buf[2], '0');
    EXPECT_EQ(buf[3], '1');
}

TEST(CircularBufferTest, FullWorks) {
    CircularBuffer buf = {'1', '2', '3', '4'};
    EXPECT_TRUE(buf.full());
    buf.pop_back();
    EXPECT_FALSE(buf.full());
}



