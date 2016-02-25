////////////////////////////////////////////////////////////
// Acoustic Flow Sensing Data Transmission on the Simblee //
////////////////////////////////////////////////////////////

#include <SimbleeCOM.h>

////////////////////
// Public Defines //
////////////////////

#define WAKE_PIN 5

/////////////
//  States //
/////////////

typedef enum { DEF, RATE, AVG_RATE, STATUS, TIME } bbStates;
bbStates state = DEF;

/////////////////////////////////////////////////////////////////////////////////////////
// Shared Defines: Keep this block consistent with corresponding block in Simblee code //
/////////////////////////////////////////////////////////////////////////////////////////

#define MAX_DATA_SIZE 10
uint8_t testData[MAX_DATA_SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

////////////////////
// Internal State //
////////////////////

int deviceId;                                 //
uint8_t deviceIdLow;                          //
uint8_t deviceIdHigh;                         //
int sleepTime;                                // Sleep time in ms
int woke(uint32_t pinName);                   //
int currentValue;                             //

void sendInt();
void processIncomingByte (const byte c);
void handlePreviousState ();

#define RX_PIN 28
#define TX_PIN 29

/////////////////////
// Setup Functions //
/////////////////////

void setup() {
        deviceId = getDeviceId();
        // put your setup code here, to run once:
        sleepTime = 1000;
        // For pin-wake Interrupt
        pinMode(WAKE_PIN, INPUT); // set pin 5 to input
        pinMode (PIN_SPI_SS, OUTPUT);
        Simblee_pinWakeCallback(WAKE_PIN, HIGH, woke);
        Serial.begin(115200, RX_PIN, TX_PIN);
}

////////////////
// Start loop //
////////////////

void loop() {

        // Ultra Low Power sleep
        Simblee_ULPDelay(INFINITE);
}

//////////////
// End loop //
//////////////

////////////////////
// UART Functions //
////////////////////

int woke(uint32_t pinName) {
        int payloadLength = 0;
        Simblee_resetPinWake(pinName);
        processIncomingByte(Serial.read());
        handlePreviousState();

        return 1;
}

void sendInt(int data) {
        Serial.print(data & 0xF000);
        Serial.print(data & 0x0F00);
        Serial.print(data & 0x00F0);
        Serial.print(data & 0x000F);
}

/////////////////////////////
// State Machine Functions //
/////////////////////////////

void handlePreviousState (){
        switch (state){
        case RATE:
                Serial.println (currentValue);
                break;
        case AVG_RATE:
                Serial.println (currentValue);
                break;
        case STATUS:
                Serial.println (currentValue);
                break;
        case TIME:
                Serial.println (currentValue);
                break;
        default:
                ;
        }  // end of switch
        currentValue = 0;
}

void processIncomingByte (const byte c){
        if (isdigit (c)){
                currentValue *= 10;
                currentValue += c - '0';
        }  // end of digit
        else{
                // The end of the number signals a state change
                handlePreviousState ();

                // set the new state, if we recognize it
                switch (c){
                case 'R':
                        state = RATE;
                        break;
                case 'A':
                        state = AVG_RATE;
                        break;
                case 'S':
                        state = STATUS;
                        break;
                case 'T':
                        state = TIME;
                        break;
                default:
                        state = DEF;
                        break;
                }  // end of switch on incoming byte
        } // end of not digit
} // end of processIncomingByte

////////////////////////////////
// Simblee Wireless Functions //
////////////////////////////////

/**
void SimbleeCOM_onReceive(unsigned int esn, const char *payload, int len, int rssi) {
        printf("%d ", rssi); // prints RSSI to the serial port printf("0x%08x ", esn); // prints ESN of sender to the serial port
        for (int i = 0; i < len; i++) {
                printf("%02x ", payload[i]); // prints payload data to the serial port printf("\n");
        }
}
*/

void SimbleeBLE_onConnect() {
        // Insert code
}


void SimbleeBLE_onDisconnect() {
        // Insert code here
}

void SimbleeBLE_onReceive(char *data, int len) {
        uint8_t myByte = data[0]; // store first char in array to myByte
        Serial.println(myByte); // print myByte via serial
}

void SimbleeBLE_onRSSI(int rssi) {
        Serial.println(rssi); // print rssi value via serial
}


