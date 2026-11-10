# MeshCore fork – RAK12039/PMSA003I Support

## AI SLOP WARNING! - I needed this for a personal project I just wanted to show that it is possible and you could use this as inspiration for your own implementation. 

## Thank you, MeshCore developers for this amazing project you've built.

## Project overview

This project extends MeshCore environmental sensor support with the **RAK12039 particulate matter sensor**, based on the Plantower PMSA003I, running on a RAK4631 WisBlock platform with the RAK19007 base board.

The implementation provides low-power air-quality monitoring by periodically waking the sensor, allowing its fan to stabilise, collecting particulate matter measurements, and returning it to sleep between measurement cycles.

Sensor readings are integrated into the existing MeshCore environmental telemetry system using CayenneLPP.

## Changelog

- Added RAK12039 / PMSA003I sensor driver integration.
- Added I²C detection and initialisation at address `0x12`.
- Added support for 12 telemetry values:
  - PM1.0, PM2.5 and PM10 – standard particle measurements.
  - PM1.0, PM2.5 and PM10 – environmental particle measurements.
  - Particle counts for 0.3, 0.5, 1.0, 2.5, 5.0 and 10.0 µm.
- Added configurable compile-time support through `ENV_INCLUDE_RAK12039`.
- Implemented non-blocking sensor wake-up and sleep management.
- Added a 30-second warm-up period before collecting measurements.
- Configured a 10-minute wake-up interval.
- Added measurement caching to prevent unnecessary I²C communication during telemetry requests.
- Retained the last valid measurement if a subsequent sensor read fails.
- Kept the sensor supply enabled and used the SET pin to control sleep mode, avoiding power-rail switching and I²C detection problems.

## How it works

At startup, the WisBlock sensor power rail is enabled and the firmware allows the RAK12039 to initialise before scanning the I²C bus.

Once detected and initialised, the sensor starts its first measurement cycle immediately. The firmware waits 30 seconds for the PMSA003I fan and measurement system to stabilise before reading the sensor.

All 12 measurement values are then stored in memory. The firmware pulls the sensor's SET pin LOW to enter sleep mode.

The sensor remains asleep until the next measurement cycle, scheduled 10 minutes after its previous wake-up. The SET pin is then driven HIGH, starting another 30-second warm-up period.

The process repeats continuously without blocking the main MeshCore application loop.

## Telemetry and data handling

MeshCore telemetry requests do not directly communicate with the PMSA003I. Instead, the sensor manager returns the most recently cached measurements.

This means telemetry requests can be handled at any time without waking the sensor, interrupting its measurement schedule, or activating its fan.

If a measurement fails, the previous valid readings are retained rather than being replaced with zero values.

## Design goals

The main objectives of this implementation are:

- Low power consumption through reduced fan operating time.
- Reliable sensor initialisation and I²C communication.
- Non-blocking operation that does not interfere with MeshCore.
- Minimal changes to the existing environmental sensor architecture.
- Efficient telemetry handling through cached measurements.

## About MeshCore

MeshCore is a lightweight, portable C++ library that enables multi-hop packet routing for embedded projects using LoRa and other packet radios. It is designed for developers who want to create resilient, decentralized communication networks that work without the internet.

## 🔍 What is MeshCore?

MeshCore now supports a range of LoRa devices, allowing for easy flashing without the need to compile firmware manually. Users can flash a pre-built binary using tools like Adafruit ESPTool and interact with the network through a serial console.
MeshCore provides the ability to create wireless mesh networks, similar to Meshtastic and Reticulum but with a focus on lightweight multi-hop packet routing for embedded projects. Unlike Meshtastic, which is tailored for casual LoRa communication, or Reticulum, which offers advanced networking, MeshCore balances simplicity with scalability, making it ideal for custom embedded solutions, where devices (nodes) can communicate over long distances by relaying messages through intermediate nodes. This is especially useful in off-grid, emergency, or tactical situations where traditional communication infrastructure is unavailable.