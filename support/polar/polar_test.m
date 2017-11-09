% Polar code testbench.
N = 2^10;
K = N/2;
design_snr = 0;
frozen = polar_construction(N, K, 'log', design_snr);

% Encode some random data.
rng(123);

% BPSK AWGN channel simulation.
sims = 1000;
Eb_N0 = 0:0.5:4;
FER_coded = zeros(size(Eb_N0));
FER_uncoded = zeros(size(Eb_N0));
for i = 1:numel(Eb_N0)
    fprintf('Simulating Eb_N0 = %f dB\n', Eb_N0(i));
    for j = 1:sims
        data = randi([0 1], K, 1);
        channel_output = polar_encode(N, K, data, frozen)*2 - 1;
        channel_output_noisy = channel_output + randn(size(channel_output)) / ...
            sqrt(2*(10^(Eb_N0(i)/10)));

        channel_thresholded = round(min(max((channel_output_noisy + 1) / 2, 0), 1));

        % Decode the encoded data.
        decoded = polar_decode(N, K, channel_thresholded, frozen);
        
        if ~isequal(decoded, data)
            FER_coded(i) = FER_coded(i) + (1/sims);
        end
        
        if ~isequal(channel_thresholded(~frozen), (channel_output(~frozen) + 1) / 2)
            FER_uncoded(i) = FER_uncoded(i) + (1/sims);
        end
    end
end

% Make FER plot.
semilogy(Eb_N0, FER_uncoded); hold on; grid on; grid minor;
semilogy(Eb_N0, FER_coded);
legend('Uncoded', sprintf('PC(%d, %d)', N, K));