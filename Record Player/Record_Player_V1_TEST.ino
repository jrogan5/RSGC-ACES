//PROJECT   :Record Player V1
//PURPOSE   :Tachometer for platter
//COURSE    :ICS3U
//AUTHOR    :J. ROGAN
//DATE      :2022 04 13
//MCU       :ATTINY85
//STATUS    :WORKING
//REFERENCE :

uint8_t segmentMap[] = {0b11111100, 0b01100000, 0b11011010, //0,1,2
                        0b11110010, 0b01100110, 0b10110110, //3,4,5
                        0b10111110, 0b11100000, 0b11111111, //6,7,8
                        0b11110110                          //9
                       };

const uint8_t povPin      = 0;
const uint8_t clockPin    = 1;
const uint8_t hallSnsrPin = 2;
const uint8_t latchPin    = 3;
const uint8_t dataPin     = 4;


//uint8_t sizeMap   = sizeof(segmentMap) >> 1;
bool squareWave = false;
uint8_t DisplayRPM;
uint8_t L1;
uint8_t L2;
uint16_t currentNum;
volatile uint16_t halfRevolutions;
float RPM;
int8_t hallThresh = 2;               //Pulses per rotation
uint16_t timePassed;
uint16_t prevTime = 0;

void setup() {
  definePins();
}

void loop() {
  displayRPM();
}


void displayRPM (void) {
  readRPM();
  if (squareWave) {                    //Changes char being displayed depending on wave state to transistors
    currentNum = L1;
  }
  else {
    currentNum = L2;
  }
  digitalWrite(povPin, squareWave);
  shift_OneLine(currentNum);
  shift_OneLine(0x00);                  //Blank value protects against ghosting
  squareWave = !squareWave;
}

void readRPM (void) {
  if (halfRevolutions >= 2) {
    timePassed = (millis() - prevTime);
    RPM = (30000.0 / timePassed) * halfRevolutions;
    DisplayRPM = RPM + 0.5;            //Rounds the RPM float value
    prevTime = millis();               //Saves current millis in prevTime
    halfRevolutions = 0;
    L1 = segmentMap[DisplayRPM % 10];
    L2 = segmentMap[DisplayRPM / 10];
  }
}

void INT0_HS (void) {
  halfRevolutions++;
}

void definePins(void) {
  DDRB |= (1 << PB4) | (1 << PB3) | (1 << PB1) | (1 << PB0); //Outputs
  DDRB &= ~(1 << PB2);                  //Input
  PORTB |= (1 << PB2);                  //Inernal pullup
  //  configINT0();                       //Doesn't work
  attachInterrupt(0, INT0_HS, FALLING);
}

void configINT0(void) {
  SREG  |= (1 << SREG_I);               //Enable global interrupts
  MCUCR |= (1 << ISC01) | (1 << ISC00); //INT0 triggered on rising edge
  GIMSK |= (1 << INT0);                 //Enable interrupt on INT0 (PB2)
}

void shift_OneLine (uint8_t value) {
  digitalWrite(latchPin, LOW);          // opens shift register outputs to new values
  shiftOut(dataPin, clockPin, LSBFIRST, value);
  digitalWrite(latchPin, HIGH);         // closes shift register outputs to new values
  delay(1);
}
