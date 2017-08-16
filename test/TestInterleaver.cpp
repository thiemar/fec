#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/Interleaver.h"

TEST(InterleaverTest, NumIterations) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1>, 2u, 8u>;
    EXPECT_EQ(2u, TestInterleaver::num_iterations());
}

TEST(InterleaverTest, SequenceTestNonPuncturing) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1>, 2u, 8u>;
    TestInterleaver::print_info();
}

TEST(InterleaverTest, SequenceTestRate2_3) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1, 0, 1>, 2u, 4u>;
    TestInterleaver::print_info();
}

TEST(InterleaverTest, SequenceTestRate3_4) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1, 0, 1, 1, 0>, 2u, 6u>;
    TestInterleaver::print_info();
}

TEST(InterleaverTest, SequenceTestRate5_6) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1, 0, 1, 1, 0, 0, 1, 1, 0>, 2u, 5u>;
    TestInterleaver::print_info();
}

TEST(InterleaverTest, SequenceTestRate7_8) {
    using TestInterleaver = Thiemar::Convolutional::Interleaver<
        Thiemar::BinarySequence<1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0>, 2u, 7u>;
    TestInterleaver::print_info();
}
