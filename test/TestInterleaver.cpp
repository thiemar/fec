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

TEST(DeinterleaverTest, NonPuncturing) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1>, 2u, 4u>;

    Thiemar::bool_vec_t in[TestInterleaver::in_buf_len()] = {0};
    Thiemar::bool_vec_t in_reconstructed[TestInterleaver::in_buf_len()] = {0};
    uint8_t out[TestInterleaver::out_buf_len()];
    in[0] = 0x581A3C6F00000000u;
    in[1] = 0x9C7BE54200000000u;

    TestInterleaver::interleave(in, out);
    TestInterleaver::deinterleave(out, in_reconstructed);

    EXPECT_EQ(in[0], in_reconstructed[0]);
    EXPECT_EQ(in[1], in_reconstructed[1]);
}

TEST(DeinterleaverTest, Rate7_8) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0>, 2u, 7u>;

    Thiemar::bool_vec_t in[TestInterleaver::in_buf_len()] = {0};
    Thiemar::bool_vec_t in_reconstructed[TestInterleaver::in_buf_len()] = {0};
    uint8_t out[TestInterleaver::out_buf_len()];
    in[0] = 0xF539DBC027A52700u;
    in[1] = 0xC638F4DD7291B900u;

    TestInterleaver::interleave(in, out);
    TestInterleaver::deinterleave(out, in_reconstructed);

    EXPECT_EQ(0x8110084021200500u, in_reconstructed[0]);
    EXPECT_EQ(0xC428D48D52913800u, in_reconstructed[1]);
}

TEST(DeinterleaverTest, Rate1_3) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1, 1>, 3u, 2u>;

    Thiemar::bool_vec_t in[TestInterleaver::in_buf_len()] = {0};
    Thiemar::bool_vec_t in_reconstructed[TestInterleaver::in_buf_len()] = {0};
    uint8_t out[TestInterleaver::out_buf_len()];
    in[0] = 0xF38A000000000000u;
    in[1] = 0xC837000000000000u;
    in[2] = 0xA4CC000000000000u;

    TestInterleaver::interleave(in, out);
    TestInterleaver::deinterleave(out, in_reconstructed);

    EXPECT_EQ(in[0], in_reconstructed[0]);
    EXPECT_EQ(in[1], in_reconstructed[1]);
    EXPECT_EQ(in[2], in_reconstructed[2]);
}

TEST(DeinterleaverTest, LargeBlockSize) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1, 0, 1, 1, 0, 0, 1, 1, 0>, 2u, 20u>;

    Thiemar::bool_vec_t in[TestInterleaver::in_buf_len()] = {0};
    Thiemar::bool_vec_t in_reconstructed[TestInterleaver::in_buf_len()] = {0};
    uint8_t out[TestInterleaver::out_buf_len()];
    in[0] = 0x6897Db5E9F618C0Au;
    in[1] = 0x1627F7DD8E55C4A9u;
    in[2] = 0x10297BB7C986FF62u;
    in[3] = 0x17255DA54EC7B77Fu;
    in[4] = 0x9F5CB34400000000u;
    in[5] = 0xE6CC5B4A00000000u;

    TestInterleaver::interleave(in, out);
    TestInterleaver::deinterleave(out, in_reconstructed);

    EXPECT_EQ(0x28035A569521080Au, in_reconstructed[0]);
    EXPECT_EQ(0x1625A5490A5484A9u, in_reconstructed[1]);
    EXPECT_EQ(0x102129234886B520u, in_reconstructed[2]);
    EXPECT_EQ(0x030054A50C431256u, in_reconstructed[3]);
    EXPECT_EQ(0x0B58920400000000u, in_reconstructed[4]);
    EXPECT_EQ(0xA48C4B4A00000000u, in_reconstructed[5]);
}
