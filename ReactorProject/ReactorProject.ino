// ReactorProject.ino

#include "Potentiometer.h"
#include "ReactorPhysics.h"
#include "UserCommands.h"
#include "PhysicsConstants.h"
#include "TaskChecker.h"
#include "Pins.h"
#include "DisplayGauge.h"
#include "DisplayPowerGauge.h"
#include "LCDScreen.h"
#include "TouchSensor.h"

// -------- Global Objects --------
ReactorPhysics reactor;  // single global instance

// -------- Task State --------
TaskRequirements currentTask;
unsigned long taskStartTime = 0;
unsigned long taskDeadline  = 0;
bool taskActive = false;
float currentMaintainTarget = 0.5f;

const unsigned long HOLD_MS = 2000;
static bool conditionMet = false;
static unsigned long conditionMetSince = 0;
static unsigned long ledTick = 0;
static bool ledOn = false;
const unsigned long GREEN_BLINK_PERIOD_MS = 250;

bool ranCommand1 = false;
int totalScore = 0;
static bool eStopLatched = false;
static int estopLastRaw = HIGH;
static unsigned long estopChangeAt = 0;
const unsigned long ESTOP_DEBOUNCE_MS = 25;
unsigned long currentTaskTimeMs = TASK_TIME_MS;

// <<< NEW: Track how many Command 2 tasks completed
static int tasksCompleted = 0;

// ================= Setup =================
void setup() {
  Serial.begin(9600);

  initPotentiometer();
  initUserCommands();

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);

  pinMode(BTN4, INPUT_PULLUP);

  Gauge_Init();
  ControlRodGauge_Init();
  LCD_init();
  touchSensor_init();

  Gauge_SetValue(0);
  ControlRodGauge_SetValue(0);

  Serial.println("All systems initialized. Reactor Simulation running...");

  // <<< FIRST Command 1 at startup
  Serial.println("\n--- COMMAND 1 (Startup) ---");
  float pts = getCommand1(totalScore, currentTaskTimeMs);
  totalScore += pts;
  Serial.print("Score after Command 1: ");
  Serial.println(totalScore);
  ranCommand1 = true;  // <<< mark as done once
  delay(500);
}

// ================= Loop =================
void loop() {
  // <<< Command 1 is no longer gated by ranCommand1 here.
  // So this block is gone.

  // 2) Physics update & read sensors
  reactor.update();
  float currentRodInsertion = readRodInsertion(); 
  float currentK = reactor.k;
  float currentFissionRate = reactor.reactionRate;

  bool eStopPressed = (digitalRead(BTN4) == LOW);

  // 3) Task management
  if (taskActive) {
    unsigned long now = millis();

    int estopRaw = digitalRead(BTN4);
    if (estopRaw != estopLastRaw) {
      estopLastRaw = estopRaw;
      estopChangeAt = millis();
    }
    if (millis() - estopChangeAt > ESTOP_DEBOUNCE_MS) {
      if (estopRaw == LOW) {
        eStopLatched = true;
      }
    }

    // TO DO:
    // This is for if the task is to call your manager.
    // for now until we make a cover for the sensor if it is covered, then the light will turn green
    // Later it will be initialized to covered, and when the cover is lifted then the sensor will turn green
    // But here is the bool for now
 
    bool calledManager = readTouchSensor();

    bool okNow = checkTaskCompletion(currentTask, currentRodInsertion, currentK, currentMaintainTarget, eStopLatched, calledManager);

    if (okNow) {
      if (!conditionMet) {
        conditionMet = true;
        conditionMetSince = now;
      }
    } else {
      conditionMet = false;
    }

    bool successIfEndedNow = (conditionMet && (now - conditionMetSince >= HOLD_MS));

    if (!okNow) {
      digitalWrite(RED_LED_PIN, HIGH);
      digitalWrite(GREEN_LED_PIN, LOW);
    } 
    else if (!successIfEndedNow) {
      if (now - ledTick >= GREEN_BLINK_PERIOD_MS) {
        ledTick = now;
        ledOn = !ledOn;
      }
      digitalWrite(GREEN_LED_PIN, ledOn ? HIGH : LOW);
      digitalWrite(RED_LED_PIN, LOW);
    } else {
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
    }

    if ((long)(now - taskDeadline) >= 0) {
      if (successIfEndedNow) {
        Serial.println("TASK SUCCESSFUL! Score +1.");
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN, LOW);
        totalScore += 1;
        
        if (currentTaskTimeMs > 3000){
          currentTaskTimeMs -= 100;
          if (currentTaskTimeMs < 3000) currentTaskTimeMs = 3000;
        }

      } else {
        Serial.println("TASK FAILED! Time expired. Score reset.");
        digitalWrite(RED_LED_PIN, HIGH);
        digitalWrite(GREEN_LED_PIN, LOW);
        totalScore = 0;
        currentTaskTimeMs = TASK_TIME_MS;
        endLoop();
      }

      taskActive = false;
      delay(500);

      // <<< NEW: increment task counter and rerun Command 1 every 3 tasks
      tasksCompleted++;
      if (tasksCompleted % 3 == 0) {
        Serial.println("\n--- COMMAND 1 ---");
        float pts = getCommand1(totalScore, currentTaskTimeMs);
        totalScore += pts;
        Serial.print("Score after Command 1: ");
        Serial.println(totalScore);
        delay(400);
      }
    }
  }

  // Start Command 2
  if (!taskActive) {
    delay(500); 
    Serial.println("\n--- NEW TASK INITIATED ---");

    currentTask = getCommand2(totalScore, currentTaskTimeMs);

    if (currentTask.requiredK == E_STOP) {
      eStopLatched = false;
    }


    if (currentTask.requiredRodInsertion < 0.0f) {
      currentMaintainTarget = currentRodInsertion;
      Serial.print("Target: Hold rod at ");
      Serial.print(currentMaintainTarget * 100.0f, 1);
      Serial.println("%");
    }

    taskStartTime = millis();
    taskDeadline  = taskStartTime + currentTaskTimeMs;
    conditionMet = false;
    conditionMetSince = 0;
    ledTick = taskStartTime;
    ledOn = false;
    taskActive = true;

    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
  }

  // 5) Status output
  Serial.print(" | k_eff: ");
  Serial.print(currentK, 4);
  Serial.print(" | Score: ");
  Serial.print(totalScore, 1);
  Serial.print(" | Macroscopic Cross Section: ");
  Serial.print(reactor.macro);

  Gauge_SetValue(currentFissionRate / 1e6);
  float targetInsertion = (currentTask.requiredRodInsertion < 0.0f)
                          ? currentMaintainTarget
                          : currentTask.requiredRodInsertion;
  ControlRodGauge_SetValue(currentRodInsertion * 100.0f);
  Serial.print(" | Target Rod: ");
  Serial.print(targetInsertion * 100.0f, 1);
  Serial.print("%");

  if (taskActive) {
    long msLeft = (long)(taskDeadline - millis());
    if (msLeft < 0) msLeft = 0;
    Serial.print(" | Time: ");
    Serial.print(msLeft / 1000.0f, 1);
    Serial.print("s");
  }
  Serial.println();

  delay(100);
}

static void endLoop(){
  Serial.println("Task Failed -- Ending Simulator");
  LCD_Display("Task Failed! End Sim", 0, 0);

  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(GREEN_LED_PIN, LOW);

  while(true){
    delay(1000);
  }
}