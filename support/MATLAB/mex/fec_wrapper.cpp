#include "mex.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "FEC/Convolutional.h"
#include "FEC/ReedSolomon.h"
#include "FEC/Polar.h"

#include "fec.h"

using ReedSolomonEncoder = Thiemar::ReedSolomon::ReedSolomonEncoder<
    8u,
    Thiemar::ReedSolomon::Polynomials::m_8_285,
    MESSAGE_PARITY_LENGTH
>;

using ConvolutionalEncoder = Thiemar::Convolutional::PuncturedConvolutionalEncoder<
    7u,
    Thiemar::Convolutional::PuncturingMatrices::n_2_rate_1_2,
    Thiemar::BinarySequence<1, 1, 0, 1, 1, 0, 1>,
    Thiemar::BinarySequence<1, 0, 0, 1, 1, 1, 1>
>;

using ConvolutionalDecoder = Thiemar::Convolutional::PuncturedHardDecisionViterbiDecoder<
    7u,
    1536u,
    Thiemar::Convolutional::PuncturingMatrices::n_2_rate_1_2,
    Thiemar::BinarySequence<1, 1, 0, 1, 1, 0, 1>,
    Thiemar::BinarySequence<1, 0, 0, 1, 1, 1, 1>
>;

using PolarDataIndices = Thiemar::Polar::PolarCodeConstructor<POLAR_BLOCK_SIZE, POLAR_BLOCK_SIZE_SHORTENED, POLAR_DATA_SIZE>::data_index_sequence;
using PolarEncoder = Thiemar::Polar::PolarEncoder<POLAR_BLOCK_SIZE, POLAR_BLOCK_SIZE_SHORTENED, POLAR_DATA_SIZE, PolarDataIndices>;
using PolarDecoder = Thiemar::Polar::SuccessiveCancellationListDecoder<POLAR_BLOCK_SIZE, POLAR_BLOCK_SIZE_SHORTENED, POLAR_DATA_SIZE, PolarDataIndices, int8_t>;

void rs_encode(const uint8_t *data, uint8_t *buf) {
    auto in = Thiemar::Detail::to_array<uint8_t, MESSAGE_DATA_LENGTH+MESSAGE_PARITY_LENGTH>(data);
    std::fill_n(in.begin() + MESSAGE_DATA_LENGTH, MESSAGE_PARITY_LENGTH, 0u);
    ReedSolomonEncoder::encode<MESSAGE_DATA_LENGTH>(in);
    memcpy(buf, in.data(), MESSAGE_DATA_LENGTH+MESSAGE_PARITY_LENGTH);
}

size_t conv_get_encoded_len() {
    return ConvolutionalEncoder::calculate_output_length(
        MESSAGE_DATA_LENGTH + MESSAGE_PARITY_LENGTH);
}

void conv_encode(const uint8_t *data, uint8_t *buf) {
    auto in = Thiemar::Detail::to_array<uint8_t, MESSAGE_DATA_LENGTH + MESSAGE_PARITY_LENGTH>(data);
    auto out = ConvolutionalEncoder::encode(in);
    memcpy(buf, out.data(), out.size());
}


void conv_decode(const uint8_t *data, uint8_t *buf) {
    auto in = Thiemar::Detail::to_array<uint8_t, ConvolutionalEncoder::calculate_output_length(
        MESSAGE_DATA_LENGTH + MESSAGE_PARITY_LENGTH)>(data);
    auto out = ConvolutionalDecoder::decode(in);
    memcpy(buf, out.data(), out.size());
}

void polar_encode(const uint8_t *data, uint8_t *buf) {
    auto in = Thiemar::Detail::to_array<uint8_t, POLAR_DATA_SIZE / 8u>(data);
    auto out = PolarEncoder::encode(in);
    memcpy(buf, out.data(), out.size());
}

void polar_decode(const uint8_t *data, uint8_t *buf) {
    auto in = Thiemar::Detail::to_array<uint8_t, POLAR_BLOCK_SIZE_SHORTENED / 8u>(data);
    auto out = PolarDecoder::decode(in);
    memcpy(buf, out.data(), out.size());
}
