#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/Interleaver.h"

using TestInterleaver = Thiemar::Convolutional::Interleaver<Thiemar::BinarySequence<1, 1>, 2u, 8u>;

TEST(InterleaverTest, NumIterations) {
    EXPECT_EQ(2u, TestInterleaver::num_iterations());
}

TEST(InterleaverTest, SequenceTestNonPuncturing) {
    Thiemar::Convolutional::Interleaver<Thiemar::BinarySequence<1, 1>, 2u, 8u>::print_info();
}

TEST(InterleaverTest, SequenceTestRate2_3) {
    Thiemar::Convolutional::Interleaver<Thiemar::BinarySequence<1, 1, 0, 1>, 2u, 4u>::print_info();
}

TEST(InterleaverTest, SequenceTestRate3_4) {
    Thiemar::Convolutional::Interleaver<Thiemar::BinarySequence<1, 1, 0, 1, 1, 0>, 2u, 6u>::print_info();
}

TEST(InterleaverTest, SequenceTestRate5_6) {
    Thiemar::Convolutional::Interleaver<Thiemar::BinarySequence<1, 1, 0, 1, 1, 0, 0, 1, 1, 0>, 2u, 5u>::print_info();
}

TEST(InterleaverTest, SequenceTestRate7_8) {
    Thiemar::Convolutional::Interleaver<Thiemar::BinarySequence<1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0>, 2u, 7u>::print_info();
}
