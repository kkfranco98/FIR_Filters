

template <typename V, size_t N>
void FIR_Filter_post_multiplication<V, N>::add_sample(V value, uint64_t time, TimeUnit time_unit)
{
    _fir_samples[_current_index].value = value;
    _fir_samples[_current_index].time_in_micros = convert_to_microseconds(time, time_unit);

    V delta_time = _fir_samples[_current_index].time_in_micros - _fir_samples[_previous_index].time_in_micros;

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

template <typename V, size_t N>
bool FIR_Filter_post_multiplication<V, N>::filter_is_loaded()
{
    return _computed_samples >= N;
}

template <typename V, size_t N>
Sample<V, uint64_t> FIR_Filter_post_multiplication<V, N>::get_last_sample()
{
    return _fir_samples[_current_index];
}

template <typename V, size_t N>
V FIR_Filter_post_multiplication<V, N>::get_last_value()
{
    return _fir_samples[_current_index].value;
}

template <typename V, size_t N>
V FIR_Filter_post_multiplication<V, N>::get_filtered_value()
{
    V result = 0;
    for (size_t i = 0; i < N; i++)
    {
        result += _fir_coefficients[i] * _fir_samples[(_current_index - i + N) % N].value;
    }
    return result;
}

template <typename V, size_t N>
V FIR_Filter_post_multiplication<V, N>::get_instantaneous_derivative()
{
    return _instantaneous_derivative[_previous_index];
}

template <typename V, size_t N>
V FIR_Filter_post_multiplication<V, N>::get_average_derivative()
{
    V result = 0;
    for (size_t i = 0; i < N; i++)
    {
        result += _instantaneous_derivative[i];
    }
    return result / static_cast<V>(N);
}

template <typename V, size_t N>
void FIR_Filter_post_multiplication<V, N>::reset()
{
    _current_index = 0;
    _previous_index = N - 1;
    _computed_samples = 0;

    for (size_t i = 0; i < N; i++)
    {
        _fir_samples[i].value = 0;
        _fir_samples[i].time_in_micros = 0;
        _instantaneous_derivative[i] = 0;
    }
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