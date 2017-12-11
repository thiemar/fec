#include "mex.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fec.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    /* Input checking. */
    if (nrhs != 1) {
        mexErrMsgIdAndTxt("encode_polar:nrhs", "One input required.");
    }
    
    if (nlhs != 1) {
        mexErrMsgIdAndTxt("encode_polar:nlhs", "One output required.");
    }
    
    if (!mxIsUint8(prhs[0])) {
        mexErrMsgIdAndTxt("encode_polar:wrongInputType", "Input type must be uint8.");
    }
    
    if(mxGetNumberOfDimensions(prhs[0]) != 2) {
        mexErrMsgIdAndTxt("encode_polar:wrongNumDimensions", "Input must be 2-dimensional.");
    }
    
    if(mxGetM(prhs[0]) != POLAR_DATA_SIZE / 8u) {
        mexErrMsgIdAndTxt("encode_polar:wrongDataLength", "Wrong number of input rows.");
    }
    
    const uint8_t *input_data = (const uint8_t *)mxGetData(prhs[0]);
    
    /* Allocate output data. */
    plhs[0] = mxCreateNumericMatrix(POLAR_BLOCK_SIZE_SHORTENED / 8u, mxGetN(prhs[0]), mxUINT8_CLASS, mxREAL);
    
    /* Get pointer to data in output matrix. */
    uint8_t *output_data = (uint8_t *)mxGetData(plhs[0]);
    
    /* Iterate through input frames. */
    for (int i = 0; i < mxGetN(prhs[0]); i++) {
        uint8_t buf[POLAR_BLOCK_SIZE_SHORTENED / 8u];
        
        /* Apply convolutional coding. */
        polar_encode(&input_data[i*(POLAR_DATA_SIZE / 8u)], &output_data[i*(POLAR_BLOCK_SIZE_SHORTENED / 8u)]);
    }
}
