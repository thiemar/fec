% Polar code testbench.
N = 2048;
K = 1024;
M = 1536; % Length of shortened code.
design_snr = 5;
frozen = polar_construction(N, K, M, 'piecewise_integer', design_snr);

% Encode some random data.
rng(123);

% BPSK AWGN channel simulation.
sims = 100;
Eb_N0 = 0:0.5:4;
FER_coded = zeros(size(Eb_N0));
for i = 1:numel(Eb_N0)
    fprintf('Simulating Eb_N0 = %f dB\n', Eb_N0(i));
    for j = 1:sims
        data = randi([0 1], K, 1);
        data_encoded = polar_encode(N, K, data, frozen);
        channel_output = data_encoded(1:M)*2 - 1;
        channel_output_noisy = channel_output + randn(size(channel_output)) / ...
            sqrt(2*(10^(Eb_N0(i)/10)));

        channel_thresholded = ones(N, 1) * -1000;
        channel_thresholded(1:M) = round(min(max((channel_output_noisy + 1) / 2, 0), 1));

        % Decode the encoded data.
        decoded = polar_decode(N, K, channel_thresholded, frozen);
        
        if ~isequal(decoded, data)
            FER_coded(i) = FER_coded(i) + (1/sims);
        end
    end
end

% Make FER plot.
semilogy(Eb_N0, FER_coded); hold on; grid on; grid minor;
legend(sprintf('PC(%d, %d)', M, K));