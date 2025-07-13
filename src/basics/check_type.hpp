#pragma once

// Caso base: se i tipi non sono uguali, is_same_custom::value sarà false
template <typename T, typename U>
struct is_same_custom
{
    static const bool value = false;
};

// Specializzazione: se i tipi sono uguali, is_same_custom::value sarà true
template <typename T>
struct is_same_custom<T, T>
{
    static const bool value = true;
};