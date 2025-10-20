# Nuclear Reactor Control Simulator (BOP IT! Edition)
**By:** Garrett Deller, Krish Patel, and Joseph Rivera  
**Platform:** Arduino Nano ESP32 (ESP32-S3)  
**Displays:** GC9A01A 240×240 Circular OLED (SPI)

---

## Overview
This project transforms the classic **BOP IT!** game into an **educational Light Water Reactor (LWR) control simulator**, where players act as reactor operators responding to timed commands.  
The simulator demonstrates key reactor physics principles such as:
- **Control Rod Insertion and Neutron Absorption**
- **Reactivity (k-effective) and Power Stability**
- **Radiation Activity**
- **Subcritical, Critical, and Supercritical States**
- **Meltdown Prevention and Human Response Time**

Real-time feedback is provided via **LEDs**, **Circular OLED Gauges**, **7-segment displays**, and **audio commands**.

---

## System Architecture

### **Microcontroller**
- **Arduino Nano ESP32 (ESP32-S3)**  
- Handles SPI communication, sensor input, game logic, and voice/audio feedback.

### **Display System**
- **3x Circular OLED Displays** (GC9A01A 240×240)
  - **Display 1:** Reactor Temperature (Thermistor Input)
  - **Display 2:** Radiation/Activity Gauge
  - **Display 3:** Power Output / Reactor State
- Driven over **SPI** using the **TFT_eSPI** or **Adafruit_GC9A01A** library.  
  Each display uses a shared SPI bus with dedicated **CS**, **DC**, and **RST** lines.

### **Input Controls**
| Component | Function |
|------------|-----------|
| **Slider Potentiometer** | Adjusts control rod insertion depth → affects fission cross-section |
| **3 Mechanical Keys** | Select which reactor to control |
| **Touch Sensor** | Two-factor authentication for reactor control (safety check) |
| **Reset Button** | Resets the game and simulation state |
| **Power Switch** | Main ON/OFF control |

### **Outputs**
| Output Type | Purpose |
|--------------|----------|
| **LED Indicators** | Green (stable), Orange (caution), Red (meltdown) |
| **LED Strip** | Represents overall system status |
| **7-Segment Display** | Displays player score |
| **Audio Commands** | Provides randomized instructions (e.g., “Increase Power!”, “Meltdown Emergency!”) |

---

## Gameplay and Logic

### **Command Set**
| Command | Description | Time Limit |
|----------|--------------|-------------|
| Increase Power | Withdraw rods (fewer neutrons absorbed) | 7 seconds |
| Decrease Power | Insert rods (more neutrons absorbed) | 7 seconds |
| Maintain Power | Hold position (k=1) | 7 seconds |
| Meltdown Emergency | Fully insert rods to stop reaction | 10 seconds |
| Select Reactor [1–3] | Choose which reactor to operate | 7 seconds |

### **Scoring System**
- **+1 point** for every successful command within the time limit.
- **Shorter command intervals** as score increases.
- Game ends after **99 successes** or any failure.
- Final score displayed on the 7-segment display.

---

## Reactor Physics Model

### **Core Equations**
- **Macroscopic Cross Section (Σₐ):**  
  `Σₐ = N * σₐ` (proportional to rod insertion)
- **Four-Factor Formula:**  
  `k_eff = η * f`  
  - η = Neutrons per fission (≈ 2.3 for U-235)  
  - f = Thermal Utilization Factor  
- **Fission Rate:**  
  `R = Φ * Σ_fuel`
- **Activity (Radiation Output):**  
  `A = λ * N`
- **Temperature:**  
  Linked to power; triggers **meltdown** if thresholds are exceeded.

### **Reactor States**
| State | Description | LED Color |
|--------|--------------|-----------|
| Subcritical | k < 1 → Power decreases | Orange |
| Critical | k = 1 → Stable power | Green |
| Supercritical | k > 1 → Power rises | Orange |
| Meltdown | Temperature threshold exceeded | Flashing Red |

---

## Software Features

### **Main Loop Pseudocode**
```cpp
loop() {
  if (!powerOn) return;
  if (resetPressed) resetGame();

  if (startButtonPressed) startGame();

  while (gameActive) {
    command = getRandomCommand();
    startCountdown(command);

    if (evaluateUserResponse(command)) {
      score++;
      shortenInterval();
    } else {
      handleFailure(command);
      break;
    }
  }
}
