# Smart AI Voice Assistant & Emergency SOS System for Elderly Care (ESA Project)

[![Platform: ESP32-S3](https://img.shields.io/badge/Platform-ESP32--S3-blue.svg)](https://www.espressif.com/)
[![ESP-IDF: v6.0+](https://img.shields.io/badge/ESP--IDF-v6.0.2-green.svg)](https://github.com/espressif/esp-idf)
[![Audio: I2S INMP441 & MAX98357A](https://img.shields.io/badge/Audio-I2S_Simplex-orange.svg)]()
[![Alert: Telegram Bot HTTPS](https://img.shields.io/badge/Alert-Telegram_Bot-2CA5E0.svg)](https://core.telegram.org/bots/api)

An autonomous AI-powered desktop voice companion and life-safety system designed specifically for elderly individuals living independently. It pairs conversational LLM voice capabilities with instant emergency physical SOS button triggers and voice emergency word recognition that dispatches real-time alerts to caretakers via Telegram.

---

## 🌟 Key Features

1. **Natural Voice Interaction**:
   - Real-time conversational AI powered by large language models (LLM) via the XiaoZhi backend.
   - High-fidelity voice synthesis and live Speech-to-Text transcription.
2. **Physical SOS Emergency Trigger**:
   - Dedicated hardware button on **GPIO 7** (tied to GND).
   - When pressed, initiates an authenticated HTTPS request to Telegram Bot API with root CA validation (`esp_crt_bundle_attach`), instantly notifying family or caretakers.
3. **Emergency Voice Keyword Detection**:
   - Continuously scans live speech transcripts for critical distress keywords (e.g., *"help"*, *"emergency"*, *"fall"*, *"fell down"*, *"call ambulance"*).
   - Automatically dispatches Telegram alerts when distress phrases are spoken.
4. **Resilient I2S Audio Pipeline**:
   - **INMP441 Digital MEMS Microphone**: Configured with 64-bit compliant stereo I2S clocking ($1.024\text{ MHz}$) and dynamic channel auto-detection (works whether `L/R` is tied to GND or 3.3V).
   - **MAX98357A I2S Class-D Amplifier**: Dedicated independent TX channel driving a 3W 4–8Ω speaker with volume normalization and clipping prevention.
   - Headless architecture (no OLED required), eliminating I2C bus hangs and reducing power consumption.

---

## 🔌 Hardware Wiring & Pinout Guide

### 1. INMP441 I²S Digital Microphone (Input)
| INMP441 Pin | ESP32-S3 Pin | Notes |
| :--- | :--- | :--- |
| **VDD** | **3.3V (3V3)** | ⚠️ Do **not** connect to 5V |
| **GND** | **GND** | Common Ground |
| **L/R** | **GND** | Selects Left channel (or 3.3V for Right channel; firmware auto-detects) |
| **WS** | **GPIO 4** | Word Select / LRCLK ($16\text{ kHz}$) |
| **SCK** | **GPIO 5** | Continuous Bit Clock ($1.024\text{ MHz}$) |
| **SD** | **GPIO 6** | Serial Audio Data In |

### 2. MAX98357A I²S Amplifier & Speaker (Output)
| MAX98357A Pin | ESP32-S3 Pin | Notes |
| :--- | :--- | :--- |
| **VIN** | **5V** (or 3.3V) | 5V provides louder, cleaner volume |
| **GND** | **GND** | Common Ground |
| **DIN** | **GPIO 8** | Serial Audio Data Out |
| **BCLK** | **GPIO 9** | Bit Clock ($1.536\text{ MHz}$) |
| **LRC** | **GPIO 10** | Left/Right Word Clock ($24\text{ kHz}$) |
| **GAIN** | *Leave Floating* | Default 9dB gain (or connect to GND for 12dB) |
| **SD / SD_MODE** | *Leave Floating* | Default `(L+R)/2` mono average via onboard pull-up |
| **$+$ and $-$** | Speaker Terminals | Connect to 4Ω or 8Ω 3W speaker |

### 3. Emergency SOS Button
| Component | ESP32-S3 Pin | Notes |
| :--- | :--- | :--- |
| **Push Button (Terminal 1)** | **GPIO 7** | Internal pull-up enabled in software |
| **Push Button (Terminal 2)** | **GND** | Pressing grounds GPIO 7, firing emergency alert |

---

## 💻 How to Clone and Setup on a New Laptop

Follow these exact steps to set up, build, and flash this project on any Windows, macOS, or Linux computer.

### Step 1: Install Prerequisites
1. **Python 3.10 to 3.13**: Make sure Python is added to your system `PATH`.
2. **Git**: [Download Git](https://git-scm.com/downloads).
3. **ESP-IDF (v6.0.2 recommended, or v5.x)**:
   - **Windows**: Use the [ESP-IDF Windows Installer](https://dl.espressif.com/dl/esp-idf/) (select version `v6.0.2` or `v5.4`).
   - **Linux / macOS**: Follow the [standard Espressif manual setup](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/get-started/).

---

### Step 2: Clone This Repository
Open your terminal (PowerShell, Command Prompt, or Bash) and clone the repository:
```bash
git clone https://github.com/tejareddyk2007/esa-project.git
cd esa-project
```

---

### Step 3: Activate the ESP-IDF Environment
In your terminal, activate the ESP-IDF tools:

- **Windows PowerShell:**
  ```powershell
  . "C:\esp\v6.0.2\esp-idf\export.ps1"
  ```
- **Windows Command Prompt (CMD):**
  ```cmd
  C:\esp\v6.0.2\esp-idf\export.bat
  ```
- **Linux / macOS:**
  ```bash
  . $HOME/esp/esp-idf/export.sh
  ```

*(Verify the installation by running `idf.py --version`. You should see `ESP-IDF v6.0.2` or similar).*

---

### Step 4: Configure Wi-Fi & Telegram Credentials

#### Wi-Fi Credentials:
Edit [`main/boards/bread-compact-wifi/compact_wifi_board.cc`](file:///main/boards/bread-compact-wifi/compact_wifi_board.cc):
```cpp
auto& ssid_manager = SsidManager::GetInstance();
if (ssid_manager.GetSsidList().empty()) {
    ssid_manager.AddSsid("YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD");
}
```

#### Telegram Alert Bot Token & Chat ID:
Edit [`main/sos_controller.h`](file:///main/sos_controller.h):
```cpp
const char* const kTelegramBotToken = "YOUR_TELEGRAM_BOT_TOKEN";
const char* const kTelegramChatId   = "YOUR_TELEGRAM_CHAT_ID";
```

---

### Step 5: Build, Flash, and Monitor
Plug your ESP32-S3 into your laptop via USB. Check Device Manager (or `ls /dev/tty*`) to find your COM port (e.g. `COM6` or `/dev/ttyUSB0`).

1. **Set target chip to ESP32-S3:**
   ```bash
   idf.py set-target esp32s3
   ```
2. **Build the firmware:**
   ```bash
   idf.py build
   ```
3. **Flash and open serial monitor (replace `COM6` with your port):**
   ```bash
   idf.py -p COM6 flash monitor
   ```

*(To exit the serial monitor at any time, press `Ctrl + ]`)*.

---

### Step 6: XiaoZhi Console Setup (First Time Activation)
1. On boot, the terminal will print a **6-character verification code** or QR link.
2. Go to **[https://xiaozhi.me/console/agents](https://xiaozhi.me/console/agents)** in your web browser.
3. Click **Add Device** $\to$ Enter your verification code.
4. Once paired, your assistant will appear as **Online**.
5. Click **Configure** on your agent card to select voice tone, language prompt (e.g., English speaking assistant), and personality.

---

## 🧪 Testing the Device

1. **Voice Conversation**:
   - Press the **BOOT button (GPIO 0)** on your board.
   - The device enters `listening` (`聆听中...`).
   - Speak into the INMP441 microphone: *"What is the time right now?"*
   - The transcript appears live in the terminal (`>> What is the time right now?`), followed by the assistant's voice through the speaker (`<< It is ...`).
2. **Physical SOS Button**:
   - Press the push button connected to **GPIO 7**.
   - The terminal logs:
     ```text
     W (...) SosController: SOS ALERT SENT!
     I (...) SosController: Telegram alert sent! Status code: 200
     ```
   - An immediate emergency message arrives on the caretaker's Telegram phone app.

---

## 📂 Project Structure

```text
├── main/
│   ├── application.cc              # XiaoZhi core state machine & transcript handling
│   ├── sos_controller.h            # Telegram Bot HTTPS emergency dispatcher
│   ├── sos_controller.cc           # HTTPS client with root CA certificate bundle
│   ├── audio/
│   │   ├── codecs/
│   │   │   ├── no_audio_codec.cc   # I2S Simplex driver (INMP441 + MAX98357A)
│   │   │   └── no_audio_codec.h
│   │   └── audio_service.cc        # Audio processing, resampler, Opus encoder
│   └── boards/
│       └── bread-compact-wifi/
│           ├── compact_wifi_board.cc # Hardware board init & button handlers
│           └── config.h              # Pin assignments (GPIO 4, 5, 6, 7, 8, 9, 10)
├── scripts/
│   └── build.py                    # Multi-platform build automation script
├── sdkconfig                       # Active ESP-IDF configuration for ESP32-S3
└── README.md                       # Documentation & setup guide
```

---

## 🛡️ License
Licensed under the Apache License 2.0. Based on the XiaoZhi ESP32 open-source ecosystem.
