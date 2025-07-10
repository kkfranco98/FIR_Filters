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
    V value;
    T time_in_micros;
};
