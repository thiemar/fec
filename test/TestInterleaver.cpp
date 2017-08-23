#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/Interleaver.h"

TEST(InterleaverTest, NonPuncturing) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1>, 2u, 4u>;

    Thiemar::bool_vec_t in[TestInterleaver::in_buf_len()] = {0};
    uint8_t out[TestInterleaver::out_buf_len()];
    in[0] = 0xFFFFFFFF00000000u;
    in[1] = 0xFFFFFFFF00000000u;

    TestInterleaver::interleave(in, out);

    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(0xFFu, out[i]);
    }

    EXPECT_EQ(2u, TestInterleaver::in_buf_len());
    EXPECT_EQ(8u, TestInterleaver::out_buf_len());
}

TEST(InterleaverTest, Rate2_3) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1, 0, 1>, 2u, 4u>;

    Thiemar::bool_vec_t in[TestInterleaver::in_buf_len()] = {0};
    uint8_t out[TestInterleaver::out_buf_len()];
    in[0] = 0xFFFFFFFF00000000u;
    in[1] = 0xFFFFFFFF00000000u;

    TestInterleaver::interleave(in, out);

    for (int i = 0; i < 6; i++) {
        EXPECT_EQ(0xFFu, out[i]);
    }

    EXPECT_EQ(2u, TestInterleaver::in_buf_len());
    EXPECT_EQ(6u, TestInterleaver::out_buf_len());
}

TEST(InterleaverTest, Rate3_4) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1, 0, 1, 1, 0>, 2u, 6u>;

    Thiemar::bool_vec_t in[TestInterleaver::in_buf_len()] = {0};
    uint8_t out[TestInterleaver::out_buf_len()];
    in[0] = 0xFFFFFFFFFFFF0000u;
    in[1] = 0xFFFFFFFFFFFF0000u;

    TestInterleaver::interleave(in, out);

    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(0xFFu, out[i]);
    }

    EXPECT_EQ(2u, TestInterleaver::in_buf_len());
    EXPECT_EQ(8u, TestInterleaver::out_buf_len());
}

TEST(InterleaverTest, Rate5_6) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1, 0, 1, 1, 0, 0, 1, 1, 0>, 2u, 5u>;

    Thiemar::bool_vec_t in[TestInterleaver::in_buf_len()] = {0};
    uint8_t out[TestInterleaver::out_buf_len()];
    in[0] = 0xFFFFFFFFFF000000u;
    in[1] = 0xFFFFFFFFFF000000u;

    TestInterleaver::interleave(in, out);

    for (int i = 0; i < 6; i++) {
        EXPECT_EQ(0xFFu, out[i]);
    }

    EXPECT_EQ(2u, TestInterleaver::in_buf_len());
    EXPECT_EQ(6u, TestInterleaver::out_buf_len());
}

TEST(InterleaverTest, Rate7_8) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0>, 2u, 7u>;

    Thiemar::bool_vec_t in[TestInterleaver::in_buf_len()] = {0};
    uint8_t out[TestInterleaver::out_buf_len()];
    in[0] = 0xFFFFFFFFFFFFFF00u;
    in[1] = 0xFFFFFFFFFFFFFF00u;

    TestInterleaver::interleave(in, out);

    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(0xFFu, out[i]);
    }

    EXPECT_EQ(2u, TestInterleaver::in_buf_len());
    EXPECT_EQ(8u, TestInterleaver::out_buf_len());
}

TEST(InterleaverTest, Rate1_3) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1, 1>, 3u, 2u>;

    Thiemar::bool_vec_t in[TestInterleaver::in_buf_len()] = {0};
    uint8_t out[TestInterleaver::out_buf_len()];
    in[0] = 0xFFFF000000000000u;
    in[1] = 0xFFFF000000000000u;
    in[2] = 0xFFFF000000000000u;

    TestInterleaver::interleave(in, out);

    for (int i = 0; i < 6; i++) {
        EXPECT_EQ(0xFFu, out[i]);
    }

    EXPECT_EQ(3u, TestInterleaver::in_buf_len());
    EXPECT_EQ(6u, TestInterleaver::out_buf_len());
}

TEST(InterleaverTest, LargeBlockSize) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1, 0, 1, 1, 0, 0, 1, 1, 0>, 2u, 20u>;

    Thiemar::bool_vec_t in[TestInterleaver::in_buf_len()] = {0};
    uint8_t out[TestInterleaver::out_buf_len()];
    in[0] = 0xFFFFFFFFFFFFFFFFu;
    in[1] = 0xFFFFFFFFFFFFFFFFu;
    in[2] = 0xFFFFFFFFFFFFFFFFu;
    in[3] = 0xFFFFFFFFFFFFFFFFu;
    in[4] = 0xFFFFFFFF00000000u;
    in[5] = 0xFFFFFFFF00000000u;

    TestInterleaver::interleave(in, out);

    for (int i = 0; i < 24; i++) {
        EXPECT_EQ(0xFFu, out[i]);
    }

    EXPECT_EQ(6u, TestInterleaver::in_buf_len());
    EXPECT_EQ(24u, TestInterleaver::out_buf_len());
}
