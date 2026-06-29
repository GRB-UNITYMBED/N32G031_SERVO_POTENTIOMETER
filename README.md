# N32G031 — Real-Time Servo Control via Potentiometer

![servo](doc/images/Servo_Potentiometer_100k.png)

An interactive automation project designed to teach analog signal reading and hardware mapping using the **Nations N32G031** (Cortex-M0). This project highlights how to control a servo motor's position in real-time by reading a potentiometer (knob) via the internal ADC (Analog-to-Digital Converter).

This project is fully optimized for cross-platform workflows. No complex vendor IDEs or OS-specific setups are required. Open it directly in our IDE, plug in your board, and flash instantly.


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

Debug probe: Any **CMSIS-DAP** adapter over **SWD** (Supported out-of-the-box).

---

## Build and Flash (Universal Cross-Platform)

Whether you are using **Windows** or **macOS**, our integrated IDE environment handles the heavy lifting behind the scenes.

1. **Open Project:** Open this project folder directly in the IDE.
2. **Build & Flash:** Simply click the **Build** and **Flash** button on the interface.
   * *Manual alternative:* Run `make` to compile, and use the custom batch routine to flash your image in less than 2 seconds.

---

## AI Prompting Guide in our IDE

Students can use the built-in AI Assistant in the IDE to explore, debug, and modify this project. Try pasting these example prompts into the AI Chat window:

* **To Learn:** `"Explain how the mathematical formula 'PULSE_MIN + ((knob_value * (PULSE_MAX - PULSE_MIN)) / KNOB_MAX)' translates the 12-bit ADC reading into the correct servo angle."`
* **To Experiment:** `"How can I modify the Read_Knob() function to average 10 readings so the servo motor stops jittering when my hand is still?"`
* **To Debug:** `"My ADC is always returning 4095 or 0. What could be wrong with my potentiometer wiring on PA0?"`

---

## Behaviour

| State | Actuator Response |
| :--- | :--- |
| **Knob Turned Left** | ADC reads closer to 0, servo moves smoothly towards 0° (Pulse: 2250). |
| **Knob Turned Right** | ADC reads closer to 4095, servo moves smoothly towards 180° (Pulse: 5500). |
| **Real-time Tracking** | Servo arm continuously mirrors the potentiometer's exact position instantly. |

---

## Layout

```
src/              Application source code (main.c, servo.c, utils.c, system.c)
inc/              Project header configurations (servo.h, n32g031_conf.h)
drivers/          Nations N32G031 low-level peripheral library
startup/          startup_n32g031_gcc.s (Vector table and Reset_Handler)
filemain/         Alternate code revisions kept for classroom reference
n32g031_flash.ld  Linker script configuration (64K Flash, 8K RAM)
openocd.cfg       Universal flashing routines and target setup
Makefile          Cross-platform compiler orchestration layout
unitymbed.json    Project architecture manifest
```

---

Part of the [UnityMbed](https://github.com/GRB-UNITYMBED) N32G031 example set.
