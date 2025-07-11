#include "waveform_function.h"

#define _2_PI 6.283185307179586476925286766559
#define PI_2 1.5707963267948966192313216916398

double Waveform_Function::triangular_wave(double teta)
{
    teta = std::fmod(teta, _2_PI);

    switch ((uint8_t)(teta / PI_2)) //* uso le equazioni della retta
    {
    case 0: //* primo quadrante
        return 1.0 * (teta / PI_2);
        break;

    case 1: //* secondo quadrante
        return -1.0 * (teta / PI_2) + 2;
        break;

    case 2: //* terzo quadrante (uguale al secondo)
        return -1.0 * (teta / PI_2) + 2;
        break;

    case 3: //* quarto quadrante
        return 1.0 * (teta / PI_2) - 4;
        break;

    default:
        return 0;
        break;
    }
}

double Waveform_Function::sawtooth_wave(double teta)
{
    teta = std::fmod(teta, _2_PI);
    return (teta / _2_PI);
}

int8_t Waveform_Function::square_wave(double teta)
{
    teta = std::fmod(teta, _2_PI);

    if (teta < PI)
        return 1;
    else
        return -1;
}

int8_t Waveform_Function::pulse_width_modulation(double teta, double duty_cycle_in_percent)
{
    teta = std::fmod(teta, _2_PI);
    if (teta < _2_PI * (duty_cycle_in_percent / 100))
        return 1;
    else
        return 0;
}