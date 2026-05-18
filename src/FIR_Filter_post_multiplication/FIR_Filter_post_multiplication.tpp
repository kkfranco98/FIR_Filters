
namespace FIR_Filters
{
    template <typename V, size_t N>
    void FIR_Filter_post_multiplication<V, N>::add_sample(V value, uint64_t time, TimeUnit time_unit)
    {
        Sample<V, uint64_t> new_sample;
        new_sample.value = value;
        new_sample.time_in_micros = convert_to_microseconds(time, time_unit);

        double derivative = 0.0;

        if (!_fir_samples.empty())
        {
            const auto &previous_sample = _fir_samples.newest_element();
            const int64_t delta_time =
                static_cast<int64_t>(new_sample.time_in_micros) -
                static_cast<int64_t>(previous_sample.time_in_micros);

            if (delta_time < 0)
            {
                Serial.print("\x1b[31m ERROR!: negative delta time \x1b[37m");
                derivative = _instantaneous_derivatives.empty() ? 0.0 : _instantaneous_derivatives.newest_element();
            }
            else if (delta_time > 0)
            {
                derivative =
                    ((static_cast<double>(new_sample.value) - static_cast<double>(previous_sample.value)) /
                     static_cast<double>(delta_time)) *
                    1e6;
            }
            else if (delta_time == 0)
            {
                // zero-order hold
                derivative = _instantaneous_derivatives.empty() ? 0.0 : _instantaneous_derivatives.newest_element();
            }
        }

        _fir_samples.push_back(new_sample);
        _instantaneous_derivatives.push_back(derivative);
    }

    template <typename V, size_t N>
    bool FIR_Filter_post_multiplication<V, N>::filter_is_loaded()
    {
        return _fir_samples.is_loaded();
    }

    template <typename V, size_t N>
    Sample<V, uint64_t> FIR_Filter_post_multiplication<V, N>::get_last_sample()
    {
        return _fir_samples.newest_element();
    }

    template <typename V, size_t N>
    V FIR_Filter_post_multiplication<V, N>::get_last_value()
    {
        return _fir_samples.newest_element().value;
    }

    template <typename V, size_t N>
    V FIR_Filter_post_multiplication<V, N>::get_filtered_value()
    {
        V result = 0;
        const size_t sample_count = _fir_samples.size();
        for (size_t i = 0; i < sample_count; i++)
        {
            result += _fir_coefficients[i] * _fir_samples.at(sample_count - 1 - i).value;
        }
        return result;
    }

    template <typename V, size_t N>
    V FIR_Filter_post_multiplication<V, N>::get_instantaneous_derivative()
    {
        return static_cast<V>(_instantaneous_derivatives.newest_element());
    }

    template <typename V, size_t N>
    V FIR_Filter_post_multiplication<V, N>::get_average_derivative()
    {
        V result = 0;
        const size_t derivative_count = _instantaneous_derivatives.size();
        for (size_t i = 0; i < derivative_count; i++)
        {
            result += static_cast<V>(_instantaneous_derivatives.at(i));
        }
        return (derivative_count > 0) ? (result / static_cast<V>(derivative_count)) : 0;
    }

    template <typename V, size_t N>
    void FIR_Filter_post_multiplication<V, N>::reset()
    {
        _fir_samples = Ring_Buffer<Sample<V, uint64_t>, N>();
        _instantaneous_derivatives = Ring_Buffer<double, N>();
    }

    template <typename V, size_t N>
    uint64_t FIR_Filter_post_multiplication<V, N>::convert_to_microseconds(uint64_t time, TimeUnit unit)
    {
        switch (unit)
        {
        case TimeUnit::SECONDS:
            return time * 1e6;
        case TimeUnit::MILLISECONDS:
            return time * 1e3;
        default:
            return time;
        }
    }

}
