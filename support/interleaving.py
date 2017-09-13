# Test efficient bit interleaving algorithm for implementation in C++.

import math

def unwrap_input_index(puncturing_matrix, start_idx, poly_idx, num_poly, num_bits, constraint_len):
    puncturing_matrix_row = puncturing_matrix[poly_idx::num_poly]
    num_wraps = num_bits / sum(puncturing_matrix_row)
    if (start_idx % len(puncturing_matrix_row)):
        num_wraps += 1

    unwrapped_puncturing_matrix = puncturing_matrix_row * num_wraps

    return [i - start_idx for i in range(start_idx, len(unwrapped_puncturing_matrix)) \
        if unwrapped_puncturing_matrix[i] is 1][:num_bits]

def unwrap_output_index(puncturing_matrix, start_idx, poly_idx, num_poly, num_bits, constraint_len):
    puncturing_matrix_row = puncturing_matrix[poly_idx::num_poly]
    num_wraps = num_bits / sum(puncturing_matrix_row)
    if (start_idx % len(puncturing_matrix_row)):
        num_wraps += 1

    unwrapped_puncturing_matrix = puncturing_matrix * num_wraps
    unwrapped_input_index = unwrap_input_index(puncturing_matrix, start_idx, poly_idx, num_poly, num_bits, constraint_len)

    accumulator = 0
    unwrapped_output_index = []
    last_idx = poly_idx
    for i in unwrapped_input_index:
        accumulator += sum(unwrapped_puncturing_matrix[last_idx:i*num_poly + poly_idx])
        unwrapped_output_index.append(accumulator)
        last_idx = i*num_poly + poly_idx

    return unwrapped_output_index

def calculate_shift_masks(word_size = 32, constraint_length = 7, num_poly = 3, puncturing_matrix = [1, 1, 1]):
    for i in range(num_poly):
        max_bits = (word_size * sum(puncturing_matrix[i::num_poly])) / sum(puncturing_matrix)
        print max_bits
        input_index_sequence = unwrap_input_index(puncturing_matrix, 0, i, num_poly, max_bits, constraint_length)
        print input_index_sequence
        output_index_sequence = unwrap_output_index(puncturing_matrix, 0, i, num_poly, max_bits, constraint_length)
        print output_index_sequence
        diff_sequence = [output_index_sequence[j] - input_index_sequence[j] for j in range(len(input_index_sequence))]
        print diff_sequence

        for k in range(int(math.log(word_size, 2)))[::-1]:
            if not [l for l in diff_sequence if l >= 2**k]:
                continue
            input_index_sequence = [m + 2**k if diff_sequence[l] >= 2**k else m for l, m in enumerate(input_index_sequence)]
            mask = mask_from_sequence(input_index_sequence)
            diff_sequence = [m - 2**k if m >= 2**k else m for l, m in enumerate(diff_sequence)]
            print 2**k, format(mask, '#x'), diff_sequence

def mask_from_sequence(index_sequence):
    mask = 0
    for i in index_sequence:
        mask |= 1 << i

    return mask
