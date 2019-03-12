enum EventID {RELEASE = 0, DRAW_BOW = 1, PULL_BOW = 2, DRAW_SWORD = 3, DRAW_QUIVER = 4, NULL_EVENT = 5};
enum OutputType {SINGLE_PULSE, DOUBLE_PULSE, LONG_PULSE};

struct EventType {
  EventID eventId;
  OutputType outputType;
  unsigned int outputPin;
  int resistance;
};

// Configuration Values
const unsigned int requiredReadings = 3;
const unsigned int minimalReadDelay = 5;// 2000; // Ms
const unsigned int referenceVoltage = 5;
const unsigned int referenceResistor = 1000;
const unsigned int BowOutPin = 2;
const unsigned int SwordOutPin = 3;
const unsigned int resistReadPin = 0;
const unsigned int shortPulseLength = 50; // Ms
const unsigned int longPulseLength = 1000;  // Ms

EventType events[] = {
  {RELEASE, DOUBLE_PULSE, SwordOutPin, 0},
  {DRAW_BOW, SINGLE_PULSE, BowOutPin,  2000},
  {PULL_BOW, DOUBLE_PULSE, BowOutPin, 3900},
  {DRAW_SWORD, SINGLE_PULSE, SwordOutPin, 7000},
  {DRAW_QUIVER, LONG_PULSE, BowOutPin, 10000}
};

int calculateResistance(float voltage, int refResistance) {
  return (int)(refResistance * ((referenceVoltage/voltage) - 1));
}

float voltageConversion(int analogValue) {
  return analogValue * (5.0/ 1023.0);
}

// Arduino will be busy during this time, hopefully we wont miss anything
// otherwise this will need a more advanced timer 
void sendPulse(unsigned int duration, unsigned int pin) {
  digitalWrite(pin, HIGH);
  delay(duration);
  digitalWrite(pin, LOW);
}

void sendEvent(const EventType& event) {
  if(event.outputType == SINGLE_PULSE) {
    sendPulse(shortPulseLength, event.outputPin);
  } else if (event.outputType == DOUBLE_PULSE) {
    sendPulse(shortPulseLength, event.outputPin);
    delay(shortPulseLength);
    sendPulse(shortPulseLength, event.outputPin);
  } else if (event.outputType == LONG_PULSE) {
    sendPulse(longPulseLength, event.outputPin);
  }
}

EventID getReadEvent(int resistanceValue) {
  for(int i = 0; i < 5; i++) {
    int lower = events[i].resistance - 1000;
    int upper = events[i].resistance + 1000;
    if(lower <= resistanceValue && resistanceValue <= upper)
      return i;
  }
  return NULL_EVENT;
}

void printEvent(unsigned int eventId) {
  enum EventID {RELEASE = 0, DRAW_BOW, PULL_BOW, DRAW_SWORD, DRAW_QUIVER, NULL_EVENT};
  if(eventId == RELEASE)
    Serial.println("Release");
  else if(eventId == DRAW_BOW)
    Serial.println("Draw Bow");
  else if(eventId == PULL_BOW)
    Serial.println("Pull Bow");
  else if(eventId == DRAW_SWORD)
    Serial.println("Draw Sword");
  else if(eventId == DRAW_QUIVER)
    Serial.println("Draw Quiver");
  else
    Serial.println("Null Event");
}

void setup() {
  pinMode(SwordOutPin, OUTPUT);
  pinMode(BowOutPin, OUTPUT);
  digitalWrite(SwordOutPin, LOW);
  digitalWrite(BowOutPin, LOW);
  Serial.begin(9600);
  Serial.println("Starting Arduino");
}

// Active Globals
unsigned int sequentialEventCount = 0;
EventID currentState = NULL_EVENT;

void loop() {
  // put your main code here, to run repeatedly:
  int sensorValue = analogRead(resistReadPin);
  float voltage = voltageConversion(sensorValue);
  int resistance = calculateResistance(voltage, referenceResistor);
  EventID readState = getReadEvent(resistance);

  // Figure out what event was read, if it was the same as previous
  if(readState == currentState) {
    sequentialEventCount++;
    sequentialEventCount = min(sequentialEventCount, 100);
  } else {
    sequentialEventCount = 0;
    currentState = readState;
  }
 
  // Look for two consequetive reads of the same event
  // before taking action
  if(sequentialEventCount == requiredReadings - 1 && currentState != NULL_EVENT) {
    sendEvent(events[currentState]); 
    /*
    Serial.print("Sending State: ");
    printEvent(currentState);
    Serial.print("Voltage: ");
    Serial.println(voltage);
    Serial.print("Resistance: ");
    Serial.println(resistance);
    Serial.print("CurrentState: ");
    Serial.println(currentState);
    */
  } else {
    delay(minimalReadDelay);
  }
}
