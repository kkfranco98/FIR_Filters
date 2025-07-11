#include <Arduino.h>
#include <FIR_Filters.h>

#define FIR_TAPS 5
#define SAMPLES 20
/**
 * There are 3 different FIR filter implementations:
 * 1) FIR_Filter_post_multiplication
 * 2) FIR_Filter_cascade
 * 3) FIR_Filter_symmetric_optimized
 *
 * In this example we demonstrate the first one.
 */

// ───────────────────────────────────────────────────────
// How to create a FIR filter object:

// Option 1: Default constructor (equal coefficients = 1/N)
FIR_Filter_post_multiplication<double, FIR_TAPS> filter_1;

// Option 2: Manually define filter coefficients (double)
double double_coefficients[5] = {0.1, 0.2, 0.3, 0.4, 0.5};
FIR_Filter_post_multiplication<double, FIR_TAPS> filter_2(double_coefficients);

// Option 3: Use float instead of double (more efficient on some platforms)
float float_coefficients[5] = {0.1, 0.2, 0.3, 0.4, 0.5};
FIR_Filter_post_multiplication<float, FIR_TAPS> filter_3(float_coefficients);

// The filter is now ready to use.

// The other two filter types (cascade and symmetric optimized)
// will be implemented in future releases.

void setup()
{
    Serial.begin(115200);

    // To add a new sample to the filter:
    filter_1.add_sample(1.0,              // <- sample value
                        0,                // <- timestamp
                        TimeUnit::SECONDS // <- time unit (see TimeUnit enum)
    );
    // TimeUnit is an enum; use autocomplete to explore supported options.
    // Specifying time correctly is important for computing the derivative.
    // If you don't care about derivatives, you can ignore the time.

    // Retrieve the filtered signal:
    filter_1.get_filtered_value();

    // Retrieve the instantaneous derivative:
    filter_1.get_instantaneous_derivative();

    // Retrieve the average derivative:
    filter_1.get_average_derivative();

    // Check if the filter is fully loaded (received at least N samples):
    filter_1.filter_is_loaded();

    // Get the most recent value:
    filter_1.get_last_value();

    // Get the last sample (struct containing value and timestamp):
    filter_1.get_last_sample();

    filter_1.get_last_sample().value;
    filter_1.get_last_sample().time_in_micros;

    // Access the coefficients used in the filter:
    filter_1.get_coefficients();

    // Reset the filter state:
    filter_1.reset();
}

#define REFRESH_RATE_IN_SECONDS 5 //<-- Must match the refresh rate in Teleplot
void loop()
{
    // Reset filter state before new run
    filter_1.reset();

    double input_signal[SAMPLES];
    for (size_t i = 0; i < SAMPLES; i++)
    {
        input_signal[i] = random(0, 30);
    }

    // Process input samples
    for (uint64_t i = 0; i < SAMPLES; i++)
    {
        Serial.print(">data_in:");
        Serial.println(input_signal[i]);

        // simuliamo di ricevere dati da un sensore
        filter_1.add_sample(input_signal[i], i + 1, TimeUnit::SECONDS);

        Serial.print(">filter_out:");
        Serial.println(filter_1.get_filtered_value());

        Serial.print(">instantaneous_derivative:");
        Serial.println(filter_1.get_instantaneous_derivative());

        delay(100);
    }

    // Wait for the next plot cycle (e.g. Teleplot)
    delay(REFRESH_RATE_IN_SECONDS * 1000);
}