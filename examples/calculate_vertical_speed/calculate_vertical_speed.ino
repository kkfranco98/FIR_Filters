/**
 * @file calculate_vertical_speed.ino
 * @details
 * In my work, I needed to calculate the derivative of an aircraft's altitude
 * to determine the VSI (Vertical Speed Indicator).
 * This is an example of how you could implement that logic in code.
 */

// If this is the first file you're reading,
// check the example file "how_to_use.ino" first.

#include <Arduino.h>
#include <FIR_Filters.h>

#define DATA_SAMPLES 42
double altitude_data[DATA_SAMPLES];
double altitude_data_with_noise[DATA_SAMPLES];

/**
 * If this is the first example you're viewing,
 * check out the "how_to_use.ino" example first.
 */

// You can choose the number of taps based on desired smoothness and delay.
// The more taps, the smoother the output—but the greater the latency.
#define FIR_TAPS 5

FIR_Filter_post_multiplication<double, FIR_TAPS> filter;
// In this example, we don't provide custom coefficients,
// because we're mainly interested in computing the derivative.
// By default, the filter uses equal weights: 1/N for each coefficient.

// If you also want to filter the signal while computing the derivative,
// you can use the following constructor with custom coefficients:
//
//* #==>  double coefficients[FIR_TAPS] = { ... };
//* #==>  FIR_Filter_post_multiplication<double, FIR_TAPS> filter(coefficients);
//
// You can design the coefficients using tools like Filter Design (MATLAB)
// for better control over the frequency response (see README.md for details).

void setup()
{
    Serial.begin(115200);

    for (size_t i = 0; i < DATA_SAMPLES; i++)
    {
        altitude_data[i] = 0.0;
    }

    // Simulate a sine-shaped climb segment followed by level flight
    size_t j = 0;
    for (size_t i = 0; i < DATA_SAMPLES; i++)
    {
        if (i >= DATA_SAMPLES / 3 and i < (DATA_SAMPLES / 3) * 2)
        {
            j++;
            altitude_data[i] += 5 + 5.0 * sin(2 * PI * 1.5 * ((double)j / (double)DATA_SAMPLES) - ((2 * PI) / 4));
        }
        if (i >= (DATA_SAMPLES / 3) * 2)
        {
            altitude_data[i] += 10;
        }
    }

    // Add high-frequency noise to simulate sensor disturbance
    for (size_t i = 0; i < DATA_SAMPLES; i++)
    {
        altitude_data_with_noise[i] = altitude_data[i];
        if (i > FIR_TAPS)
        {
            altitude_data_with_noise[i] = altitude_data[i] + 0.2 * sin(2 * PI * 13.4 * ((double)i / (double)DATA_SAMPLES) - FIR_TAPS + 1);
        }
    }
}

#define REFRESH_RATE_IN_SECONDS 5 //<-- Must match the refresh rate in Teleplot
void loop()
{
    filter.reset();

    for (uint64_t i = 0; i < DATA_SAMPLES; i++)
    {
        Serial.print(">data_in(without_noise):");
        Serial.println(altitude_data[i]);

        Serial.print(">data_in(with_noise):");
        Serial.println(altitude_data_with_noise[i]);

        filter.add_sample(altitude_data_with_noise[i], i + 1, TimeUnit::SECONDS);

        Serial.print(">filter_out:");
        Serial.println(filter.get_filtered_value());

        Serial.print(">instantaneous_derivative:");
        Serial.println(filter.get_instantaneous_derivative());

        Serial.print(">average_derivative:");
        Serial.println(filter.get_average_derivative());

        delay(100);
    }

    // Wait before repeating the loop for visualization (e.g., in Teleplot)
    delay(REFRESH_RATE_IN_SECONDS * 1000);
}