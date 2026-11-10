/**
 * RAK12039 / Plantower PMSA003I I2C driver.
 */
#include "RAK12039_PMSA003I.h"

RAK_PMSA003I::RAK_PMSA003I(byte addr) : _deviceAddress(addr) {}

bool RAK_PMSA003I::begin(TwoWire &wirePort, uint8_t deviceAddress) {
  _deviceAddress = deviceAddress;
  _i2cPort = &wirePort;

  uint8_t sensor_id = 0;
  delay(100);
  if (!readRegister(PMSA003I_REG_ID, &sensor_id, 1)) return false;
  return sensor_id == PMSA003I_CHIP_ID;
}

bool RAK_PMSA003I::readDate(PMSA_Data_t *pmsa_data) {
  if (pmsa_data == nullptr || _i2cPort == nullptr) return false;

  uint8_t frame[32] = {};
  if (!readRegister(PMSA003I_REG_ID, frame, sizeof(frame))) return false;
  if (frame[0] != 0x42 || frame[1] != 0x4D) return false;

  uint16_t checksum = 0;
  for (uint8_t i = 0; i < 30; ++i) checksum += frame[i];
  const uint16_t received_checksum = (uint16_t(frame[30]) << 8) | frame[31];
  if (checksum != received_checksum) return false;

  // Convert the 16-bit values from big-endian wire order.
  uint16_t words[13];
  for (uint8_t i = 0; i < 13; ++i) {
    const uint8_t offset = 2 + i * 2;
    words[i] = (uint16_t(frame[offset]) << 8) | frame[offset + 1];
  }

  pmsa_data->header = 0x424D;
  pmsa_data->date_len = words[0];
  pmsa_data->pm10_standard = words[1];
  pmsa_data->pm25_standard = words[2];
  pmsa_data->pm100_standard = words[3];
  pmsa_data->pm10_env = words[4];
  pmsa_data->pm25_env = words[5];
  pmsa_data->pm100_env = words[6];
  pmsa_data->particles_03um = words[7];
  pmsa_data->particles_05um = words[8];
  pmsa_data->particles_10um = words[9];
  pmsa_data->particles_25um = words[10];
  pmsa_data->particles_50um = words[11];
  pmsa_data->particles_100um = words[12];
  pmsa_data->version = frame[28];
  pmsa_data->error_code = frame[29];
  pmsa_data->check_sum = received_checksum;
  return true;
}

bool RAK_PMSA003I::readRegister(uint8_t registerAddress, uint8_t *readData, uint8_t size) {
  if (_i2cPort == nullptr || readData == nullptr || size == 0) return false;

  _i2cPort->beginTransmission(_deviceAddress);
  _i2cPort->write(registerAddress);
  if (_i2cPort->endTransmission(false) != 0) return false;

  const uint8_t received = _i2cPort->requestFrom(_deviceAddress, size);
  if (received != size) {
    while (_i2cPort->available()) _i2cPort->read();
    return false;
  }

  for (uint8_t i = 0; i < size; ++i) {
    if (!_i2cPort->available()) return false;
    readData[i] = static_cast<uint8_t>(_i2cPort->read());
  }
  return true;
}
