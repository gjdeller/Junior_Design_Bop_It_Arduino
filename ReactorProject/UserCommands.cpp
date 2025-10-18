#include "UserCommands.h"
#include "Pins.h"

void initUserCommands(){
  // Configures all pins and stuff for the commands
  pinMode(BTN1, INPUT_PULLUP);
}
// Command 1 Method: Returns 1 if the player succeeds (pressed the button in time or 0.0 if they fail)
float getCommand1(){

  unsigned long startTime = millis();

  const unsigned long timeout = 7000; // 7 seconds

  // iNITIALIZES SUCCESS AS FALSE (NO SUCCESS YET)
  bool success = false;

  // PRINTS OUT THE FIRST COMMAND
  Serial.println("Command 1: Select a reactor (press button within 7 seconds)");

  // STARTS A LOOP THAT RUNS UNITL 7 SECONDS HAVE PASSED (ESSENTIALLY A WHILE LOOP)
  while (millis() - startTime < timeout){
    // CHECKS IF BUTTON IS PRESSED
    if (digitalRead(BTN1) == LOW){
      // PRINTS OUT THAT REACTOR 1 WAS SELECTED
      Serial.println("Reactor 1 selected");
      // SETS SUCCESS BOOL TO TRUE
      success = true;
      break;
    }
    delay(10); // small delay to prevent issues
  }

  // IF SUCCESS IS FALSE PRINT OUT NO REACTOR
  if(!success){
    Serial.println("Time expired! No reactor selected.");
  }

  // RETURNS THE SUCCESS BOOL 
  return success ? 1.0 : 0.0;
}