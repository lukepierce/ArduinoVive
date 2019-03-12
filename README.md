Configurations that can be tweaked:  
const unsigned int requiredReadings = 3;  // Required consecutive readings (higher = less false positives, more likely to miss an event)  
const unsigned int minimalReadDelay = 5;  // Delay between readings  
const unsigned int referenceResistor = 1000;  // Reference resisitor in the setup  
const unsigned int BowOutPin = 2;  // Digital out pin for Bow  
const unsigned int SwordOutPin = 3; // Digital out pin for sword  
const unsigned int resistReadPin = 0;  // Analog pin that reads resistance  
const unsigned int shortPulseLength = 50; // Length of short pulse sent to vive  
const unsigned int longPulseLength = 1000;  // Length of long pulse sent to vive  


Serial Printing Can be Enabled via uncommenting the serial under the sendEvent call.
