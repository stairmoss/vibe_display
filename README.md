# Vibe Display

Vibe Display is a smart ESP32-based device that shows your currently playing Spotify track in real time on a TFT screen. It also supports on-screen notifications, making it more interactive and useful.

---

## Overview

Vibe Display connects to your Spotify account and displays:

- Current track name  
- Artist name  
- Playback status  
- Real-time notifications on the screen  

It is built using an ESP32 microcontroller and a TFT display, with optional buttons for playback control.

---

## Features

- Real-time Spotify track display  
- WiFi-enabled ESP32 system  
- SPI-based TFT screen interface  
- On-screen notification system  
- Playback control (play/pause, skip, previous)  
- Compact custom 3D-printed case  
- Clean and minimal UI  

---

## Hardware Requirements

- ESP32 Board (LOLIN C3 Mini recommended)  
- 1.8" TFT Display (ST7735 or ILI9341)  
- 3x Tactile Switches (optional)  
- M3 Heatset Inserts (for case)  
- Dupont wires or soldering setup  

---

## Software Requirements

- Arduino IDE  
- ESP32 Board Package  

### Required Libraries

- SpotifyEsp32  
- Adafruit GFX  
- Adafruit ST7735  
- ArduinoJson  
- WiFi (built-in)  
- SPI (built-in)  

---

## Setup Instructions

### 1. Spotify API Setup

1. Go to https://developer.spotify.com/dashboard  
2. Create a new app  
3. Copy:
   - Client ID  
   - Client Secret  
4. Add a redirect URI (can be temporary)
