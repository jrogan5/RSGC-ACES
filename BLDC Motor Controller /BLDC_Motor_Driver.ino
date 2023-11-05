//PROJECT : Sensored Brushless Direct Current Motor Controller
//PURPOSE : Combine hall senor feedback with commutation using PCInterrupts.
//AUTHOR  : J. Rogan
//DATE    : 20 May, 2023
//MCU     : 328P (SMD)
//COURSE  : ICS4U-E
//STATUS  : Working (sort of)
//REFRENCE: https://electronoobs.com/eng_circuitos_tut19_2.php


#define potPin   (1 << PC0)  //I/O register definitions
#define SensorA  (1 << PB0)
#define SensorB  (1 << PB1)
#define SensorC  (1 << PB2)

#define AH    (1 << PD0)
#define AL    (1 << PD1)
#define BH    (1 << PD2)
#define BL    (1 << PD3)
#define CH    (1 << PD4)
#define CL    (1 << PD5)
#define test  (1 << PD6)

uint8_t comStep = 1;        //set a default comStep to align the if else ladder in the ISR
uint8_t Delay = 4000;       //Timing stuff
uint16_t previousTime = 0;
uint16_t currentTime = 0;

void setup() {
  DDRC  &= ~potPin;                            //Define sensor pins and potentiometer as inputs
  DDRB  &= ~(SensorA | SensorB | SensorC);
  DDRD  |= AH | AL | BH | BL | CH | CL | test; //Define gate driver conrtol pins as outputs on MCU
  PORTD &= ~(AH | AL | BH | BL | CH | CL | test);

  PCICR  |= (1 << PCIE0);   //enable PCMSK0 scan on hall sensor pins
  PCMSK0 |= (1 << PCINT0);  //Sensor A
  PCMSK0 |= (1 << PCINT1);  //Sensor B
  PCMSK0 |= (1 << PCINT2);  //Sensor C
  currentTime = micros();
}


void loop() {
  currentTime = micros();
  if (currentTime - previousTime >= Delay) {
    previousTime += Delay;

    switch (comStep) {
      //comStep_1 A-B
      case 1:
        PORTD = AH | BL;  //Current enters A and leaves B
        break;
      //comStep_2 C-B
      case 2:
        PORTD = CH | BL;  //Current enters C and leaves B
        break;
      //comStep_3 C-A
      case 3:
        PORTD = CH | AL;  //Current enters C and leaves A
        break;
      //comStep_4 B-A
      case 4:
        PORTD = BH | AL;  //Current enters B and leaves A
        break;
      //comStep_5 B-C
      case 5:
        PORTD = BH | CL;  //Current enters B and leaves C
        break;
      //comStep_6 A-C
      case 6:
        PORTD = AH | CL;  //Current enters A and leaves C
        break;
    }                     //end of switch
  }                       //end if
  Delay = map(analogRead(potPin), 0, 1024, 1, 4000); //Obtain delay with pot reading
}                         //Void end

//In order to determine the current rotor positon and commutation step,
//run this every time a hall sensor pin changes state:
ISR (PCINT0_vect) {
  if (  (PINB & (SensorB)) && comStep == 6  ) {
    comStep = 1;
  } else if (  (PINB & (SensorA & SensorB) && comStep == 1 )) {
    comStep = 2;
  } else if (  (PINB & (SensorA & SensorB)) && comStep == 2 ) {
    comStep = 3;
  } else if (  (PINB & SensorB) && comStep == 3 ) {
    comStep = 4;
  } else if (  (PINB & (SensorB & SensorC)) && comStep == 4 ) {
    comStep = 5;
  } else if (  (PINB & SensorA) && comStep == 5 ) {
    comStep = 6;
  }
}

//  void loop() {
//    PORTD = AH | BL;    //Running open loop, the code is simple!
//    delay(Delay);       //This loop completes one electrical cycle per iteration.
//    PORTD = AH | CL;
//    delay(Delay);
//    PORTD = BH | CL;
//    delay(Delay);
//    PORTD = BH | AL;
//    delay(Delay);
//    PORTD = CH | AL;
//    delay(Delay);
//    PORTD = CH | BL;
//    delay(Delay);
//  }//Void end
