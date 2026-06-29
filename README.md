# N32G031_SERVO_POTENTIOMETER — Real-Time Servo Control

![servo](doc/images/Servo_Potentiometer_100k.png)

An interactive automation project designed to teach analog signal reading and hardware mapping using the **Nations N32G031** (Cortex-M0). This project highlights how to control a servo motor's position in real-time by reading a potentiometer (knob) via the internal ADC. This project is fully optimized for cross-platform workflows using UnityMbed.

---

## Wiring

| Device | Pin | N32G031 | Notes |
| :--- | :--- | :--- | :--- |
| **Potentiometer** | 🧡 Signal | **PA0** | Analog Input (ADC Channel 0) |
| (Analog Knob) | ❤️ VCC | 3.3V / 5V | Power supply for the knob |
| | 🤎 GND | GND | Common system ground |
| **Servo Motor** | 🧡 Signal | **PA1** | Control Signal Pin (PWM Bit-Banging) |
| (e.g., SG90) | ❤️ VCC | 5V | Motor main power supply (Requires stable 5V) |
| | 🤎 GND | GND | Common system ground |

---

## Behaviour & Execution

| State | Actuator Response |
| :--- | :--- |
| **Knob Turned Left** | ADC reads closer to 0, servo moves smoothly towards 0° (Pulse: 2250). |
| **Knob Turned Right** | ADC reads closer to 4095, servo moves smoothly towards 180° (Pulse: 5500). |
| **Real-time Tracking** | Servo arm continuously mirrors the potentiometer's exact position instantly. |

---

## Hardware Setup & Troubleshooting

* **Debug probe:** Any **CMSIS-DAP** adapter over **SWD** is supported out-of-the-box.
* **Jittery Servo:** If the servo motor jitters or shakes, ensure the 5V power supply is stable, as servo motors draw significant peak current when moving.

---

## Learning & AI Extension Ideas

Students can use the built-in AI Assistant in the IDE to explore, debug, and modify this project. Try pasting these example prompts:
* **To Learn:** `"Explain how the mathematical formula 'PULSE_MIN + ((knob_value * (PULSE_MAX - PULSE_MIN)) / KNOB_MAX)' translates the 12-bit ADC reading into the correct servo angle."`
* **To Experiment:** `"How can I modify the Read_Knob() function to average 10 readings so the servo motor stops jittering when my hand is still?"`
* **To Debug:** `"My ADC is always returning 4095 or 0. What could be wrong with my potentiometer wiring on PA0?"`

---

## Build and Flash (Universal Cross-Platform)
1. **Open Project:** Open this project folder directly in the IDE.
2. **Build & Flash:** Simply click the **Build** and **Flash** buttons on the interface.

---
Part of the [UnityMbed](https://github.com/GRB-UNITYMBED) N32G031 example set.
