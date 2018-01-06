% FEC code testbench.
fec_test_params = get_fec_test_parameters();
cd('mex');
make_fec_mex(fec_test_params);
cd('..');
addpath('mex');

% Seed the random number generator.
rng(123);

% BPSK AWGN channel simulation.
Eb_N0 = 0:0.5:2;

if fec_test_params.test_polar
    addpath('polar');
    FER_polar = zeros(size(Eb_N0));
    frozen = polar_construction(fec_test_params.polar_block_size, ...
        fec_test_params.polar_data_size, fec_test_params.polar_block_size_shortened, ...
        'piecewise_integer', 5);
end

if fec_test_params.test_rs_viterbi
    FER_rs_viterbi = zeros(size(Eb_N0));
end

for i = 1:numel(Eb_N0)
    fprintf('Simulating Eb_N0 = %f dB\n', Eb_N0(i));
    for j = 1:fec_test_params.num_sims
        % RS/Viterbi test.
        if fec_test_params.test_rs_viterbi
            data = uint8(randi([0 255], fec_test_params.rs_viterbi_message_data_length, 1));
            encoded = encode_rs_viterbi(data);
            channel_output = double(reshape(de2bi(encoded, 8, 'left-msb')', [], 1))*2 - 1;
            
            channel_output_noisy = channel_output + randn(size(channel_output)) / ...
                sqrt(2*(10^(Eb_N0(i)/10)));

            channel_thresholded = round(min(max((channel_output_noisy + 1) / 2, 0), 1));
            
            % Decode the encoded data.
            decoded = decode_rs_viterbi(...
                bi2de(uint8(reshape(channel_thresholded, 8, [])'), 'left-msb'));
            
            if ~isequal(decoded, data)
                FER_rs_viterbi(i) = FER_rs_viterbi(i) + (1/fec_test_params.num_sims);
            end
        end

        % Polar test.
        if fec_test_params.test_polar
            data = uint8(randi([0 255], fec_test_params.polar_data_size / 8, 1));
            encoded = encode_polar(data);
            channel_output = double(reshape(de2bi(encoded, 8, 'left-msb')', [], 1))*2 - 1;

            channel_output_noisy = channel_output + randn(size(channel_output)) / ...
                sqrt(2*(10^(Eb_N0(i)/10)));

            channel_thresholded = round(min(max((channel_output_noisy + 1) / 2, 0), 1));

            % Decode the encoded data.
            decoded_bitsequence = polar_decode(fec_test_params.polar_block_size, ...
                fec_test_params.polar_data_size, ...
                [channel_thresholded; zeros(fec_test_params.polar_block_size - ...
                fec_test_params.polar_block_size_shortened, 1)], frozen);
            decoded = bi2de(uint8(reshape(decoded_bitsequence, 8, [])'), 'left-msb');
            
            if ~isequal(decoded, data)
                FER_polar(i) = FER_polar(i) + (1/fec_test_params.num_sims);
            end
        end
    end
end

% Make FER plot.
legend_entries = {};
if fec_test_params.test_rs_viterbi
    semilogy(Eb_N0, FER_rs_viterbi); hold on; grid on; grid minor;
    legend_entries{end+1} = sprintf('RS(%d, %d) + Convolutional k=7', ...
        fec_test_params.rs_viterbi_message_data_length + fec_test_params.rs_viterbi_message_parity_length, ...
        fec_test_params.rs_viterbi_message_data_length);
end

if fec_test_params.test_polar
    semilogy(Eb_N0, FER_polar); hold on; grid on; grid minor;
    legend_entries{end+1} = sprintf('Polar(%d, %d)', ...
        fec_test_params.polar_block_size_shortened, ...
        fec_test_params.polar_data_size);
end

legend(legend_entries{:});
