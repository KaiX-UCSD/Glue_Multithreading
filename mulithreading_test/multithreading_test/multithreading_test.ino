#define ledPin 9    // the pin that the PWM LED is attached to
#define blinkPin 10 // the pin the the blinking LED is attached to
#define blinkPin2 11

#include <Utility.h>
#include <TimedAction.h>

bool blinkOnOff = true;
bool blinkOnOff2 = true;

void timedactionBlink();
void timedactionBlink2();

TimedAction blinkThread = TimedAction(1000, timedactionBlink);
TimedAction blinkThread2 = TimedAction(1170, timedactionBlink2);

void timedactionBlink()
{
  digitalWrite(blinkPin, blinkOnOff ? HIGH : LOW);
  blinkOnOff = (blinkOnOff ? LOW : HIGH);
}
void timedactionBlink2()
{
  digitalWrite(blinkPin2, blinkOnOff2 ? HIGH : LOW);
  blinkOnOff2 = (blinkOnOff2 ? LOW : HIGH);
}

void setup ()
{
  pinMode(ledPin, OUTPUT);  // declare pin 9 to be an output:
  pinMode(blinkPin, OUTPUT); // declare pin 10 to be an output:
  pinMode(blinkPin2, OUTPUT);
}
 
void loop()
{
  for (int a=0; a<=150;a++)               //loop from 0 to 255
  {
    analogWrite(ledPin, a);               // set the brightness of pin 9:
    blinkThread.check();
    blinkThread2.check();
    delay(8);                             //wait for 8 microseconds            
  }
    for (int a=150; a>=0;a--)             //loop from 255 down to 0
  {
    analogWrite(ledPin, a);               // set the brightness of pin 9:
    blinkThread.check();
    blinkThread2.check();
    delay(8);                             //wait for 8 microseconds   
  }
  //delay(800);                             //wait for 800 microseconds  
}

