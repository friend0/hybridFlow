#define ARM_MATH_CM4
#include <arm_math.h>

int SAMPLE_RATE = 100000;              // Sample rate in hertz.
float SPECTRUM_MIN_DB = 30.0;          // Audio intensity (in decibels) that maps to low LED brightness.
float SPECTRUM_MAX_DB = 60.0;          // Audio intensity (in decibels) that maps to high LED brightness.

// TODO: remove this
int LEDS_ENABLED = 1;                  // Control if the LED's should display the spectrum or not.  1 is true, 0 is false.

// Useful for turning the LED display on and off with commands from the serial port.
const int FFT_SIZE = 1024;             // Size of the FFT.  Can be as most, 1024. Possibly 2048.
const int ANALOG_READ_RESOLUTION = 10; // Bits of resolution for the ADC.
const int ANALOG_READ_AVERAGING = 1;  // Number of samples to average with each ADC reading.
const int MAX_CHARS = 65;              // Max size of the input command buffer

// TODO: update logic and reasoning here when writing spectrum analysis functions
static int const CHARACTERISTIC_FREQUENCIES = 1;      // TODO: CHARACTERISTIC_FREQUENCIES should be determined by number of frequencies in characteristic array of freqs
// 1 characteristic frequency is the defualt
float frequencyWindow[CHARACTERISTIC_FREQUENCIES + 1];

float samples[FFT_SIZE * 2];        // Contains the latest set of time domain samples for the FFT
float magnitudes[FFT_SIZE];         // Contains magnitudes, in dB, for each positive frequence bin up to Fnyq


