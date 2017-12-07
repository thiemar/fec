N = 1024;
K = 512;
design_snr = 5;

test_data = uint8([...
    228, 245, 140,  35,  38,  65, 215,  65, ...
    208,  62, 237,  89,  50,  64, 157, 121, ...
     90, 212, 149, 140, 234,  73, 193, 192, ...
     97, 145,  19,  13, 135, 199, 239,  33, ...
    145, 120,   3,  86,  41, 203,  79, 135, ...
     42, 154,  67, 167, 176, 191, 115,  21, ...
     58, 233,  39, 211, 137, 255,  20, 113, ...
     27, 246,   1, 198, 209, 222,  21, 102 ...
]);
test_bitsequence = reshape(de2bi(test_data, 'left-msb')', [], 1);
frozen = polar_construction(N, K, 'piecewise_integer', design_snr);

test_bitsequence_encoded = polar_encode(N, K, test_bitsequence, frozen);

test_data_encoded = bi2de(uint8(reshape(test_bitsequence_encoded, 8, [])'), 'left-msb');