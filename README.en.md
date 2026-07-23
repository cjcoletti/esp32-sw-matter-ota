# Magic Button — Matter (ESP32-C6 Super Mini)

[![Português](https://img.shields.io/badge/Language-Português-inactive)](README.md)[![English](https://img.shields.io/badge/_English-blue)](README.en.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Matter Wi-Fi switch (**On/Off Plugin Unit**) built with **ESP-IDF + esp-matter**.

## About this project

**Magic Button** is a **personal open-source project**, created as a **community contribution** for Matter/ESP32 developers.

- Works in a lab environment and can be **reused, studied, and adapted** freely.
- **Not a commercial product** — no official support, warranty, or Matter certification.
- Uses **test VID/PID** (`0xFFF1` / `0x8000`); **do not ship in a product for sale** without CSA certification and your own IDs.
- Built on [ESP-IDF](https://github.com/espressif/esp-idf) and [esp-matter](https://github.com/espressif/esp-matter) (Espressif licenses).

**Status:** working firmware for ESP32-C6 Super Mini — commissioning, On/Off, OTA, and decommission tested in a development environment.

| Field        | Value                                              |
| ------------ | -------------------------------------------------- |
| Vendor name  | `CJC`                                              |
| Product name | `Magic Button`                                     |
| VID/PID      | `0xFFF1` / `0x8000` (Matter test — non-commercial) |

## Structure

```text
esp32-sw-matter-ota/
├── README.md
├── README.en.md
├── CONTRIBUTING.md
├── CONTRIBUTING.en.md
├── SECURITY.md
├── LICENSE
├── .gitignore
├── .github/             # Issue templates
├── docs/                # guides (EN in docs/en/)
└── idf/                 # ESP-IDF + esp-matter firmware
    ├── CMakeLists.txt
    ├── partitions.csv
    ├── sdkconfig.defaults
    ├── sdkconfig.defaults.esp32c6
    └── main/
```

## Hardware

| Signal       | GPIO   | Notes                                                                                              |
| ------------ | ------ | -------------------------------------------------------------------------------------------------- |
| Switch       | **14** | Pull-up; LOW = closed. Supports **momentary button** or **simple switch** (auto-detection)         |
| Status LED   | **15** | Mirrors OnOff; Identify; decommission feedback                                                   |
| Decommission | **8**  | Momentary button only; hold ≥ **5 s** → Matter factory reset                                       |

### Switch detection (GPIO14)

| Hardware        | Detection                     | Behavior                                      |
| --------------- | ----------------------------- | --------------------------------------------- |
| Momentary button | Short press and release (< 800 ms) | Each click **toggles** OnOff             |
| Simple switch   | Continuous press ≥ 800 ms     | OnOff **follows** physical position (LOW=ON)  |

Simple switch already closed at boot: detected automatically after ~800 ms.

## 3D enclosure

Compact case for the **Magic Button** with **ESP32-C6 Super Mini** and Hilink mini PSU (parametric model **V16.3**).

CAD/STL files, dimensions, internal layout, print settings, and fasteners live in a dedicated repository:

**[esp32c6-supermini-enclosure](https://github.com/cjcoletti/esp32c6-supermini-enclosure)** — [English README](https://github.com/cjcoletti/esp32c6-supermini-enclosure/blob/main/README.en.md) · [Portuguese README](https://github.com/cjcoletti/esp32c6-supermini-enclosure/blob/main/README.md)

## Espressif stack environment

Espressif repos:

```bash
source ~/esp-idf/export.sh
export PW_ACTIVATE_SKIP_CHECKS=1
source ~/esp-matter/export.sh
export IDF_CCACHE_ENABLE=1
```

Requires ESP-IDF **stable** and a compatible `esp-matter`, with `ESP_MATTER_PATH` set by `export.sh`.

## Build

Build the firmware:

```bash
# Option A — script
bash scripts/build_wsl.sh

# Option B — manual
source ~/esp-idf/export.sh
export PW_ACTIVATE_SKIP_CHECKS=1
source ~/esp-matter/export.sh
cd idf
idf.py set-target esp32c6
idf.py build
```

### Windows prerequisite

On Windows, a WSL 2 distro is required for full compatibility with ESP-IDF stable (e.g. 5.5.4) and exportable esp-matter.

**Full guide:** [docs/en/wsl-esp-idf-setup.md](docs/en/wsl-esp-idf-setup.md) (install Ubuntu, ESP-IDF, esp-matter, and build the project). Portuguese version: [docs/wsl-esp-idf-setup.md](docs/wsl-esp-idf-setup.md).

## Pairing (after flash)

Default Matter **lab** values (do not use in production):

```text
chip-tool pairing ble-wifi <node_id> <ssid> <pass> 20202021 3840
```

Or use a Matter app (Home Assistant / Google Home / Alexa) with the QR/manual code from the monitor.

## OTA (Over The Air)

The firmware enables `CONFIG_ENABLE_OTA_REQUESTOR` and uses `ota_0` / `ota_1` partitions, allowing over-the-air updates.

For a new release: bump `PROJECT_VER_NUMBER` (and the `PROJECT_VER` string) in `idf/CMakeLists.txt`, generate the Matter OTA image, and publish via Home Assistant. See [docs/en/flash.md](docs/en/flash.md) for flashing and commissioning ([PT](docs/flash.md)).

## Decommission

Hold GPIO8 to GND for ≥ 5 s: the LED blinks rapidly and the device runs `esp_matter::factory_reset()`. Remove the node from your hub and commission again.

## Contributions

This project is maintained personally. **Suggestions and ideas are welcome** via [Issues](https://github.com/cjcoletti/esp32-sw-matter-ota/issues/new/choose)
(or Discussions). Please **do not send Pull Requests** — I prefer to implement changes
myself, at my own pace. You are free to fork under the MIT license.

See [CONTRIBUTING.en.md](CONTRIBUTING.en.md) for details.

## Security

See [SECURITY.md](SECURITY.md) to report vulnerabilities privately.

## License

Licensed under the [MIT License](LICENSE).

## Legal notice

This software is provided **"as is"**, without warranty of any kind. The author is not liable for damages arising from its use. Matter® is a trademark of the Connectivity Standards Alliance; this project is not affiliated with the CSA or Espressif.
