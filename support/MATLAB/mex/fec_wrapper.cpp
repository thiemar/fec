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

void rs_encode(const uint8_t *data, uint8_t *buf) {
    uint8_t temp[MESSAGE_DATA_LENGTH+MESSAGE_PARITY_LENGTH] = {};
    memcpy(temp, data, MESSAGE_DATA_LENGTH);
    ReedSolomonEncoder::encode<MESSAGE_DATA_LENGTH>(temp);
    memcpy(buf, temp, MESSAGE_DATA_LENGTH+MESSAGE_PARITY_LENGTH);
}

size_t conv_get_encoded_len(size_t len) {
    return ConvolutionalEncoder::calculate_output_length(len);
}

void conv_encode(const uint8_t *data, size_t len, uint8_t *buf) {
    ConvolutionalEncoder::encode(data, len, buf);
}

size_t conv_get_decoded_len(size_t len) {
    return ConvolutionalDecoder::calculate_output_length(len);
}

void conv_decode(const uint8_t *data, size_t len, uint8_t *buf) {
    memset(buf, 0u, len);
    ConvolutionalDecoder::decode(data, len, buf);
}

void polar_encode(const uint8_t *data, uint8_t *buf) {
    auto out = PolarEncoder::encode(data);
    memcpy(buf, out.data(), out.size());
}
