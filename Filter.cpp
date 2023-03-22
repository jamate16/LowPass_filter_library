/*
MIT License

Copyright (c) 2020, curiores. 2023 jamate16.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "Filter.h"


// Explicit instantiations, this also makes sure LowPass will only be able to be used with order 1 and order 2
template class LowPass<1>;
template class LowPass<2>;

template<int order>
LowPass<order>::LowPass(float fc, float Ts, bool adaptive) 
    : omega_c {6.28318530718f*fc/*Convertion from frequency[hz] to angular requency [rad/s]*/},
        delta_time {Ts},
        adaptive {adaptive} {

    // Initialize the input and output arrays to zero
    for(int k = 0; k < order+1; k++){
        x[k] = 0;
        y[k] = 0;
    }

    // Set coeffs based on the cut-off frequency
    setCoefficients();

    // Get initial time for delta time calculation later on in filter() function
    last_time = get_absolute_time();
}

template<int order>
void LowPass<order>::setCoefficients(){
    if(adaptive){
        // Calculate elapsed time since last function call
        absolute_time_t current_time = get_absolute_time();
        delta_time = absolute_time_diff_us(last_time, current_time)/1e6f;
        last_time = current_time;
    }

    // Update coefficients based on filter order
    float alpha = omega_c*delta_time; // Recurrent constant for discrete time filter's constants calculation
    if (order==1) {
        // Coefficients based on first order filter discretized using Bilinear approximation
        a[0] = -(alpha - 2.0) / (alpha + 2.0);
        b[0] = alpha / (alpha + 2.0);
        b[1] = alpha / (alpha + 2.0);

    } else if (order==2) {
        float beta[] = {1, sqrt(2), 1}; // Normalized butterworth polynomial coefficients

        // Coefficients based on non-normalized butterworth second order filter discretized using Bilinear approximation
        float alphaSq = alpha*alpha;
        float D = alphaSq*beta[0] + 2*alpha*beta[1] + 4*beta[2];
        b[0] = alphaSq/D;
        b[1] = 2*b[0];
        b[2] = b[0];
        a[0] = -(2*alphaSq*beta[0] - 8*beta[2])/D;
        a[1] = -(beta[0]*alphaSq - 2*beta[1]*alpha + 4*beta[2])/D;      
    }
}

template<int order>
float LowPass<order>::filter(float unf_pv) {
    if (adaptive) {
        setCoefficients(); // Update coefficients if necessary      
    }

    y[0] = 0;
    x[0] = unf_pv;
    // Compute the filtered values in a recursive manner
    for(int k=0; k < order; k++){
        y[0] += a[k]*y[k+1] + b[k]*x[k];
    }
    y[0] += b[order]*x[order];

    // Shift "registers" for next iteration
    for(int k=order; k > 0; k--){
        y[k] = y[k-1];
        x[k] = x[k-1];
    }

    // Return the filtered value    
    return y[0];
}