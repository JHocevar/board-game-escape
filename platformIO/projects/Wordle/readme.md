# Notes

Make sure to upload the word list when you upload new code

## Pins expected


| Function      | Pin(s) (GPIO #)                                                 | File                                                     |
|---------------|-----------------------------------------------------------------|----------------------------------------------------------|
| RFID readers  | 17, 16, 4, 32, 5 (sda) 3v3, gnd, 23 (MOSI), 19 (MISO), 18 (SCK) | rfid.cpp                                                 |
| Button        | 26, gnd                                                         | game.h                                                   |
| LED Strip     | 33, VIN, gnd                                                    | BGE_led.h                                                |
| LED ready pin | 14, gnd                                                         | BGE_rfid.cpp - NOT IN USE                                |
| OLED Screen   | 22 (12C SCL), 21 (12C SDA), 2v2, gnd                            | BGE_sceen.cpp - not configured, uses boards SCL/SDA pins |



```bash
pio run --target upload && pio run --target uploadfs
```

you can monitor with 

```bash
pio device monitor
```

Or all together with 

```bash
pio run --target upload && pio run --target uploadfs && pio device monitor
```