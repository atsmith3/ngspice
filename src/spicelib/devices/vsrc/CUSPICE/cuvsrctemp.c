/*
 * Copyright (c) 2014, NVIDIA Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation and/or 
 *    other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to 
 *    endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ngspice/config.h"
#include "cuda_runtime_api.h"
#include "vsrcdefs.h"
#include "ngspice/CUSPICE/CUSPICE.h"

/* cudaMemcpy MACRO to check it for errors --> CUDAMEMCPYCHECK(name of pointer, dimension, type, status) */
#define CUDAMEMCPYCHECK(a, b, c, d) \
    if (d != cudaSuccess) \
    { \
        fprintf (stderr, "cuVSRCtemp routine...\n") ; \
        fprintf (stderr, "Error: cudaMemcpy failed on %s size1 of %d bytes\n", #a, (int)(b * sizeof(c))) ; \
        fprintf (stderr, "Error: %s = %d, %s\n", #d, d, cudaGetErrorString (d)) ; \
        return (E_NOMEM) ; \
    }

int
cuVSRCtemp
(
GENmodel *inModel
)
{
    int i ;
    long unsigned int size1, size2 ;
    cudaError_t status ;
    VSRCmodel *model = (VSRCmodel *)inModel ;
    VSRCinstance *here ;

    size1 = (long unsigned int) model->gen.GENnInstances;

    i = 0 ;

    for (here = VSRCinstances(model); here != NULL ; here = VSRCnextInstance(here))
    {
        size2 = (long unsigned int)here->n_coeffs ;
        status = cudaMemcpy (model->VSRCparamCPU.VSRCcoeffsArray [i], model->VSRCparamCPU.VSRCcoeffsArrayHost [i], size2 * sizeof(double), cudaMemcpyHostToDevice) ;
        CUDAMEMCPYCHECK (model->VSRCparamCPU.VSRCcoeffsArray [i], size2, double, status)

        i++ ;
    }

    /* DOUBLE */
    status = cudaMemcpy (model->VSRCparamGPU.d_VSRCdcvalueArray, model->VSRCparamCPU.VSRCdcvalueArray, size1 * sizeof(double), cudaMemcpyHostToDevice) ;
    CUDAMEMCPYCHECK(model->VSRCparamGPU.d_VSRCdcvalueArray, size1, double, status)

    status = cudaMemcpy (model->VSRCparamGPU.d_VSRCrdelayArray, model->VSRCparamCPU.VSRCrdelayArray, size1 * sizeof(double), cudaMemcpyHostToDevice) ;
    CUDAMEMCPYCHECK(model->VSRCparamGPU.d_VSRCrdelayArray, size1, double, status)

    /* INT */
    status = cudaMemcpy (model->VSRCparamGPU.d_VSRCdcGivenArray, model->VSRCparamCPU.VSRCdcGivenArray, size1 * sizeof(int), cudaMemcpyHostToDevice) ;
    CUDAMEMCPYCHECK(model->VSRCparamGPU.d_VSRCdcGivenArray, size1, int, status)

    status = cudaMemcpy (model->VSRCparamGPU.d_VSRCfunctionTypeArray, model->VSRCparamCPU.VSRCfunctionTypeArray, size1 * sizeof(int), cudaMemcpyHostToDevice) ;
    CUDAMEMCPYCHECK(model->VSRCparamGPU.d_VSRCfunctionTypeArray, size1, int, status)

    status = cudaMemcpy (model->VSRCparamGPU.d_VSRCfunctionOrderArray, model->VSRCparamCPU.VSRCfunctionOrderArray, size1 * sizeof(int), cudaMemcpyHostToDevice) ;
    CUDAMEMCPYCHECK(model->VSRCparamGPU.d_VSRCfunctionOrderArray, size1, int, status)

    status = cudaMemcpy (model->VSRCparamGPU.d_VSRCrGivenArray, model->VSRCparamCPU.VSRCrGivenArray, size1 * sizeof(int), cudaMemcpyHostToDevice) ;
    CUDAMEMCPYCHECK(model->VSRCparamGPU.d_VSRCrGivenArray, size1, int, status)

    status = cudaMemcpy (model->VSRCparamGPU.d_VSRCrBreakptArray, model->VSRCparamCPU.VSRCrBreakptArray, size1 * sizeof(int), cudaMemcpyHostToDevice) ;
    CUDAMEMCPYCHECK(model->VSRCparamGPU.VSRCrBreakptArray, size1, int, status)

    return (OK) ;
}
