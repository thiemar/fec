#include "mex.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "correct.h"
#include "fec.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    /* Input checking. */
    if (nrhs != 1) {
        mexErrMsgIdAndTxt("decode_rs_viterbi:nrhs", "One input required.");
    }
    
    if (nlhs != 1) {
        mexErrMsgIdAndTxt("decode_rs_viterbi:nlhs", "One output required.");
    }
    
    if (!mxIsUint8(prhs[0])) {
        mexErrMsgIdAndTxt("decode_rs_viterbi:wrongInputType", "Input type must be uint8.");
    }
    
    if(mxGetNumberOfDimensions(prhs[0]) != 2) {
        mexErrMsgIdAndTxt("decode_rs_viterbi:wrongNumDimensions", "Input must be 2-dimensional.");
    }
    
    const uint8_t *input_data = (const uint8_t *)mxGetData(prhs[0]);
    
    /* Set up Reed-Solomon decoder. */
    correct_reed_solomon *rs_enc = correct_reed_solomon_create(
        correct_rs_primitive_polynomial_8_4_3_2_0, 1, 1, MESSAGE_PARITY_LENGTH);
    
    /* Length of full frame after FEC is applied. */
    mwSize packet_len = conv_get_encoded_len();
        
    if(mxGetM(prhs[0]) != packet_len) {
        mexErrMsgIdAndTxt("decode_rs_viterbi:wrongDataLength", "Wrong number of input rows.");
    }
    
    /* Allocate output data. */
    plhs[0] = mxCreateNumericMatrix(MESSAGE_DATA_LENGTH, mxGetN(prhs[0]), mxUINT8_CLASS, mxREAL);
    
    /* Get pointer to data in output matrix. */
    uint8_t *output_data = (uint8_t *)mxGetData(plhs[0]);
    
    /* Iterate through input frames. */
    for (int i = 0; i < mxGetN(prhs[0]); i++) {
        uint8_t buf[255];
        
        /* Decode convolutional coding. */
        conv_decode(&input_data[i*packet_len], buf);
        
        /* Decode Reed-Solomon. */
        ssize_t m = correct_reed_solomon_decode(rs_enc, buf,
            MESSAGE_DATA_LENGTH + MESSAGE_PARITY_LENGTH,
            &output_data[i*MESSAGE_DATA_LENGTH]);
    }
}
