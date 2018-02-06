% Do polar encoding.
function encoded_systematic = polar_encode(N, K, data, frozen)
    assert(log2(N) == round(log2(N)), 'N must be a power of two');
    assert(K <= N, 'k must be smaller than or equal to N');
    assert(isequal(size(data), [K 1]), 'Data must be a column vector of K bits');
    assert(isequal(size(frozen), [N 1]), 'Array of frozen indices must have N elements');
    assert(sum(frozen) == N - K, 'Array of frozen indices must have K non-zero elements');
    assert(islogical(frozen), 'Frozen indices must be of type logical');

    n = log2(N); % Number of recursions.
    
    u = zeros(N, 1);
    u(~frozen) = data;

    % Create generator matrix.
    G = [1 0; 1 1];
    F = G;
    for i = 1:n-1
        F = kron(F, G);
    end
    
    encoded = mod(u'*F, 2)';
    
    % Extra step to do systematic encoding.
    encoded(frozen) = 0;
    encoded_systematic = mod(encoded'*F, 2)';
end