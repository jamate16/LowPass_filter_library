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
#pragma once

#include <cmath>

#include "pico/stdlib.h"


template<int order> // order is 1 or 2
class LowPass {
private:
    float a[order]; // Denumerator coefficients
    float b[order+1]; // Numerator coefficients
    float omega_c; // Cut-off angular frequency of the filter
    float delta_time; // Time between filter() function calls

    float x[order+1]; // Raw values
    float y[order+1]; // Filtered values

    bool adaptive; // If true, recalculates the filter coefficients in every call to filter
    absolute_time_t last_time; // Variable to store last time the filter loop was run in case of adaptative

public:
    /**
     * @brief Construct a LowPass filter object. In the code of a RP2040 board using the PICO SDK
     * 
     * @param fc Cut-off frequency [-3 dB] of the filter
     * @param Ts Sampling time (this has to be as precise as possible in the case of not using adaptative for the discrete time transfor to accurately represent the filter)
     * @param adaptive false by default. If true the code with set the sample frequency based on elapsed time between filter() function calls
     */
    LowPass(float fc, float Ts, bool adaptive=false);

    /**
     * @brief Set the Coefficients of the filter based on the cut-off frequency of the filter. In case the filter is adaptative, this function is called with every filter() call.
     * 
     */
    void setCoefficients();

    /**
     * @brief Filter the sample
     * 
     * This function filters the process variable based on the cut-off frequency of the filter and returns the result.
     * 
     * @param unf_pv Unfiltered process variable
     * @return float Filtered process variable
     */
    float filter(float unf_pv);
};