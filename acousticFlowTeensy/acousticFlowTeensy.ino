/**
 * Acoustic Flow Sensing on the Teensy 3.2
 */

#define ARM_MATH_CM4
#include <arm_math.h>
#include "teensyFSM.h"
#include <TimerOne.h>

////////////////////
// Public Defines //
////////////////////

typedef union floatBytes floatBytes;

union floatBytes {
   float asFloat;
   byte asBytes[4];
} data;

#define ONE_MILLION 1000000
#define HWSERIAL Serial3
static float const POLLING_RATE_HZ = 0.2;

//////////////////////////
// State Machine Setups //
//////////////////////////

/**
Definte UART packet structure: keep this consitent between Simblee and Teensy code 
**/
// Setup Bit-Bang UART FSM States
typedef enum { NOT_LISTENING, LISTENING, LOW_POWER_POLL, FAST_POLL, TRANSMIT } procStates;
// current UART state-machine state
procStates processingState = NOT_LISTENING;
// Transmit Order
char transmitOrder[] = {'R', 'A', 'T'};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Shared UART payload definition: Keep this consistent with corresponding block in Simblee code //
///////////////////////////////////////////////////////////////////////////////////////////////////

static int const MAX_DATA_SIZE = 3;
float payload[MAX_DATA_SIZE];

///////////////////////
// Pin Configuration //
///////////////////////

const int AUDIO_INPUT_PIN = 20;        // Input ADC pin for audio data.
const int POWER_LED_PIN = 13;          // Output pin for power LED (pin 13 to use Teensy 3.0's onboard LED).
const int PIN_WAKE  = 9;

///////////////////////
// FFT Configuration //
///////////////////////

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

//////////////////////////////
// State and Command Buffer //
//////////////////////////////

float samples[FFT_SIZE * 2];        // Contains the latest set of time domain samples for the FFT
float magnitudes[FFT_SIZE];         // Contains magnitudes, in dB, for each positive frequence bin up to Fnyq

// TODO: Change these to the values we actually want to send
// Estimate Vector to be sento to Simblee, to be sent to Artik, to be sent to to Web App
float rate;
float avgRate;
float timeStamp;

int sampleCounter = 0;
int sampleCounterOneShot = 0;

char commandBuffer[MAX_CHARS];


/////////
// FSM //
/////////

teensyFSM AquaStat;
teensyFSMEvent currentEvent;
teensyFSMEvent lastEvent;

////////////
// Timers //
////////////

//TODO: may need one more timer to keep track of water flow from start to finish, i.e. how long water has been running.
IntervalTimer samplingTimer;        // Use this time to set the sampling rate
IntervalTimer flowCheckTimer;       // Use this to wake the Teensy and poll the sensor to do flow detection

/////////////////////
// SETUP FUNCTIONS //
/////////////////////


void setup() {

        /** 
        Setup FSM for the AquaStat 
        **/        
        teensyFSMCtor(&AquaStat);        
        FsmInit((Fsm *)&AquaStat, 0);


        // Var Inits
        rate = 1.0;
        avgRate = 2.0;
        timeStamp = 1.78;

              
        // UART for Simblee Comm
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


if(currentEvent != lastEvent){

    updateFSM(teensyFSM, currentEvent)
    lastEvent = currentEvent;
}
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


////////////////////////
// SAMPLING FUNCTIONS //
////////////////////////

// TODO: use this function to write another function called 'fftOneShot' that takes enough samples, performs FFT and does flow check on result.
void samplingCallback() {
        // Read from the ADC and store the sample data
        samples[sampleCounter] = (float32_t)analogRead(AUDIO_INPUT_PIN);
        sampleCounter++;
        // Set complex FFT coefficients to zero.        
        samples[sampleCounter] = 0.0;
        sampleCounter ++;
        if (sampleCounter >= FFT_SIZE * 2) samplingTimer.end();        
}

/**
 * [samplingBegin description]
 */
void samplingBegin() {
        // Reset sample buffer position and start callback at defined sampling rate.
        sampleCounter = 0;
        // Start the interval timer with a period inverse to the sampling rate. Scale by 1e6 because arg is in uSec
        samplingTimer.begin(samplingCallback, 1000000 / SAMPLE_RATE);
}

/**
 * Check if there are enough new samples to do the FFT.
 * @return {[type]} boolean
 */
boolean samplingIsDone() {
        return sampleCounter >= FFT_SIZE * 2;
}

/////////////////////////
// SPECTRUM  FUNCTIONS //
/////////////////////////

// Note that the Radix4 FFT is optimized for FFTs with lengths a power of 4

/**
 * Take enough samples for FFT_SIZE, then perform a single FFT. Blocking Code. Meant to be called in-between sleep
 * cycles to detect flow-on events.
 * @return {[type]} [description]
 */
void fftOneShot(){
        samplingBegin(); 
        while(!samplingIsDone()); // :smiling_imp:
        arm_cfft_radix4_instance_f32 fft_inst;
        arm_cfft_radix4_init_f32(&fft_inst, FFT_SIZE, 0, 1);
        arm_cfft_radix4_f32(&fft_inst, samples);
        arm_cmplx_mag_f32(samples, magnitudes, FFT_SIZE);
}

// TODO: implement a function that does a specified number of FFTs in a row
void fftContinuous(int iterations){
    for(int i = 0; i < iterations; i++){
        samplingBegin(); 
        while(!samplingIsDone()); // :smiling_imp:
        arm_cfft_radix4_instance_f32 fft_inst;
        arm_cfft_radix4_init_f32(&fft_inst, FFT_SIZE, 0, 1);
        arm_cfft_radix4_f32(&fft_inst, samples);
        arm_cmplx_mag_f32(samples, magnitudes, FFT_SIZE);
        // TODO: will need to do something with the values here, else it might be silly to do this.
    }
}

/**
 * Perform logic to detect flow events based on latest FFT data, configuration data.
 * @return {[type]} [description]
 */
void flowCheck(){

}

// TODO: use this function to write another function called 'fftOneShot' that takes enough samples, performs FFT and does flow check on result.
void pollingCallback() {
        // Read from the ADC and store the sample data
        samples[sampleCounter] = (float32_t)analogRead(AUDIO_INPUT_PIN);
        sampleCounter++;
        // Set complex FFT coefficients to zero.        
        samples[sampleCounter] = 0.0;
        sampleCounter ++;
        if (sampleCounter >= FFT_SIZE * 2) samplingTimer.end();        
}

/**
 * [samplingBegin description]
 */
void pollingBegin() {
        // Reset sample buffer position and start callback at defined sampling rate.
        sampleCounter = 0;
        // Start the interval timer with a period inverse to the sampling rate. Scale by 1e6 because arg is in uSec
        samplingTimer.begin(samplingCallback, 1000000 / POLLING_RATE_HZ);
}


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

// TODO: repurpose this function
void spectrumSetup() {
        // Set the frequency window values by evenly dividing the possible frequency
        // spectrum across the number of characteristic frequencies.
        float windowSize = (SAMPLE_RATE / 2.0) / float(CHARACTERISTIC_FREQUENCIES);
        for (int i = 0; i < CHARACTERISTIC_FREQUENCIES + 1; ++i) {
                frequencyWindow[i] = i * windowSize;
        }
        // Evenly spread hues across all pixels.
        for (int i = 0; i < CHARACTERISTIC_FREQUENCIES; ++i) {
                // TODO: load these into an array that we'll use
                // hues[i] = 360.0 * (float(i) / float(LIMIT - 1));
        }
}

/**
 * This function is used to iterate over characteristic frequencies for signal of interest, 
 * and log their amplitudes.
 * @return {[type]} [description]
 */
void spectrumLoop() {
        float intensity, otherMean;
        for (int i = 0; i < CHARACTERISTIC_FREQUENCIES; ++i) {
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
        //pixels.show();
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

}




