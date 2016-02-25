/**
 * Acoustic Flow Sensing on the Teensy 3.2
 */

#define ARM_MATH_CM4
#include <arm_math.h>
#include <Adafruit_NeoPixel.h>


// Processing States
typedef enum { NOT_LISTENING, LISTENING, LOW_POWER_POLL, FAST_POLL, TRANSMIT } procStates;
// current state-machine state
procStates processingState = NOT_LISTENING;
// Transmit Order
char transmitOrder[] = {'R', 'A', 'T'};

////////////////////
// Public Defines //
////////////////////

#define ONE_MILLION 1000000
#define HWSERIAL Serial3
#define PIN_WAKE 5

/////////////////////////////////////////////////////////////////////////////////////////
// Shared Defines: Keep this block consistent with corresponding block in Simblee code //
/////////////////////////////////////////////////////////////////////////////////////////
#define MAX_DATA_SIZE 3
float payload[MAX_DATA_SIZE];

///////////////////////
// FFT Configuration //
///////////////////////

int SAMPLE_RATE = 100000;              // Sample rate in hertz.
float SPECTRUM_MIN_DB = 30.0;          // Audio intensity (in decibels) that maps to low LED brightness.
float SPECTRUM_MAX_DB = 60.0;          // Audio intensity (in decibels) that maps to high LED brightness.
int LEDS_ENABLED = 1;                  // Control if the LED's should display the spectrum or not.  1 is true, 0 is false.
// Useful for turning the LED display on and off with commands from the serial port.
const int FFT_SIZE = 1024;             // Size of the FFT.  Can be as most, 1024. Possibly 2048.
const int AUDIO_INPUT_PIN = 14;        // Input ADC pin for audio data.
const int ANALOG_READ_RESOLUTION = 10; // Bits of resolution for the ADC.
const int ANALOG_READ_AVERAGING = 16;  // Number of samples to average with each ADC reading.
const int POWER_LED_PIN = 13;          // Output pin for power LED (pin 13 to use Teensy 3.0's onboard LED).
const int MAX_CHARS = 65;              // Max size of the input command buffer

// @TODO remove refs to the neopixel
const int NEO_PIXEL_PIN = 3;           // Output pin for neo pixels.
const int NEO_PIXEL_COUNT = 4;         // Number of neo pixels.  You should be able to increase this without

////////////////////
// INTERNAL STATE //
////////////////////

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NEO_PIXEL_COUNT, NEO_PIXEL_PIN, NEO_GRB + NEO_KHZ800);
IntervalTimer samplingTimer, flowCheckTimer;

float samples[FFT_SIZE * 2];
float magnitudes[FFT_SIZE];
// TODO: update this
float frequencyWindow[NEO_PIXEL_COUNT + 1];
float hues[NEO_PIXEL_COUNT];

unsigned int currentValue;
int sampleCounter = 0;
// TODO: remove this
char commandBuffer[MAX_CHARS];

////////////////////
// External State //
////////////////////

float rate;
float avgRate;
float timeStamp;

/////////////////////
// SETUP FUNCTIONS //
/////////////////////

void setup() {

        // Var Inits
        rate = 1.0;
        avgRate = 2.0;
        timeStamp = 1.78;
        
        // UART
        Serial.begin(38400);  
        HWSERIAL.begin(115200);

        // Init test-data
        for(int i = 0; i < MAX_DATA_SIZE; i++) payload[i] = i;
        
        // ADC and Audio Init
        pinMode(AUDIO_INPUT_PIN, INPUT);                        
        analogReadResolution(ANALOG_READ_RESOLUTION);
        analogReadAveraging(ANALOG_READ_AVERAGING);

        // GPIO Inits
        pinMode(PIN_WAKE, OUTPUT);
        pinMode(POWER_LED_PIN, OUTPUT);
        digitalWrite(PIN_WAKE, LOW);
        digitalWrite(POWER_LED_PIN, HIGH);
        
        // Clear the input command buffer
        memset(commandBuffer, 0, sizeof(commandBuffer));

        // Initialize spectrum display
        //spectrumSetup();
        // Begin sampling audio
        //samplingBegin();     

}

////////////////
// Start loop //
////////////////

void loop() {

        // TODO: New guard

//        if (samplingIsDone()) {
//                // Run FFT on sample data. Create arm FFT instance, initialize it, run it, then get the magnitudes of the results.
//                arm_cfft_radix4_instance_f32 fft_inst;
//                arm_cfft_radix4_init_f32(&fft_inst, FFT_SIZE, 0, 1);
//                arm_cfft_radix4_f32(&fft_inst, samples);
//                arm_cmplx_mag_f32(samples, magnitudes, FFT_SIZE);
//                
//                /* Used to update the leds, don't need any more
//                if (LEDS_ENABLED == 1)
//                {
//                        // spectrumLoop(); 
//                }
//                */
//                // Restart Sampling
//                samplingBegin();                                
//        }
//        // Parse pending commands.
//        parserLoop();

  char ack = 0;
  initCommunication(MAX_DATA_SIZE);
  float payload[3] = {rate, avgRate, timeStamp};
  sendPayload(payload, MAX_DATA_SIZE, transmitOrder);
  
  while (HWSERIAL.available ()){
    ack = HWSERIAL.read();
    Serial.println((char) ack);
  }
  
  delay(1000);
}

//////////////
// End loop //
//////////////

////////////////////
// UART Functions //
////////////////////

void initCommunication(int payloadLength){
  digitalWrite(PIN_WAKE, HIGH);
  HWSERIAL.print((uint8_t)payloadLength);
}

void sendPayload(float payload[], int payloadLength, char transmitOrder[]){

  for(int i = 0; i < payloadLength; i++){
    HWSERIAL.print(transmitOrder[i]);
    for(int j = 0; j < 4; j++){
      HWSERIAL.print( (int) payload[i] & (0xF000 >> (4*j)));
    }
  }
  digitalWrite(PIN_WAKE, LOW);  
}

///////////////////////
// UTILITY FUNCTIONS //
///////////////////////

// Compute the average magnitude of a target frequency window vs. all other frequencies.
void windowMean(float* magnitudes, int lowBin, int highBin, float* windowMean, float* otherMean) {
        *windowMean = 0;
        *otherMean = 0;
        // Notice the first magnitude bin is skipped because it represents the average power of the signal.       
        for (int i = 1; i < FFT_SIZE / 2; ++i) {
                if (i >= lowBin && i <= highBin) {
                        *windowMean += magnitudes[i];
                }
                else {
                        *otherMean += magnitudes[i];
                }
        }
        *windowMean /= (highBin - lowBin) + 1;
        *otherMean /= (FFT_SIZE / 2 - (highBin - lowBin));
}

// Convert a frequency to the appropriate FFT bin it will fall within.
int frequencyToBin(float frequency) {
        float binFrequency = float(SAMPLE_RATE) / float(FFT_SIZE);
        return int(frequency / binFrequency);
}

////////////////////////
// SAMPLING FUNCTIONS //
////////////////////////

void samplingCallback() {
        // Read from the ADC and store the sample data
        samples[sampleCounter] = (float32_t)analogRead(AUDIO_INPUT_PIN);
        // Complex FFT functions require a coefficient for the imaginary part of the input.
        // Since we only have real data, set this coefficient to zero.
        samples[sampleCounter + 1] = 0.0;
        // Update sample buffer position and stop after the buffer is filled
        sampleCounter += 2;
        if (sampleCounter >= FFT_SIZE * 2) {
                samplingTimer.end();
        }
}

void samplingBegin() {
        // Reset sample buffer position and start callback at defined sampling rate.
        sampleCounter = 0;
        // Start the interval timer with a period inverse to the sampling rate. Scale by 1e6 because arg is in uSec
        samplingTimer.begin(samplingCallback, 1000000 / SAMPLE_RATE);
}

boolean samplingIsDone() {
        return sampleCounter >= FFT_SIZE * 2;
}


////////////////////////////////////////////////////////////////////////////////
// COMMAND PARSING FUNCTIONS
// These functions allow parsing simple commands input on the serial port.
// Commands allow reading and writing variables that control the device.
//
// All commands must end with a semicolon character.
//
// Example commands are:
// GET SAMPLE_RATE;
// - Get the sample rate of the device.
// SET SAMPLE_RATE 400;
// - Set the sample rate of the device to 400 hertz.
//
////////////////////////////////////////////////////////////////////////////////

void parserLoop() {
        // Process any incoming characters from the serial port
        while (Serial.available() > 0) {
                char c = Serial.read();
                // Add any characters that aren't the end of a command (semicolon) to the input buffer.
                if (c != ';') {
                        c = toupper(c);
                        strncat(commandBuffer, &c, 1);
                }
                else
                {
                        // Parse the command because an end of command token was encountered.
                        parseCommand(commandBuffer);
                        // Clear the input buffer
                        memset(commandBuffer, 0, sizeof(commandBuffer));
                }
        }
}

// Macro used in parseCommand function to simplify parsing get and set commands for a variable
#define GET_AND_SET(variableName) \
  else if (strcmp(command, "GET " #variableName) == 0) { \
    Serial.println(variableName); \
  } \
  else if (strstr(command, "SET " #variableName " ") != NULL) { \
    variableName = (typeof(variableName)) atof(command+(sizeof("SET " #variableName " ") - 1)); \
  }

void parseCommand(char* command) {
        if (strcmp(command, "GET MAGNITUDES") == 0) {
                for (int i = 0; i < FFT_SIZE; ++i) {
                        Serial.println(magnitudes[i]);
                }
        }
        else if (strcmp(command, "GET SAMPLES") == 0) {
                for (int i = 0; i < FFT_SIZE * 2; i += 2) {
                        Serial.println(samples[i]);
                }
        }
        else if (strcmp(command, "GET FFT_SIZE") == 0) {
                Serial.println(FFT_SIZE);
        }
        GET_AND_SET(SAMPLE_RATE)
        GET_AND_SET(LEDS_ENABLED)
        GET_AND_SET(SPECTRUM_MIN_DB)
        GET_AND_SET(SPECTRUM_MAX_DB)

        // Update spectrum display values if sample rate was changed.
        if (strstr(command, "SET SAMPLE_RATE ") != NULL) {
                spectrumSetup();
        }

        // Turn off the LEDs if the state changed.
        if (LEDS_ENABLED == 0) {
                for (int i = 0; i < NEO_PIXEL_COUNT; ++i) {
                        pixels.setPixelColor(i, 0);
                }
                pixels.show();
        }
}


// TODO: Remove this
// Convert from HSV values (in floating point 0 to 1.0) to RGB colors usable
// by neo pixel functions.
uint32_t pixelHSVtoRGBColor(float hue, float saturation, float value) {
        // Implemented from algorithm at http://en.wikipedia.org/wiki/HSL_and_HSV#From_HSV
        float chroma = value * saturation;
        float h1 = float(hue) / 60.0;
        float x = chroma * (1.0 - fabs(fmod(h1, 2.0) - 1.0));
        float r = 0;
        float g = 0;
        float b = 0;
        if (h1 < 1.0) {
                r = chroma;
                g = x;
        }
        else if (h1 < 2.0) {
                r = x;
                g = chroma;
        }
        else if (h1 < 3.0) {
                g = chroma;
                b = x;
        }
        else if (h1 < 4.0) {
                g = x;
                b = chroma;
        }
        else if (h1 < 5.0) {
                r = x;
                b = chroma;
        }
        else // h1 <= 6.0
        {
                r = chroma;
                b = x;
        }
        float m = value - chroma;
        r += m;
        g += m;
        b += m;
        return pixels.Color(int(255 * r), int(255 * g), int(255 * b));
}


////////////////////////////////
// SPECTRUM DISPLAY FUNCTIONS //
////////////////////////////////

// TODO: repurpose this function
void spectrumSetup() {
        // Set the frequency window values by evenly dividing the possible frequency
        // spectrum across the number of neo pixels.
        float windowSize = (SAMPLE_RATE / 2.0) / float(NEO_PIXEL_COUNT);
        for (int i = 0; i < NEO_PIXEL_COUNT + 1; ++i) {
                frequencyWindow[i] = i * windowSize;
        }
        // Evenly spread hues across all pixels.
        for (int i = 0; i < NEO_PIXEL_COUNT; ++i) {
                hues[i] = 360.0 * (float(i) / float(NEO_PIXEL_COUNT - 1));
        }
}

void spectrumLoop() {
        // Update each LED based on the intensity of the audio
        // in the associated frequency window.
        float intensity, otherMean;
        for (int i = 0; i < NEO_PIXEL_COUNT; ++i) {
                windowMean(magnitudes,
                           frequencyToBin(frequencyWindow[i]),
                           frequencyToBin(frequencyWindow[i + 1]),
                           &intensity,
                           &otherMean);
                // Convert intensity to decibels.
                intensity = 20.0 * log10(intensity);
                // Scale the intensity and clamp between 0 and 1.0.
                intensity -= SPECTRUM_MIN_DB;
                //Saturate intensity at 0, i.e. restrict to positive intensity
                intensity = intensity < 0.0 ? 0.0 : intensity;  
                intensity /= (SPECTRUM_MAX_DB - SPECTRUM_MIN_DB);
                intensity = intensity > 1.0 ? 1.0 : intensity;

        }
        pixels.show();
}
