#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/Interleaver.h"

TEST(InterleaverTest, NumIterations) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1>, 2u, 8u>;
    EXPECT_EQ(2u, TestInterleaver::num_iterations());
}

TEST(InterleaverTest, NonPuncturing) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1>, 2u, 8u>;

    Thiemar::bool_vec_t temp[2] = {0};
    temp[0] = 0xFF00000000000000u;
    temp[1] = 0xFF00000000000000u;
    Thiemar::bool_vec_t out = TestInterleaver::interleave<0u>(temp, std::make_index_sequence<2u>{});
    printf("%lx\n", out);
}

TEST(InterleaverTest, Rate2_3) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1, 0, 1>, 2u, 4u>;

    Thiemar::bool_vec_t temp[2] = {0};
    temp[0] = 0xFF00000000000000u;
    temp[1] = 0xFF00000000000000u;
    Thiemar::bool_vec_t out = TestInterleaver::interleave<0u>(temp, std::make_index_sequence<2u>{});
    printf("%lx\n", out);
}

TEST(InterleaverTest, Rate3_4) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1, 0, 1, 1, 0>, 2u, 6u>;

    Thiemar::bool_vec_t temp[2] = {0};
    temp[0] = 0xFF00000000000000u;
    temp[1] = 0xFF00000000000000u;
    Thiemar::bool_vec_t out = TestInterleaver::interleave<0u>(temp, std::make_index_sequence<2u>{});
    printf("%lx\n", out);
}

TEST(InterleaverTest, Rate5_6) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1, 0, 1, 1, 0, 0, 1, 1, 0>, 2u, 5u>;

    Thiemar::bool_vec_t temp[2] = {0};
    temp[0] = 0xFF00000000000000u;
    temp[1] = 0xFF00000000000000u;
    Thiemar::bool_vec_t out = TestInterleaver::interleave<0u>(temp, std::make_index_sequence<2u>{});
    printf("%lx\n", out);
}

TEST(InterleaverTest, Rate7_8) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0>, 2u, 7u>;

    Thiemar::bool_vec_t temp[2] = {0};
    temp[0] = 0xFF00000000000000u;
    temp[1] = 0xFF00000000000000u;
    Thiemar::bool_vec_t out = TestInterleaver::interleave<0u>(temp, std::make_index_sequence<2u>{});
    printf("%lx\n", out);
}
