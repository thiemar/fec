% Do successive-cancellation polar decoding.
% Follows the SC algorithm described in:
% https://arxiv.org/pdf/1504.00353.pdf
function data = polar_decode(N, K, encoded, frozen)
    assert(log2(N) == round(log2(N)), 'N must be a power of two');
    assert(K <= N, 'k must be smaller than or equal to N');
    assert(isequal(size(encoded), [N 1]), 'Encoded data must be a column vector of N bits');
    assert(isequal(size(frozen), [N 1]), 'Array of frozen indices must have N elements');
    assert(sum(frozen) == N - K, 'Array of frozen indices must have K non-zero elements');
    assert(islogical(frozen), 'Frozen indices must be of type logical');

    n = log2(N); % Number of recursions.
    alpha = (-ones(N, 1)*2 .* encoded) + 1; % Initialise LLRs using the input data.
    
    % Recurse down and then back up the tree to get the bit estimates.
    [decoded, ~] = traverse(n, alpha, frozen);
    
    data = decoded(~frozen);
end

% Traverse the code tree.
function [u, beta] = traverse(nv, alpha, frozen)
    if nv == 0
        % Leaf node.
        if frozen || alpha >= 0
            beta = false;
        else
            beta = true;
        end
        
        u = beta;
    else
        [u_l, beta_l] = traverse(nv-1, ...
            f(alpha(1:2^(nv-1)), alpha(2^(nv-1)+1:end)), ...
            frozen(1:2^(nv-1))); % Left-traversal.
        [u_r, beta_r] = traverse(nv-1, ...
            g(alpha(1:2^(nv-1)), alpha(2^(nv-1)+1:end), beta_l), ...
            frozen(2^(nv-1)+1:end)); % Right-traversal.
        
        % Estimate systematic codeword.
        u = [u_l; u_r];
        
        % Make the bit-decisions.
        beta = [mod(beta_l + beta_r, 2); beta_r];
    end
end

% Do the f-operation (min-sum).
function alpha = f(a1, a2)
    alpha = sign(a1).*sign(a2).*min(abs(a1), abs(a2));
end

% Do the g-operation.
function alpha = g(a1, a2, b)
    alpha = a2 + (1 - 2*double(b)).*a1;
end
