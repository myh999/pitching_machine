#include "ArduinoMotorShieldR3.h"

#define TIMER_MAX 781 //OCR1A = [16 MHz / (2 * N * fDesired)] - 1, N is prescalar (1024)
//I put in a timer interrupt if you want one. Use the equation above and set TIMER_MAX to get fDesired.
//That is, it will call ISR(TIMER1_COMPA_vect) every 1/fDesired seconds. The default value gives 10 Hz.

ArduinoMotorShieldR3 md;

void setup()
{
  md.init();
  md.setSpeed2(0, 0);
  md.clearBrake2();
  pinMode(ENCODER_1, INPUT); // set ENCODER_1 to input
  pinMode(ENCODER_2, INPUT); // set ENCODER_2 to input
  InitializeInterrupt();
  interrupts();

  //test
  
  Serial.begin(115200); //115200 baud, 8 data bits, 1 stop bit, no parity, XON/XOFF flow control
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("");
  Serial.println("UW ECE Ideas Clinic Pitching Machine");
  Serial.println(analogRead(ENCODER_1));
  Serial.println(analogRead(ENCODER_2));
}
void loop()
{
  if (Serial.available()) {
    float speed = Serial.parseFloat();
    if (speed >= -1 && speed <= 1) {
      Serial.print("Setting speed to ");
      Serial.println(speed);
      md.setSpeed2(speed, speed);

      delay(1000);

      int analog1, analog2 = -999;

      analog1 = analogRead(ENCODER_1);
      analog2 = analogRead(ENCODER_2);
      Serial.print("Test: Analog1 is: ");
      Serial.println(analog1);
      Serial.print("Test: Analog2 is: ");
      Serial.println(analog2);
    } else {
      Serial.println("Error: Speed ranges from -1 to 1");
    }
  }
}
void InitializeInterrupt() //Don't mess with this function - it sets up the control registers for the IR sensor and timer interrupts
{
  cli();    // switch interrupts off while messing with their settings
  
  PCICR   = 0x02;   // Enable PCINT1 interrupt
  PCMSK1  = 0b00001100;
  
  PRR    &= ~0x04;   //Enable Timer/Counter1
  TCCR1A  = 0b00000000; //No output pins, WGM11 = 0, WGM10 = 0
  TCCR1B  = 0b00001101; //WGM12 = 1, WGM13 = 0, CS1 = 0b101 (clk/1024)
  OCR1A   = TIMER_MAX; //Set count
  TIMSK1 |= 0b00000010; //OCIE1A = 1 (interrupt enable for OCR1A)
  
  sei();    // turn interrupts back on
}

ISR(PCINT1_vect) //Encoder Interrupt Service Routine
{
//This will trigger each time either of the IR sensors experiences any change in state
}

ISR(TIMER1_COMPA_vect) //Timer Interrupt Service Routine
{
//This will trigger at a frequency determined by TIMER_MAX
}


