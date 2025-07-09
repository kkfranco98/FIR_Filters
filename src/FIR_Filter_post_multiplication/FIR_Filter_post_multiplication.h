#include "basics/Sample.hpp"

template <typename V, typename T, size_t N>
class FIR_Filter_post_multiplication
{
    Sample<V, T> _fir_samples[N];

public:
    FIR_Filter_post_multiplication()
    {
        // Limit T a float o double
        static_assert(std::is_same<V, float>::value || std::is_same<V, double>::value,
                      "Circular_Array_FIR_Filter can only be instantiated with float or double");

#include <cstdint>
#if defined(UINT64_MAX)
        // uint64_t è disponibile
        static_assert(std::is_same<T, uint32_t>::value || std::is_same<T, uint64_t>::value,
                      "M deve essere uint32_t o uint64_t");
#else
        // Solo uint32_t disponibile
        static_assert(std::is_same<T, uint32_t>::value,
                      "M deve essere uint32_t (uint64_t non supportato)");
#endif
    };

    void add_sample(V value, T time);
    Sample get_last_element();

    // TODO
};
