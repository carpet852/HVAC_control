// Adapted from Adafruit tutorial
// https://learn.adafruit.com/ir-sensor/making-an-intervalometer

// --- SCA 20230729
// Sketch for Carrier A/C timer control, because the remote only have a 24h timer setting
// Arduino Uno with IR LED, IR sensor, button+LED
// IR codes captured by sketch ir_remote_decoder.ino
// --- SCA 20230826
// add OFF command repeat


////////////////////////////////////////////////////////////////////////////////////
const int IRledPin =  11;    // IR LED
const int ButtonPin = 13;    // button connected to GND
const int ledPin =  10;    // LED connected to GND

int Temp = 22;  // Temperature setting (degC)
unsigned long OnTimeSec = 60*30;  // ON time setting (sec)
unsigned long OffTimeSec = 60*60*5+60*30;  // OFF time setting (sec)
////////////////////////////////////////////////////////////////////////////////////

long DispTimeSec = 60;  // Refresh time for display (sec)
int RepeatCmd = 2;  // Nb of times OFF command is repeated
enum State {ON, OFF, KILL}; // FSM states
unsigned char ACState = ON; // Initial state
// use unsigned long for time variables: overflow 2^32-1 msec ~49 days
unsigned long OnTimeMillis = OnTimeSec*1000;  // ON time (msec)
unsigned long OffTimeMillis = OffTimeSec*1000;  // OFF time (msec)
unsigned long DispTimeMillis = DispTimeSec*1000;
unsigned long previousMillis = 0;
unsigned long previousDispMillis = 0;
int ledState = LOW;
int RepeatCmdCount = 0;

// The setup() method runs once, when the sketch starts
void setup()   {
  pinMode(IRledPin, OUTPUT);
  pinMode(ledPin, OUTPUT);     
  pinMode(ButtonPin, INPUT_PULLUP);
  Serial.begin(9600);
  delay(5000);
  Serial.print("Temperature setting (degC): ");
  Serial.println(Temp);
  Serial.print("Timer ON time setting (sec): ");
  Serial.println(OnTimeSec);
  Serial.print("Timer OFF time setting (sec): ");
  Serial.println(OffTimeSec);
  if (ACState == ON)  {
    Serial.println("Sending ON command");
    SendOn(Temp);
  }
  else  {
    Serial.println("Sending OFF command");
    SendOff();
  }
}


// main loop with FSM
void loop()                     
{
  // millis() returns the number of milliseconds passed since the program started
  unsigned long currentMillis = millis();
  unsigned int currentSec = currentMillis/1000;
  //unsigned int currentHours = currentSec/3600;
  unsigned int previousSec = previousMillis/1000;
  //unsigned int previousHours = previousSec/3600;
  if (digitalRead(ButtonPin) == LOW) {
    Serial.print("Time elapsed (sec): ");
    Serial.println(currentSec);
    Serial.println("Button KILL Pressed");
    Serial.println("Sending OFF command");
    SendOff();
    ACState = KILL;
    delay(1000);
  }
  switch (ACState) {
    case OFF:
      if (currentMillis - previousMillis >= OffTimeMillis) {
        previousMillis = currentMillis;
        previousDispMillis = currentMillis;
        Serial.print("Time elapsed (sec): ");
        Serial.println(currentSec);
        Serial.println("Sending ON command");
        SendOn(Temp);
        ACState = ON;
      }
      else if (currentMillis - previousDispMillis >= DispTimeMillis) {
        previousDispMillis = currentMillis;
        Serial.print("OFF state, time remaining before change (sec): ");
        Serial.println(OffTimeSec - currentSec + previousSec);
        RepeatCmdCount++;
        if (RepeatCmdCount <= RepeatCmd)  {
          Serial.println("Sending OFF command");  // repeat OFF command
          SendOff();
        }
      }
      break; // Continue with rest of the program
    case ON:
      if (currentMillis - previousMillis >= OnTimeMillis) {
        previousMillis = currentMillis;
        previousDispMillis = currentMillis;
        Serial.print("Time elapsed (sec): ");
        Serial.println(currentSec);
        Serial.println("Sending OFF command");
        SendOff();
        RepeatCmdCount = 0;
        ACState = OFF;
      }
      else if (currentMillis - previousDispMillis >= DispTimeMillis) {
        previousDispMillis = currentMillis;
        Serial.print("ON state, time remaining before change (sec): ");
        Serial.println(OnTimeSec - currentSec + previousSec);
      }
      break; // Continue with rest of the program
    case KILL:
      if (currentMillis - previousDispMillis >= DispTimeMillis) {
        previousDispMillis = currentMillis;
        Serial.println("KILL state, press RESET to restart timer");
        if (ledState == LOW)  {
          ledState = HIGH;
        }
        else  {
          ledState = LOW;
        }
        digitalWrite(ledPin, ledState);
      }
      break; // Continue with rest of the program
    default:
      // do nothing
      break; // Continue with rest of the program
  } 
}


// This procedure sends a 38KHz pulse to the IRledPin 
// for a certain # of microseconds. We'll use this whenever we need to send codes
void pulseIR(long microsecs) {
  // we'll count down from the number of microseconds we are told to wait
  cli();  // this turns off any background interrupts
 
  while (microsecs > 0) {
    // 38 kHz is about 13 microseconds high and 13 microseconds low
   digitalWrite(IRledPin, HIGH);  // this takes about 3 microseconds to happen
   delayMicroseconds(10);         // hang out for 10 microseconds, you can also change this to 9 if its not working
   digitalWrite(IRledPin, LOW);   // this also takes about 3 microseconds
   delayMicroseconds(10);         // hang out for 10 microseconds, you can also change this to 9 if its not working
   // so 26 microseconds altogether
   microsecs -= 26;
  }
  sei();  // this turns them back on
}


// send ON command to Carrier A/C (mode=cool, airflow=low, temp)
void SendOn(int temp) {
  switch (temp) {
    case 24:
      pulseIR(4580);
      delayMicroseconds(4460);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1660);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(560);
      delayMicroseconds(5400);
      pulseIR(4580);
      delayMicroseconds(4460);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(600);
      delayMicroseconds(540);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(620);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(540);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1660);
      pulseIR(600);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(560);
      break;
    case 23:
      pulseIR(4500);
      delayMicroseconds(4520);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(660);
      delayMicroseconds(1580);
      pulseIR(640);
      delayMicroseconds(500);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(640);
      delayMicroseconds(1620);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(640);
      delayMicroseconds(1620);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(640);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(660);
      delayMicroseconds(480);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(640);
      delayMicroseconds(1620);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(620);
      delayMicroseconds(1640);
      pulseIR(540);
      delayMicroseconds(5380);
      pulseIR(4600);
      delayMicroseconds(4460);
      pulseIR(620);
      delayMicroseconds(1600);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(660);
      delayMicroseconds(1600);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(640);
      delayMicroseconds(500);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(640);
      delayMicroseconds(500);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(620);
      delayMicroseconds(1600);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(660);
      delayMicroseconds(1580);
      pulseIR(620);
      delayMicroseconds(1640);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(640);
      delayMicroseconds(500);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(640);
      delayMicroseconds(480);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(660);
      delayMicroseconds(1600);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(640);
      delayMicroseconds(1600);
      pulseIR(560);
      break;
    case 22:
      pulseIR(4580);
      delayMicroseconds(4460);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1640);
      pulseIR(540);
      delayMicroseconds(5400);
      pulseIR(4580);
      delayMicroseconds(4460);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(620);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(1620);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(540);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(520);
      pulseIR(620);
      delayMicroseconds(500);
      pulseIR(620);
      delayMicroseconds(520);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(600);
      delayMicroseconds(1640);
      pulseIR(540);
      break;
    default:
      Serial.println("Error: no ON command available");
      break;
  }
}


// send OFF command to Carrier A/C
void SendOff() {
    pulseIR(4580);
    delayMicroseconds(4460);
    pulseIR(660);
    delayMicroseconds(1580);
    pulseIR(660);
    delayMicroseconds(460);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(660);
    delayMicroseconds(460);
    pulseIR(640);
    delayMicroseconds(500);
    pulseIR(620);
    delayMicroseconds(1620);
    pulseIR(620);
    delayMicroseconds(500);
    pulseIR(620);
    delayMicroseconds(500);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(640);
    delayMicroseconds(500);
    pulseIR(620);
    delayMicroseconds(500);
    pulseIR(660);
    delayMicroseconds(1580);
    pulseIR(660);
    delayMicroseconds(1580);
    pulseIR(660);
    delayMicroseconds(460);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(640);
    delayMicroseconds(500);
    pulseIR(620);
    delayMicroseconds(1620);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(620);
    delayMicroseconds(1620);
    pulseIR(620);
    delayMicroseconds(1620);
    pulseIR(660);
    delayMicroseconds(460);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(660);
    delayMicroseconds(1600);
    pulseIR(640);
    delayMicroseconds(480);
    pulseIR(620);
    delayMicroseconds(500);
    pulseIR(640);
    delayMicroseconds(480);
    pulseIR(660);
    delayMicroseconds(460);
    pulseIR(640);
    delayMicroseconds(1620);
    pulseIR(640);
    delayMicroseconds(460);
    pulseIR(640);
    delayMicroseconds(500);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(640);
    delayMicroseconds(500);
    pulseIR(620);
    delayMicroseconds(500);
    pulseIR(620);
    delayMicroseconds(500);
    pulseIR(620);
    delayMicroseconds(500);
    pulseIR(640);
    delayMicroseconds(480);
    pulseIR(640);
    delayMicroseconds(500);
    pulseIR(660);
    delayMicroseconds(460);
    pulseIR(640);
    delayMicroseconds(480);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(620);
    delayMicroseconds(1620);
    pulseIR(660);
    delayMicroseconds(1600);
    pulseIR(620);
    delayMicroseconds(1600);
    pulseIR(660);
    delayMicroseconds(1600);
    pulseIR(660);
    delayMicroseconds(5280);
    pulseIR(4600);
    delayMicroseconds(4460);
    pulseIR(640);
    delayMicroseconds(1580);
    pulseIR(660);
    delayMicroseconds(480);
    pulseIR(620);
    delayMicroseconds(1620);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(640);
    delayMicroseconds(480);
    pulseIR(640);
    delayMicroseconds(480);
    pulseIR(640);
    delayMicroseconds(1620);
    pulseIR(620);
    delayMicroseconds(500);
    pulseIR(660);
    delayMicroseconds(460);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(660);
    delayMicroseconds(480);
    pulseIR(620);
    delayMicroseconds(480);
    pulseIR(680);
    delayMicroseconds(1580);
    pulseIR(620);
    delayMicroseconds(1620);
    pulseIR(640);
    delayMicroseconds(480);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(640);
    delayMicroseconds(500);
    pulseIR(600);
    delayMicroseconds(1640);
    pulseIR(620);
    delayMicroseconds(1620);
    pulseIR(620);
    delayMicroseconds(1620);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(640);
    delayMicroseconds(480);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(640);
    delayMicroseconds(1620);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(640);
    delayMicroseconds(480);
    pulseIR(660);
    delayMicroseconds(460);
    pulseIR(640);
    delayMicroseconds(500);
    pulseIR(620);
    delayMicroseconds(500);
    pulseIR(620);
    delayMicroseconds(1620);
    pulseIR(640);
    delayMicroseconds(480);
    pulseIR(660);
    delayMicroseconds(460);
    pulseIR(640);
    delayMicroseconds(1620);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(620);
    delayMicroseconds(500);
    pulseIR(660);
    delayMicroseconds(460);
    pulseIR(640);
    delayMicroseconds(500);
    pulseIR(640);
    delayMicroseconds(480);
    pulseIR(620);
    delayMicroseconds(500);
    pulseIR(640);
    delayMicroseconds(480);
    pulseIR(640);
    delayMicroseconds(500);
    pulseIR(620);
    delayMicroseconds(500);
    pulseIR(620);
    delayMicroseconds(1620);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(640);
    delayMicroseconds(1600);
    pulseIR(620);
    delayMicroseconds(1640);
    pulseIR(620);
}
