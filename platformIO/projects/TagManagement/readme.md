# Tag Management

## Pins

| Function      | Pin(s) (GPIO #)                              | File                                                     |
|---------------|----------------------------------------------|----------------------------------------------------------|
| RFID readers  | 16, 3v3, gnd, 23 (MOSI), 19 (MISO), 18 (SCK) | rfid.cpp                                                 |
| LED ready pin | 14                                           | BGE_rfid.cpp - NOT IN USE                                |
| OLED Screen   | 22 (12C SCL), 21 (12C SDA), 2v2, gnd         | BGE_sceen.cpp - not configured, uses boards SCL/SDA pins |
