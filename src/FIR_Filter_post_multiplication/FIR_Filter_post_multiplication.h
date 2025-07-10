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
        for (uint32_t i = 0; i < N; i++)
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
        for (uint32_t i = 0; i < N; i++)
        {
            _fir_coefficients[i] = coefficients[i];
        }
        // Limit T a float o double
        static_assert(std::is_same<V, float>::value || std::is_same<V, double>::value,
                      "Circular_Array_FIR_Filter can only be instantiated with float or double");
    };

    void add_sample(V value, uint64_t time, TimeUnit time_unit)
    {
        _fir_samples[_current_index].value = value;
        _fir_samples[_current_index].time_in_micros = convert_to_microseconds(time, time_unit);

        V delta_time = (V)_fir_samples[_current_index].time_in_micros - (V)_fir_samples[_previous_index].time_in_micros;

        if (delta_time < 0)
        {
            Serial.print("\x1b[31m ERROR!: negative delta time \x1b[37m");
        }
        else
        {
            if (delta_time > 0)
            {
                _instantaneous_derivative[_current_index] =
                    ((_fir_samples[_current_index].value - _fir_samples[_previous_index].value) / delta_time) * 1e6;
            }
            else if (delta_time == 0)
            {
                // zero-order hold
                _instantaneous_derivative[_current_index] = _instantaneous_derivative[_previous_index];
            }

            if (_computed_samples < N)
                _computed_samples++;
        }

        _previous_index = _current_index;
        _current_index = (_current_index + 1) % N;
    }

    bool filter_is_loaded() { return _computed_samples >= N; }

    const V (&get_coefficients())[N] { return _fir_coefficients; }

    Sample<V, uint64_t> get_last_sample() { return _fir_samples[_current_index]; }

    V get_last_value() { return _fir_samples[_current_index].value; }

    V get_filtered_value()
    {
        V result = 0;
        for (uint32_t i = 0; i < N; i++)
        {
            result += _fir_coefficients[i] * _fir_samples[(_current_index - i + N) % N].value;
        }
        return result;
    }

    V get_instantaneous_derivative()
    {
        return _instantaneous_derivative[_previous_index];
    }

    V get_average_derivative()
    {
        V result = 0;
        for (uint32_t i = 0; i < N; i++)
        {
            result += _instantaneous_derivative[i];
        }
        return result / static_cast<V>(N);
    }

    V get_filtered_derivative()
    {
        V result = 0;
        for (uint32_t i = 0; i < N; i++)
        {
            result += _fir_coefficients[i] * _instantaneous_derivative[(_current_index - i + N) % N];
        }
        return result;
    }

private:
    uint64_t convert_to_microseconds(uint64_t time, TimeUnit unit)
    {
        switch (unit)
        {
        case TimeUnit::SECONDS:
            return time * 1000000;
        case TimeUnit::MILLISECONDS:
            return time * 1000;
        default:
            return time;
        }
    }
};
