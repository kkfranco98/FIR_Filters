#pragma once

#include "basics/Sample.hpp"
#include "basics/check_type.hpp"
#include <RingBuffer.h>

namespace FIR_Filters
{
    template <typename V, size_t N>
    class FIR_Filter_post_multiplication
    {
    public:
        using value_type = V;

    private:
        Ring_Buffer<Sample<V, uint64_t>, N> _fir_samples;
        V _fir_coefficients[N];

        Ring_Buffer<double, N> _instantaneous_derivatives;

    public:
        FIR_Filter_post_multiplication()
        {
            for (size_t i = 0; i < N; i++)
            {
                _fir_coefficients[i] = 1.0 / N;
            }

            // Limit T a float o double
            static_assert(is_same_custom<V, float>::value || is_same_custom<V, double>::value,
                          "Circular_Array_FIR_Filter can only be instantiated with float or double");
        };

        FIR_Filter_post_multiplication(const V (&coefficients)[N])
        {
            for (size_t i = 0; i < N; i++)
            {
                _fir_coefficients[i] = coefficients[i];
            }

            // Limit T a float o double
            static_assert(is_same_custom<V, float>::value || is_same_custom<V, double>::value,
                          "Circular_Array_FIR_Filter can only be instantiated with float or double");
        };

        void add_sample(V value, uint64_t time, TimeUnit time_unit);

        bool filter_is_loaded();

        const V (&get_coefficients()) [N]
        { return _fir_coefficients; }

        Sample<V, uint64_t> get_last_sample();
        V get_last_value();
        V get_filtered_value();
        V get_instantaneous_derivative();
        V get_average_derivative();
        void reset();

        //! (DON'T USE) the derivative filter coefficients must be different. Probably i will remove it
        // V get_filtered_derivative()
        // {
        //     V result = 0;
        //     for (uint32_t i = 0; i < N; i++)
        //     {
        //         result += _fir_coefficients[i] * _instantaneous_derivatives.at(_instantaneous_derivatives.size() - 1 - i);
        //     }
        //     return result;
        // }

    private:
        uint64_t convert_to_microseconds(uint64_t time, TimeUnit unit);
    };

}

#include "FIR_Filter_post_multiplication.tpp"
