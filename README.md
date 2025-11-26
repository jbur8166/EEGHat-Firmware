# EEG Hat Firmware (nRF52840, MDBT50Q-1MV2)

- nRF5 SDK: 17.1.0
- SoftDevice: s140 v7.2.0
- Board: Custom EEG hat (Raytac MDBT50Q-1MV2 + LIS3DH)
- LFCLK: external 32.768 kHz crystal (ABS07L-32.768KHZ-T), 20 ppm

Key config:
- `sdk_config.h`: `NRF_SDH_CLOCK_LF_SRC = NRF_CLOCK_LF_SRC_XTAL`
- `NRF_SDH_CLOCK_LF_RC_CTIV = 0`
- `NRF_SDH_CLOCK_LF_RC_TEMP_CTIV = 0`
- `NRF_SDH_CLOCK_LF_ACCURACY = NRF_CLOCK_LF_ACCURACY_20_PPM`

App:
- BLE NUS service (`DEVICE_NAME = "EEGhat"`).
- Streams LIS3DH accelerometer data over BLE at 20 Hz.
