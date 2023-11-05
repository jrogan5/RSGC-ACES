//PROJECT : Register-Level Shiftout
//PURPOSE : Use the outshift function (in ICS4U23.h) with an ATtiny85
//AUTHOR  : J. Rogan
//DATE    : 11 Feb, 2022
//MCU     : ATtiny85
//COURSE  : ICS4U-E
//STATUS  : Working
//REFRENCE: http://darcy.rsgc.on.ca/ACES/TEI4M/2223/Tasks.html#RLShiftout
//
#include <ICS4U23.h>                        // includes a local library (denoted by " ")

#define clk (1<<PB2)
#define data (1<<PB1)
#define latch (1<<PB0)
#define LSBF 0
#define MSBF 1
uint32_t value1 = 0x1234ABCD;
uint32_t value2 = 0xAAAAAAAA;

void setup() {
  DDRB |= clk | data | latch;
}

void loop() {
  m_clearPin(&PORTB, latch);                        // & is the refrence operator and returns an address. 
  outShift(&PORTB, data, clk, MSBF, value1);      // In this case, the adress of PORTB. 
  m_setPin(&PORTB, latch);                          // A *ptr is contained within outShift and gets the value.
  delay(1000);
  for (int i = 0; i < 5; i++) {                     // Alternates 5 times using bitwise not (0xAA ---> 0x55)
    m_clearPin(&PORTB, latch);
    outShift(&PORTB, data, clk, MSBF, value2);
    m_setPin(&PORTB, latch);
    delay(100);

    m_clearPin(&PORTB, latch);                      // m_clearPin and m_setPin are macros created in ICS4U23  
    outShift(&PORTB, data, clk, MSBF, ~value2);
    m_setPin(&PORTB, latch);
    delay(100);
  }

  sweep8(&PORTB, data, clk, latch);
  sweep8(&PORTB, data, clk, latch);
  sweep16(&PORTB, data, clk, latch);

}
