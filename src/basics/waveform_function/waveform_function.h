#pragma once

#include <Arduino.h>

class Waveform_Function
{
public:
    static double triangular_wave(double teta);
    static double sawtooth_wave(double teta);
    static int8_t square_wave(double teta);
    static int8_t pulse_width_modulation(double teta, double duty_cycle_in_percent);
};