float floatmap(float x, float in_min, float in_max, float out_min, float out_max){
  /*
    Takes an input x in the range of (0 to 4095), 
    and converts it to the equivalent range (0 to 3.3 volts) to get 
    the voltage that is going through the potentiometer into the pin 4. 
    This is then returned and used in the loop method to get the current 
    updated voltage value along with the analog value
  */
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void setup() {
  
  // Initialize serial com at 9600 bits per second:
  Serial.begin(9600);
  // set the ADC attenuation to 11 dB (up to 3.3V inout)
  analogSetAttenuation(ADC_11db);
}

void loop() {
  // Read GPIO Pin 4
  int analogValue = analogRead(4);
  // Rescale to potentiometer's voltage (from 0 V to 3.3 V);
  float voltage = floatmap(analogValue, 0, 4095, 0, 3.3);

  // Print out what we read:
  Serial.print("Analog: ");
  Serial.print(analogValue);
  Serial.print(", Voltage: ");
  Serial.println(voltage);
  delay(1000);
}
