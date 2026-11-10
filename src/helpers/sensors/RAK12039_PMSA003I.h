#pragma once

#include <Arduino.h>
#include <Wire.h>

#define PMSA003I_DEFAULT_ADDRESS 0x12
#define PMSA003I_REG_ID          0x00
#define PMSA003I_CHIP_ID         0x42

// PMSA003I's 32-byte response frame. Multi-byte values are decoded from
// big-endian wire order by the driver; version and error_code are bytes.
typedef struct __attribute__((packed)) {
  uint16_t header;
  uint16_t date_len;
  uint16_t pm10_standard;
  uint16_t pm25_standard;
  uint16_t pm100_standard;
  uint16_t pm10_env;
  uint16_t pm25_env;
  uint16_t pm100_env;
  uint16_t particles_03um;
  uint16_t particles_05um;
  uint16_t particles_10um;
  uint16_t particles_25um;
  uint16_t particles_50um;
  uint16_t particles_100um;
  uint8_t version;
  uint8_t error_code;
  uint16_t check_sum;
} PMSA_Data_t;
static_assert(sizeof(PMSA_Data_t) == 32, "PMSA003I data frame must be 32 bytes");

class RAK_PMSA003I {
public:
  explicit RAK_PMSA003I(byte addr = PMSA003I_DEFAULT_ADDRESS);
  bool begin(TwoWire &wirePort, uint8_t deviceAddress = PMSA003I_DEFAULT_ADDRESS);
  bool readDate(PMSA_Data_t *pmsa_data);

private:
  bool readRegister(uint8_t registerAddress, uint8_t *readData, uint8_t size);
  uint8_t _deviceAddress;
  TwoWire *_i2cPort = nullptr;
};
