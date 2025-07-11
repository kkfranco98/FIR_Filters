#include <Arduino.h>
#include <FIR_Filters.h>

// If this is the first file you're reading,
// check the example file "how_to_use.ino" first.

// Number of data samples to simulate
#define DATA_SAMPLES 40

// Original temperature data
double temperature_data[DATA_SAMPLES] = {
    // Initial samples are identical to preload the filter
    25.7, 25.7, 25.7, 25.7, 25.7, 25.7, 25.7, 25.7, 25.7, 25.7, 25.7,

    // Normal variation
    26.5, 26.1, 25.7, 25.9, 26.1, 26.1, 26.1, 26.8, 27.1, 27.1, 27.1, 27.1, 27.1, 27.1,

    // Spike
    32.1, 44.5, 50.1, 29.6,

    // Back to normal
    28.1, 27.1, 26.4, 26.1, 25.6, 25.1, 24.6, 24.1, 23.6, 23.1, 22.6};

// Array for noisy temperature data
double temperature_data_with_noise[DATA_SAMPLES];

#define FIR_TAPS 13

// Low-pass filter coefficients
// Designed using MATLAB Filter Designer with:
// - Design Method: Equiripple
// - Fs = 10 Hz
// - Fpass = 1 Hz
// - Fstop = 3 Hz
// - Apass = 0.1 dB
// - Astop = 60 dB
double coefficients[FIR_TAPS] = {
    0.00324948743177, -0.008574651213989, -0.03837407444715, -0.02869586555284, 0.09287615258727, 0.2831550885564, 0.3787024748877, 0.2831550885564, 0.09287615258727, -0.02869586555284, -0.03837407444715, -0.008574651213989, 0.00324948743177};

// FIR filter object using the coefficients above
FIR_Filter_post_multiplication<double, FIR_TAPS> filter(coefficients);

void setup()
{
    Serial.begin(115200);
}

// Teleplot refresh rate (must match the one set in Teleplot)
#define REFRESH_RATE_IN_SECONDS 5
void loop()
{
    // add some noise
    for (uint64_t i = 0; i < DATA_SAMPLES; i++)
    {
        temperature_data_with_noise[i] = temperature_data[i] + 7.5 * sin(2 * PI * 22.5 * ((double)i / (double)DATA_SAMPLES));
    }

    // Reset the filter state before processing
    filter.reset();

    for (uint64_t i = 0; i < DATA_SAMPLES; i++)
    {
        // Print original signal
        Serial.print(">data_in(without_noise):");
        Serial.println(temperature_data[i]);

        // Print noisy signal
        Serial.print(">data_in(with_noise):");
        Serial.println(temperature_data_with_noise[i]);

        // Add sample to the filter
        filter.add_sample(temperature_data_with_noise[i], i + 1, TimeUnit::SECONDS);

        // Print filtered result
        Serial.print(">filter_out:");
        Serial.println(filter.get_filtered_value());

        delay(100);
    }

    // Wait for the next Teleplot refresh cycle
    delay(REFRESH_RATE_IN_SECONDS * 1000);
}