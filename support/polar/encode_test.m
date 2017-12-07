N = 128;
K = 64;
design_snr = 5;

test_data = uint8([61 219 159 102 183 50 205 93]);
test_bitsequence = reshape(de2bi(test_data, 'left-msb')', [], 1);
frozen = polar_construction(N, K, 'piecewise_integer', design_snr);

test_bitsequence_encoded = polar_encode(N, K, test_bitsequence, frozen);

test_data_encoded = bi2de(uint8(reshape(test_bitsequence_encoded, 8, [])'), 'left-msb');