#include "mex.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fec.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    /* Input checking. */
    if (nrhs != 1) {
        mexErrMsgIdAndTxt("encode_rs_viterbi:nrhs", "One input required.");
    }
    
    if (nlhs != 1) {
        mexErrMsgIdAndTxt("encode_rs_viterbi:nlhs", "One output required.");
    }
    
    if (!mxIsUint8(prhs[0])) {
        mexErrMsgIdAndTxt("encode_rs_viterbi:wrongInputType", "Input type must be uint8.");
    }
    
    if(mxGetNumberOfDimensions(prhs[0]) != 2) {
        mexErrMsgIdAndTxt("encode_rs_viterbi:wrongNumDimensions", "Input must be 2-dimensional.");
    }
    
    if(mxGetM(prhs[0]) != MESSAGE_DATA_LENGTH) {
        mexErrMsgIdAndTxt("encode_rs_viterbi:wrongDataLength", "Wrong number of input rows.");
    }
    
    const uint8_t *input_data = (const uint8_t *)mxGetData(prhs[0]);
    
    /* Length of full frame after FEC is applied. */
    mwSize packet_len = conv_get_encoded_len();
    
    /* Allocate output data. */
    plhs[0] = mxCreateNumericMatrix(packet_len, mxGetN(prhs[0]), mxUINT8_CLASS, mxREAL);
    
    /* Get pointer to data in output matrix. */
    uint8_t *output_data = (uint8_t *)mxGetData(plhs[0]);
    
    /* Iterate through input frames. */
    for (int i = 0; i < mxGetN(prhs[0]); i++) {
        uint8_t buf[256];
        
        /* Apply Reed-Solomon encoding. */
        rs_encode(&input_data[i*MESSAGE_DATA_LENGTH], buf);
        
        /* Apply convolutional coding. */
        conv_encode(buf, &output_data[i*packet_len]);
    }
}
