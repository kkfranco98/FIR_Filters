#include "basics/Sample.hpp"

template <typename V, size_t N>
class FIR_Filter_post_multiplication
{
private:
    Sample<V, uint64_t> _fir_samples[N];
    V _fir_coefficients[N];

    double _instantaneous_derivative[N];

    uint32_t _current_index, _previous_index, _computed_samples;

public:
    FIR_Filter_post_multiplication() : _current_index(0),
                                       _previous_index(N - 1)
    {
        for (size_t i = 0; i < N; i++)
        {
            _fir_coefficients[i] = 1.0 / N;
        }

        // Limit T a float o double
        static_assert(std::is_same<V, float>::value || std::is_same<V, double>::value,
                      "Circular_Array_FIR_Filter can only be instantiated with float or double");
    };

    FIR_Filter_post_multiplication(const V (&coefficients)[N]) : _current_index(0),
                                                                 _previous_index(N - 1)
    {
        for (size_t i = 0; i < N; i++)
        {
            _fir_coefficients[i] = coefficients[i];
        }

        // Limit T a float o double
        static_assert(std::is_same<V, float>::value || std::is_same<V, double>::value,
                      "Circular_Array_FIR_Filter can only be instantiated with float or double");
    };

    void add_sample(V value, uint64_t time, TimeUnit time_unit);

    bool filter_is_loaded();

    const V (&get_coefficients())[N] { return _fir_coefficients; }

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
    //         result += _fir_coefficients[i] * _instantaneous_derivative[(_current_index - i + N) % N];
    //     }
    //     return result;
    // }

private:
    uint64_t convert_to_microseconds(uint64_t time, TimeUnit unit);
};

#include "FIR_Filter_post_multiplication.tpp"