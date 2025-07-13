/**
 * @file waveform_filter_example.ino
 * @brief FIR Filter example on different waveforms (sine, triangle)
 */

// If this is the first file you're reading,
// check the example file "how_to_use.ino" first.

// ⚠️ Warning: The example files are written to be visualized using the Teleplot extension in VSCode. If you don't use this extension, the data printed to the serial monitor will not make much sense as displayed.

//------------------------------------------------------------------
// Choose the signal type
// Available options:
#define SINE_WAVES 0
#define SINE_WAVES_WITH_MEAN_VALUE_OF_1 1
#define TRIANGULAR_WAVE 2

#include <FIR_Filters.h>
#include "basics/waveform_function/waveform_function.h"

#define SIGNAL_TYPE (SINE_WAVES) //<-- Select the signal here

//------------------------------------------------------------------
// Set filter coefficients depending on the signal type
#if (SIGNAL_TYPE == SINE_WAVES) // <-- LOW PASS FILTER
#define FIR_TAPS 51
const double coefficients[FIR_TAPS] = {
    -0.0003050742606699, -0.0005047685253627, -0.0008616010882402, -0.001314195424022, -0.001833454689205, -0.002362853269245, -0.002815256537629, -0.003072882017364, -0.002991425041566, -0.002407811924752, -0.001152617424261, 0.0009344719556839, 0.003986782630593, 0.008092728857227, 0.01327915588398, 0.01949791623952, 0.0266188260074, 0.03442890622136, 0.04263902537016, 0.05089930357254, 0.058820571068, 0.06600078427976, 0.07205591488344, 0.07664849671656, 0.07951579680627, 0.08049062637894, 0.07951579680627, 0.07664849671656, 0.07205591488344, 0.06600078427976, 0.058820571068, 0.05089930357254, 0.04263902537016, 0.03442890622136, 0.0266188260074, 0.01949791623952, 0.01327915588398, 0.008092728857227, 0.003986782630593, 0.0009344719556839, -0.001152617424261, -0.002407811924752, -0.002991425041566, -0.003072882017364, -0.002815256537629, -0.002362853269245, -0.001833454689205, -0.001314195424022, -0.0008616010882402, -0.0005047685253627, -0.0003050742606699};

#elif (SIGNAL_TYPE == SINE_WAVES_WITH_MEAN_VALUE_OF_1) // <-- HIGH PASS FILTER
#define FIR_TAPS 57
const double coefficients[FIR_TAPS] = {
    0.008025385637561, -0.007005042602471, -0.00500693042307, -0.003223260108276, -0.001036617618986, 0.001632304766096, 0.004364707990073, 0.006387108478206, 0.00678743014796, 0.004993127458246, 0.0009710273494166, -0.00451599141745, -0.01002535479649, -0.01371677724606, -0.01386454748355, -0.009451063796601, -0.0006358795726608, 0.01098417825222, 0.02252541396394, 0.03031343498603, 0.03070207401602, 0.02099812182777, 0.0002705845633769, -0.03016519524841, -0.06677055308935, -0.1043167249654, -0.1368654753064, -0.1589638081487, 0.833212732833, -0.1589638081487, -0.1368654753064, -0.1043167249654, -0.06677055308935, -0.03016519524841, 0.0002705845633769, 0.02099812182777, 0.03070207401602, 0.03031343498603, 0.02252541396394, 0.01098417825222, -0.0006358795726608, -0.009451063796601, -0.01386454748355, -0.01371677724606, -0.01002535479649, -0.00451599141745, 0.0009710273494166, 0.004993127458246, 0.00678743014796, 0.006387108478206, 0.004364707990073, 0.001632304766096, -0.001036617618986, -0.003223260108276, -0.00500693042307, -0.007005042602471, 0.008025385637561};

#elif (SIGNAL_TYPE == TRIANGULAR_WAVE) // <-- LOW PASS FILTER
#define FIR_TAPS 34
const double coefficients[FIR_TAPS] = {
    -0.0004372596060551, -0.001402291012967, -0.003267439835099, -0.006175556266092, -0.009967220840763, -0.01401220102453, -0.01714629929015, -0.0177641722361, -0.01409401757659, -0.004622006043845, 0.01142445948084, 0.0336691496628, 0.06039331684187, 0.08865117728946, 0.1147134707956, 0.1347602095338, 0.1456632961278, 0.1456632961278, 0.1347602095338, 0.1147134707956, 0.08865117728946, 0.06039331684187, 0.0336691496628, 0.01142445948084, -0.004622006043845, -0.01409401757659, -0.0177641722361, -0.01714629929015, -0.01401220102453, -0.009967220840763, -0.006175556266092, -0.003267439835099, -0.001402291012967, -0.0004372596060551};
#endif

// Initialize FIR filter
FIR_Filter_post_multiplication<double, FIR_TAPS> filter(coefficients);

// Signal buffer
#if defined(ARDUINO_AVR_UNO) || defined(__AVR_ATmega328P__)
#pragma message("Using less samples for ARDUINO_AVR_UNO or __AVR_ATmega328P__, because of limited RAM")
    #if (SIGNAL_TYPE == SINE_WAVES)
        #define SIGNAL_SAMPLES 100
        #pragma message("Using 100 samples for SINE_WAVES")
    #elif (SIGNAL_TYPE == SINE_WAVES_WITH_MEAN_VALUE_OF_1)
        #define SIGNAL_SAMPLES 70
        #pragma message("Using 70 samples for SINE_WAVES_WITH_MEAN_VALUE_OF_1")
    #elif (SIGNAL_TYPE == TRIANGULAR_WAVE)
        #define SIGNAL_SAMPLES 120
        #pragma message("Using 120 samples for TRIANGULAR_WAVE")
    #endif
#else
#define SIGNAL_SAMPLES 200
#endif


double input_signal[SIGNAL_SAMPLES];

void setup()
{
    Serial.begin(115200);

    // Generate synthetic signal based on chosen waveform
    for (auto i = 0; i < SIGNAL_SAMPLES; i++)
    {
#if (SIGNAL_TYPE == SINE_WAVES)
        // low + high frequency sine
        input_signal[i] = 1.0 * sin(2 * PI * 2 * ((double)i / (double)SIGNAL_SAMPLES)) +
                          0.3 * sin(2 * PI * 30 * ((double)i / (double)SIGNAL_SAMPLES));
#elif (SIGNAL_TYPE == SINE_WAVES_WITH_MEAN_VALUE_OF_1)
        // sine with DC offset
        #if defined(ARDUINO_AVR_UNO) || defined(__AVR_ATmega328P__)
            input_signal[i] = 1 + sin(2 * PI * ((double)SIGNAL_SAMPLES/8.0) * ((double)i / (double)SIGNAL_SAMPLES)); // sine wave with mean value of 1
        #else
            input_signal[i] = 1 + sin(2 * PI * 20.0 * ((double)i / (double)SIGNAL_SAMPLES)); // sine wave with mean value of 1
        #endif
#elif (SIGNAL_TYPE == TRIANGULAR_WAVE)
        // triangle wave
        input_signal[i] = Waveform_Function::triangular_wave(2 * PI * ((double)SIGNAL_SAMPLES/40.0) * ((double)i / (double)SIGNAL_SAMPLES));
#endif
    }
}

#define REFRESH_RATE_IN_SECONDS 5 //<-- Must match Teleplot window refresh rate
void loop()
{
    filter.reset();

    for (uint64_t i = 0; i < SIGNAL_SAMPLES; i++)
    {
        Serial.print(">data_in:");
        Serial.println(input_signal[i]);

        filter.add_sample(input_signal[i], i + 1, TimeUnit::SECONDS);

        Serial.print(">filter_out:");
        Serial.println(filter.get_filtered_value());

        Serial.print(">instantaneous_derivative:");
        Serial.println(filter.get_instantaneous_derivative());

        delay(20);
    }

    // Wait before repeating the loop for visualization (e.g., in Teleplot)
    delay(REFRESH_RATE_IN_SECONDS * 1000);
}