
////////////////////////////////////////////////////////////
// Acoustic Flow Sensing Data Transmission on the Simblee //
////////////////////////////////////////////////////////////

// #include <SimbleeCOM.h>
#include <SimbleeBLE.h>

////////////////////
// Public Defines //
////////////////////

#define WAKE_PIN 19
#define LED 2
#define AD_INTERVAL 1000 
#define RX_PIN 8
#define TX_PIN 5

/////////////
//  States //
/////////////

typedef enum { DEF, RATE_SIG, RATE_BUFF, AVG_RATE_SIG, AVG_RATE_BUFF, STATUS_SIG, STATUS_BUFF, TIME_SIG, TIME_BUFF } bbStates;
bbStates state = DEF;
bbStates nextState = DEF;

/////////////////////////////////////////////////////////////////////////////////////////
// Shared Defines: Keep this block consistent with corresponding block in Simblee code //
/////////////////////////////////////////////////////////////////////////////////////////

#define MAX_DATA_SIZE 10
uint8_t testData[MAX_DATA_SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

////////////////////
// Internal State //
////////////////////

float rate;
float avgRate;
float timeStamp;     // Maybe only needs to be int

int deviceId;                                 //
int sleepTime;                                // Sleep time in ms
int woke(uint32_t pinName);                   //
int shiftCount;         // Count how mant shifts have been done on current value

uint8_t deviceIdLow;                          //
uint8_t deviceIdHigh;                         //

char currentValue;                            //
char status;

bool connected;

///////////////////////////////
// Forward Defined Functions //
///////////////////////////////

void processIncomingByte (const byte c);
void handlePreviousState ();
void bleTransmit();

/////////////////////
// Setup Functions //
/////////////////////

void setup() {

        deviceId = getDeviceId();
  
        // BLE Inits
        connected = false;
        SimbleeBLE.advertisementData = "-data";
        SimbleeBLE.deviceName = "Simblee";  
        SimbleeBLE.advertisementInterval = AD_INTERVAL;
        
        // Running into trouble if I don't start this before I start Serial
        SimbleeBLE.begin();
        Simblee_ULPDelay(500);         

        // Pin-wake Interrupt
        pinMode(WAKE_PIN, INPUT); // set pin 5 to input
        pinMode (PIN_SPI_SS, OUTPUT);
        pinMode(LED, OUTPUT);
        Simblee_pinWakeCallback(WAKE_PIN, HIGH, woke);
                  
        // UART
        Serial.begin(115200, RX_PIN, TX_PIN);               
        
}

////////////////
// Start loop //
////////////////

void loop() {

        // Simblee_systemOff();
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
        digitalWrite(LED, HIGH);
        Simblee_resetPinWake(pinName);
        // TODO: implement something that doesn't block. Maybe not too important given that listening to UART is just 
        // about all we're going to do with the Simblee..
        while(Serial.available()){
                processIncomingByte(Serial.read());
                handlePreviousState();
        }    
        digitalWrite(LED, LOW);        
        return 1;
}

/////////////////////////////
// State Machine Functions //
/////////////////////////////

void handlePreviousState (){
        state = nextState;
        switch (nextState){          
        case RATE_SIG:
                shiftCount = 0;
                rate = 0;
                nextState = RATE_BUFF;
                break;
        case RATE_BUFF:        
                if(shiftCount < 4) rate = (float) ((int) rate << 8 || (0x00FF & currentValue));
                //if(shiftCount == 4) Serial.println(rate);                                                
                shiftCount++;
                break;
        case AVG_RATE_SIG:
                shiftCount = 0;   
                avgRate = 0;     
                //Serial.println (currentValue);
                nextState = AVG_RATE_BUFF;
                break;
        case AVG_RATE_BUFF:
                if(shiftCount < 4) avgRate = (float) ((int) avgRate << 8 || ((0x00FF & currentValue)));
                //if(shiftCount == 4) Serial.println(avgRate);                                
                shiftCount++;
                break;
        case STATUS_SIG:
                shiftCount = 0;
                status = 0;
                //Serial.println (currentValue);
                nextState = STATUS_BUFF;                
                break;
        case STATUS_BUFF:
                if(shiftCount < 1) status = status << 8 || ((0x00FF & currentValue));
                //if(shiftCount == 1) Serial.println(status);                
                shiftCount++;        
                break;
        case TIME_SIG:
                shiftCount = 0;
                timeStamp = 0;
                //Serial.println (currentValue);
                nextState = TIME_BUFF;
                break;
        case TIME_BUFF:
                if(shiftCount < 4) timeStamp = (float) ((int) timeStamp << 8 || ((0x00FF & currentValue)));
                //if(shiftCount == 4) Serial.println(timeStamp);
                shiftCount++;
                break;                
        case DEF:
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
                // handlePreviousState ();
                // set the new state, if we recognize it
                switch (c){
                case 'R':
                        shiftCount = 0;
                        nextState = RATE_SIG;
                        Serial.println('R');
                        break;
                case 'A':
                        shiftCount = 0;
                        nextState = AVG_RATE_SIG;
                        Serial.println('A');                        
                        break;
                case 'S':
                        shiftCount = 0;
                        nextState = STATUS_SIG;
                        Serial.println('S');                        
                        break;
                case 'T':
                        shiftCount = 0;
                        nextState = TIME_SIG;
                        Serial.println('T');                        
                        break;
                default:
                        nextState = state;
                        break;
                }  // end of switch on incoming byte
        } // end of not digit
} // end of processIncomingByte

////////////////////////////////
// Simblee Wireless Functions //
////////////////////////////////

void bleTransmit(){
  //SimbleeBLE.sendFloat();
  //SimbleeBLE.sendFloat();
  //SimbleeBLE.sendFloat();
  //SimbleeBLE.sendByte();  
}

void SimbleeBLE_onAdvertisement(bool start)
{
  // turn the green led on if we start advertisement, and turn it
  // off if we stop advertisement
  
  //if (start) digitalWrite(LED, ~(digitalRead(LED)));
  //else digitalWrite(led, HIGH);
}

void SimbleeBLE_onConnect() {
        // Insert code
        connected = true;
}

void SimbleeBLE_onDisconnect() {
        // Insert code here
        connected = false;
}

void SimbleeBLE_onReceive(char *data, int len) {
        uint8_t myByte = data[0]; // store first char in array to myByte
        Serial.println(myByte); // print myByte via serial
}

void SimbleeBLE_onRSSI(int rssi) {
        Serial.println(rssi); // print rssi value via serial
}
