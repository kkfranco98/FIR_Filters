#pragma once

enum class TimeUnit
{
    SECONDS,
    MILLISECONDS,
    MICROSECONDS
};

template <typename V, typename T>
struct Sample
{
    V value = static_cast<V>(0);
    T time_in_micros = static_cast<T>(0);
};
