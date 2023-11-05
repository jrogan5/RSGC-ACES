// PROJECT  :2.2. Persistence of Vision
// PURPOSE  : To use input from the serial monitor to output two characters onto a dual 14 segment display.
// COURSE   :ICS3U
// AUTHOR   :J. Rogan
// DATE     :2021 11 13
// MCU      :328p
// STATUS   :Working
// REFERENCE:http://darcy.rsgc.on.ca/ACES/TEI3M/1920/Tasks.html#PoV
// Segment Map:
//           http://darcy.rsgc.on.ca/ACES/TEI3M/images/16SegmentCharMap.png

uint16_t segmentMap[] = {                   //array defines what state outputs should be for each uppercase character of alphabet
  //ABCDEFGHJKLMNP__     : Segment order (from bit 0 to bit 15)
  0b1110110010001000,  //A 0                 //'0b' defines the value as binary
  0b1111001010100000,  //B
  0b1001110000000000,  //C
  0b1111001000100000,  //D
  0b1001110010001000,  //E
  0b1000110010001000,  //F 5
  0b1011110010000000,  //G
  0b0110110010001000,  //H
  0b1001001000100000,  //I
  0b0111100000000000,  //J
  0b0000110101001000,  //K 10
  0b0001110000000000,  //L
  0b0110110100000100,  //M
  0b0110110001000100,  //N
  0b1111110000000000,  //O
  0b1100110010001000,  //P 15
  0b1111110001000000,  //Q
  0b1100110011001000,  //R
  0b1011010010001000,  //S
  0b1000001000100000,  //T
  0b0111110000000000,  //U 20
  0b0000110100010000,  //V
  0b0110110001010000,  //W
  0b0000000101010100,  //X
  0b0000000100100100,  //Y
  0b1001000100010000,  //Z 25
  0b0001100000001100,  //e              // lowercase noble gas characters
  0b0111000101100000,  //g
  0b0000100000010100,  //n
  0b0000100000000100,  //r
};
uint8_t dataPin   = 12;                    // 595 pinout
uint8_t oePin     = 11;                    // ''
uint8_t latchPin  = 10;                    // ''
uint8_t clockPin  = 9;                     // ''
uint8_t sizeMap   = sizeof(segmentMap) >> 1;  // number of entries in array
uint8_t povPin    = 6;                     // base pins of transistors for  PoV effect
bool squareWave;                           // boolean: either true or false
String input;                              // ('A' + 'B') consant string
uint8_t t         = 1;                     // time delay between shifting out values
uint16_t d1 = 14;                          // Starts by displaying 'O' on left display
uint16_t d2 = 13;                          // Starts by displaying 'N' on right display
uint16_t currentLetter;                    // value in storage register

void setup() {
  Serial.begin(9600);                       // begins serial monitor at 9600 baud
  pinMode(clockPin, OUTPUT);                // defines values as I/O pins
  pinMode(povPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(oePin, OUTPUT);
  pinMode(squareWave, OUTPUT);
  digitalWrite(squareWave, LOW);
}
void loop() {
  Serial.println("Write two letters");
  if (Serial.available()) {             // reads character input
    input = Serial.readString();     // read values as a string. ex/ AB –> 'A' + 'B'
    input.toUpperCase();             // converts input to ASCII uppercase letters
    Serial.print("Now displaying: ");
    d1 = (input.charAt(0) - 'A');    // subtracts 'A' from string char to get seg map value
    d2 = (input.charAt(1) - 'A');    // ''
    Serial.print(char(d1 + 'A'));    // converts number back to ASCII uppercase char
    Serial.println(char(d2 + 'A'));  // ''
  }                                  // end read input
  for (uint8_t i = 'A'; i <= 'Z'; i++) {   // scramble quickly runs through all letters
    digitalWrite(povPin, squareWave);
    digitalWrite(latchPin, LOW);     // allows bits to be pushed into storage registrer from shift register
    digitalWrite(oePin, HIGH);       // while high, no output allowed
    shiftOut(dataPin, clockPin, LSBFIRST, lowByte(segmentMap[i - 'A']));  // operation always gets a value between 0 and 25
    shiftOut(dataPin, clockPin, LSBFIRST, highByte(segmentMap[i - 'A']));
    digitalWrite(latchPin, HIGH);    // locks storage register and outputs
    digitalWrite(oePin, LOW);        // while low, output allowed
    delay(20);
    squareWave = LOW ? (squareWave == HIGH) : (squareWave == LOW);  // whatever state the pov pin is, this makes it opposite
  }                                  // end scramble
  while (!Serial.available()) {      // while there is no new value in serial monitor
    if (squareWave == HIGH) {        // diff char displayed depending on wave state
      (currentLetter = segmentMap[d1]);
    }
    else {
      (currentLetter = segmentMap[d2]);
    }
    digitalWrite(povPin, squareWave);
    digitalWrite(latchPin, LOW);     // allows bits to be pushed into storage registrer from shift register
    digitalWrite(oePin, HIGH);       // while high, no output allowed
    shiftOut(dataPin, clockPin, LSBFIRST, lowByte(currentLetter));
    shiftOut(dataPin, clockPin, LSBFIRST, highByte(currentLetter));
    digitalWrite(latchPin, HIGH);    // locks storage register and outputs
    digitalWrite(oePin, LOW);        // while low, output allowed
    delay(t);
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, 0b00000000);  // prevents ghosting
    shiftOut(dataPin, clockPin, LSBFIRST, 0b00000000);
    digitalWrite(latchPin, HIGH);
    squareWave = LOW ? (squareWave == HIGH) : (squareWave == LOW);  
  }

}

// This is an attempt at inputting of the name of a Noble Gas
//   and outputting its Periodic Table symbol. 
// I couldn't get it working, but here is the closest I got:
// 
//    if (input.charAt(2) > 0) {
//        input = Serial.readString();
//        input.toUpperCase();                    // converts input to ASCII uppercase lette
