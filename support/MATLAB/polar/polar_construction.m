% Polar code construction via Bhattacharyya bounds.
% Valid choices of 'method' are:
% - 'linear'
% - 'log'
% - 'piecewise'
% - 'piecewise_integer'
function [frozen, z, I] = polar_construction(N, K, M, method, design_snr)
    assert(log2(N) == round(log2(N)), 'N must be a power of two');
    assert(K <= N, 'k must be smaller than or equal to N');
    assert(M <= N && M >= K, 'M must be between N and K');
    
    if nargin < 4
        design_snr = 0;
    end
    
    if nargin < 3
        method = 'linear';
    end
    
    S = 10^(design_snr / 10); % Initial channel B-parameter.
    n = log2(N); % Number of recursions.
    z = zeros(N, 1); % Array of bit B-parameters.
    
    if strcmp(method, 'linear')
        z(1) = exp(-S); % Set initial channel B-parameter.
        for i = 1:n
            u = 2^i;
            for j = 1:u/2
                T = z(j);
                z(j) = 2*T - T^2;
                z(j + u/2) = T^2;
            end
            
            z = reshape([z(1:u/2)'; z(u/2+1:u)'], 1, [])';
        end

        [z, I] = sort(z);
    elseif strcmp(method, 'log')
        z(1) = 0.5*log(exp(-S) / (1 - exp(-S)));
        for i = 1:n
            u = 2^i;
            for j = 1:u/2
                T = z(j);
                z(j) = T + 0.5*log(2 + exp(2*T));
                z(j + u/2) = 2*T - 0.5*log(1 + 2*exp(2*T));
            end
            
            z = reshape([z(1:u/2)'; z(u/2+1:u)'], 1, [])';
        end

        [z, I] = sort(z);
    elseif strcmp(method, 'piecewise')
        z(1) = 0.5*log(exp(-S) / (1 - exp(-S)));
        for i = 1:n
            u = 2^i;
            for j = 1:u/2
                T = z(j);

                % Upper bound.
                if T > log(2)/2
                    z(j) = 2*T;
                else
                    z(j) = T + log(2)/2;
                end

                % Lower bound.
                if T > -log(2)/2
                    z(j + u/2) = T - log(2)/2;
                else
                    z(j + u/2) = 2*T;
                end
            end
            
            z = reshape([z(1:u/2)'; z(u/2+1:u)'], 1, [])';
        end

        [z, I] = sort(z);
    elseif strcmp(method, 'piecewise_integer')
        z = zeros(N, 1, 'int32');
        z(1) = int32(0.5*log(exp(-S) / (1 - exp(-S))));
        for i = 1:n
            u = 2^i;
            for j = 1:u/2
                T = z(j);

                % Upper bound.
                if T > 1
                    z(j) = 2*T;
                else
                    z(j) = T + 1;
                end

                % Lower bound.
                if T > -1
                    z(j + u/2) = T - 1;
                else
                    z(j + u/2) = 2*T;
                end
            end
            
            z = reshape([z(1:u/2)'; z(u/2+1:u)'], 1, [])';
        end

        [z, I] = sort(z);
    else
        error('Unrecognised construction method');
    end
    
    % Work out shortening.
    shortened = (M+1):N;
    [~, ia, ~] = intersect(I, shortened);
    I(ia) = [];
    
    frozen = false(N, 1);
    frozen([I(K+1:end); shortened']) = true;
end