function make_fec_mex(test_params)
    if exist('./build', 'dir')
        rmdir('./build', 's');
    end
    mkdir('./build');
    
    includes = { ...
        ['-I' fullfile('libcorrect', 'include')] ...
        ['-I' fullfile('..', '..', '..', 'include')] ...
    };

    if nargin < 1
        test_params = get_fec_test_parameters;
    end

    defines = { ...
        sprintf('-DMESSAGE_DATA_LENGTH=%du', test_params.rs_viterbi_message_data_length) ...
        sprintf('-DMESSAGE_PARITY_LENGTH=%du', test_params.rs_viterbi_message_parity_length) ...
        sprintf('-DPOLAR_BLOCK_SIZE=%du', test_params.polar_block_size) ...
        sprintf('-DPOLAR_BLOCK_SIZE_SHORTENED=%du', test_params.polar_block_size_shortened) ...
        sprintf('-DPOLAR_DATA_SIZE=%du', test_params.polar_data_size) ...
    };

    cxxflags = 'CXXFLAGS=-std=c++14 -Wno-ignored-attributes -Wno-deprecated-declarations';
    
    if ispc()
        ldflags = 'LDFLAGS=-static-libgcc -static-libstdc++ -static -lpthread';
    else ismac()
        ldflags = '';
    end
    
    sourceFiles = { ...
        'libcorrect/src/reed-solomon/polynomial.c' ...
        'libcorrect/src/reed-solomon/reed-solomon.c' ...
        'libcorrect/src/reed-solomon/encode.c' ...
        'libcorrect/src/reed-solomon/decode.c' ...
    };

    mkdir('./build/rs');
    mex('-v', '-c', cxxflags, ldflags, '-largeArrayDims', ...
        '-outdir', './build/rs', includes{:}, sourceFiles{:}, defines{:});
    
    sourceFiles = { ...
        'fec_wrapper.cpp' ...
    };

    mkdir('./build/fec_wrapper');
    mex('-v', '-c', cxxflags, ldflags, '-largeArrayDims', ...
        '-outdir', './build/fec_wrapper', includes{:}, sourceFiles{:}, defines{:});
    
    sourceFiles = { ...
        'encode_rs_viterbi.c' ...
        './build/fec_wrapper/fec_wrapper.o'
    };

    mex('-v', cxxflags, ldflags, '-largeArrayDims', includes{:}, sourceFiles{:}, defines{:});
    
    sourceFiles = { ...
        'decode_rs_viterbi.c' ...
        './build/rs/polynomial.o' ...
        './build/rs/reed-solomon.o' ...
        './build/rs/encode.o' ...
        './build/rs/decode.o' ...
        './build/fec_wrapper/fec_wrapper.o'
    };

    mex('-v', cxxflags, ldflags, '-largeArrayDims', includes{:}, sourceFiles{:}, defines{:});

    sourceFiles = { ...
        'encode_polar.c' ...
        './build/fec_wrapper/fec_wrapper.o'
    };

    mex('-v', cxxflags, ldflags, '-largeArrayDims', includes{:}, sourceFiles{:}, defines{:});
end