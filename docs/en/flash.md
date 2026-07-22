# Flashing the firmware to the board

**Language / Idioma:** [Português](../flash.md) · English

Guide to flash **Magic Button** on the **ESP32-C6 Super Mini** from **WSL2** on Windows.

| Item | Value |
|------|--------|
| Board | ESP32-C6 Super Mini |
| IDF target | `esp32c6` |
| Flash | 4 MB (`CONFIG_ESPTOOLPY_FLASHSIZE_4MB`) |
| Main image | `idf/build/matter_magic_button.bin` |

---

## 1. Prerequisites

1. WSL environment with ESP-IDF + esp-matter configured — see [wsl-esp-idf-setup.md](wsl-esp-idf-setup.md).
2. Firmware built successfully — see [build.md](build.md).
3. USB **data** cable (not charge-only) connecting the board to the PC.
4. USB serial driver on Windows (Super Mini usually appears as **USB JTAG/serial**; Windows 11 typically recognizes it).

---

## 2. Expose the USB port to WSL (usbipd)

WSL does not see Windows USB automatically. Use [usbipd-win](https://github.com/dorssel/usbipd-win).

### Install usbipd (Windows)

In **PowerShell as administrator**:

```powershell
winget install dorssel.usbipd-win
```

Reopen the terminal after installing.

### Connect the board to WSL

1. Connect the ESP32-C6 Super Mini via USB.
2. In **PowerShell as administrator**:

```powershell
usbipd list
```

Find the board line (e.g. `Espressif`, `USB JTAG/serial`, `ESP32-C6`). Note the **BUSID** (e.g. `2-3`).

3. First time only, bind the device:

```powershell
usbipd bind --busid 2-3
```

4. Attach to WSL (Ubuntu open or stopped — WSL must exist):

```powershell
usbipd attach --wsl --busid 2-3
```

5. In **Ubuntu (WSL)**, check the port:

```bash
ls /dev/ttyACM* /dev/ttyUSB* 2>/dev/null
```

On Super Mini the usual port is **`/dev/ttyACM0`**. Keep that port — use `-p` in the commands below.

### Detach when done

In PowerShell (admin):

```powershell
usbipd detach --busid 2-3
```

---

## 3. Prepare the WSL environment

In an Ubuntu terminal:

```bash
source ~/esp-idf/export.sh
export PW_ACTIVATE_SKIP_CHECKS=1
source ~/esp-matter/export.sh
export IDF_CCACHE_ENABLE=1
```

If `~/.bashrc` is configured, a new terminal loads this automatically.

Go to the firmware directory:

```bash
cd /path/to/repo/esp32-sw-matter-ota/idf
```

(Replace with your clone path.)

---

## 4. Flash the firmware

Set the port (adjust if not `ttyACM0`):

```bash
export ESPPORT=/dev/ttyACM0
```

### First flash (recommended)

Erases the entire flash and writes everything — ideal the first time or after boot issues:

```bash
idf.py -p $ESPPORT erase_flash flash
```

### Subsequent flashes

If you already flashed before and only changed firmware:

```bash
idf.py -p $ESPPORT flash
```

### Build + flash in one step

After changing code:

```bash
idf.py -p $ESPPORT build flash
```

### Serial monitor (logs and Matter pairing)

To see logs, commissioning QR code, and error messages:

```bash
idf.py -p $ESPPORT monitor
```

Exit monitor: **Ctrl + ]**

Build, flash, and monitor together:

```bash
idf.py -p $ESPPORT build flash monitor
```

---

## 5. Download mode (if flash fails)

If `idf.py flash` cannot find the board or times out:

1. Hold **BOOT** pressed.
2. Press and release **RESET** (or disconnect/reconnect USB).
3. Release **BOOT**.
4. Retry `idf.py -p $ESPPORT flash`.

On Super Mini the buttons depend on your board layout; check the silkscreen.

---

## 6. What should happen after flash

1. The board reboots and opens the **Matter commissioning window** (BLE).
2. The monitor should show esp-matter logs and, when applicable, a **QR code** or **manual pairing code**.
3. The **LED (GPIO15)** may indicate state / Identify.
4. The **switch (GPIO14)** and **decommission button (GPIO8)** respond as described in the [README](../../README.en.md).

---

## 7. Matter commissioning (after flash)

### Matter app (Home Assistant, Google Home, Alexa)

Use the QR or numeric code shown in `idf.py monitor`.

In Home Assistant the device appears as an outlet (`switch` / `device_class=outlet`). For a switch icon: *Show as → Switch*.

### chip-tool (lab, if you have host tools installed)

Default Matter lab values:

```text
chip-tool pairing ble-wifi <node_id> <ssid> <pass> 20202021 3840
```

Replace `<ssid>`, `<pass>`, and `<node_id>` with your values.

---

## 8. Quick hardware test

| GPIO | Function | Test |
|------|----------|------|
| 14 | Switch | Momentary: click toggles OnOff. Fixed switch: position follows state |
| 15 | Status LED | Follows OnOff / Identify |
| 8 | Decommission | Hold to GND ≥ 5 s → Matter factory reset (LED blinks fast) |

After decommission, remove the node from your hub and commission again.

---

## 9. Troubleshooting

| Symptom | Likely cause | Action |
|---------|--------------|--------|
| `Could not open /dev/ttyACM0` | USB not attached to WSL | `usbipd attach --wsl --busid <BUSID>` |
| Port disappears after closing WSL | Normal with usbipd | Attach again with `usbipd attach` |
| `Permission denied` on port | Serial permission | `sudo usermod -aG dialout $USER` and reopen terminal, or use `sudo idf.py ...` once |
| `Serial port not found` | Charge-only cable / driver | Swap cable; check `usbipd list` |
| `Timed out waiting for packet header` | Board not in download mode | BOOT + RESET mode (section 5) |
| `Please build project binary` | No build | `idf.py build` or see [build.md](build.md) |
| `Please set ESP_MATTER_PATH` | Environment not exported | `source ~/esp-matter/export.sh` |
| Monitor without QR / no BLE | Already commissioned | Factory reset (GPIO8) or `erase_flash` |
| Flash OK but no Wi-Fi | Pairing credentials | Re-commission with correct SSID/password |

### Permanent serial port permission (optional)

```bash
sudo usermod -aG dialout $USER
```

Close and reopen the WSL terminal to apply.

---

## 10. Quick checklist

- [ ] `idf.py build` completed without error
- [ ] Board connected and visible in `usbipd list`
- [ ] `usbipd attach --wsl` done
- [ ] `/dev/ttyACM0` (or similar) exists in WSL
- [ ] `idf.py -p $ESPPORT erase_flash flash` OK
- [ ] `idf.py -p $ESPPORT monitor` shows boot and pairing window
- [ ] Device commissioned on Matter hub

---

## See also

- [build.md](build.md) — build after code changes
- [wsl-esp-idf-setup.md](wsl-esp-idf-setup.md) — environment setup
- [../../README.en.md](../../README.en.md) — hardware, OTA, and decommission
