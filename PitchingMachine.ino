#include "ArduinoMotorShieldR3.h"
#include <Console.h>

#define TIMER_MAX 781 //OCR1A = [16 MHz / (2 * N * fDesired)] - 1, N is prescalar (1024)
//I put in a timer interrupt if you want one. Use the equation above and set TIMER_MAX to get fDesired.
//That is, it will call ISR(TIMER1_COMPA_vect) every 1/fDesired seconds. The default value gives 10 Hz.

ArduinoMotorShieldR3 md;
float userSpeed;
float newSpeed;

void setup()
{
  Bridge.begin();
  Console.begin();
  md.init();
  md.setSpeed2(0, 0);
  md.clearBrake2();
  userSpeed = 0;
  newSpeed = 0;
  pinMode(ENCODER_1, INPUT); // set ENCODER_1 to input
  pinMode(ENCODER_2, INPUT); // set ENCODER_2 to input
  pinMode(A3, INPUT);
  pinMode(A2, INPUT);
  InitializeInterrupt();
  interrupts();
  //Serial.begin(115200); //115200 baud, 8 data bits, 1 stop bit, no parity, XON/XOFF flow control
  while (!Console/*Serial*/) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Console.println("");
  Console.println("UW ECE Ideas Clinic Pitching Machine");
}

void loop()
{
  /*while (Console.available() <= 0) {
      md.setSpeed2(255, 255);
  }
  md.setSpeed2(0, 0);*/
  if (Console.available() > 0) {
    Console.println("Available");
    //get input from console
    String input = "";
    char c = Console.read();
    while (Console.available() > 0 && c != '\n') {
      input += c;
      c = Console.read();
    }
    newSpeed = input.toFloat();
    if (newSpeed >= -1 && newSpeed <= 1) {
      userSpeed = newSpeed;
      Console.print("Set speed to ");
      Console.println(newSpeed);
    } else {
      Console.println("Invalid speed");
    }
  }
  md.setSpeed2(userSpeed, userSpeed);
  Console.print("A2: ");
  Console.print((int)analogRead(A2));
  Console.print(", A3: ");
  Console.println((int)analogRead(A3));
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
  Console.println("IR Interrupt Activated");
}

ISR(TIMER1_COMPA_vect) //Timer Interrupt Service Routine
{
//This will trigger at a frequency determined by TIMER_MAX
}


