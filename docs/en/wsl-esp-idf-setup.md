# Guide: WSL + ESP-IDF + esp-matter (Ubuntu 24.04)

**Language / Idioma:** [Português](../wsl-esp-idf-setup.md) · English

Development environment to build the **Magic Button** firmware (ESP32-C6 Super Mini + Matter) on Windows via **WSL2**.

| Item | Recommended value |
|------|-------------------|
| WSL distro | Ubuntu 24.04 LTS |
| ESP-IDF | **v5.5.4** (stable, compatible with this project's `dependencies.lock`) |
| esp-matter | **release/v1.4** branch or **main** compatible with IDF 5.5.x |
| Firmware target | `esp32c6` |

---

## 1. Install WSL and Ubuntu

In **PowerShell** or **Terminal** on Windows (as administrator):

```powershell
wsl --install -d Ubuntu-24.04
```

Restart the PC if the installer asks.

On first Ubuntu launch, create a Linux **username** and **password**.

Verify:

```powershell
wsl --list --verbose
```

Expected output (example):

```text
  NAME            STATE   VERSION
* Ubuntu-24.04    Running 2
```

If the default version is not 2:

```powershell
wsl --set-default-version 2
```

---

## 2. Update Ubuntu and install dependencies

Open Ubuntu (Start menu or `wsl` in terminal) and run:

```bash
sudo apt update && sudo apt upgrade -y

sudo apt install -y \
  git wget curl flex bison gperf python3 python3-pip python3-venv \
  cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0 \
  pkg-config unzip
```

---

## 3. Clone and install ESP-IDF (v5.5.4)

Keep SDKs under `~/` (Linux home), **not** under `/mnt/c/` — builds are faster and permission issues are avoided.

```bash
cd ~
git clone -b v5.5.4 --recursive https://github.com/espressif/esp-idf.git
cd ~/esp-idf
./install.sh esp32c6
```

`install.sh` downloads the ESP32-C6 RISC-V toolchain and other tools. This can take several minutes.

Test the export:

```bash
source ~/esp-idf/export.sh
idf.py --version
```

---

## 4. Clone and install esp-matter

```bash
cd ~
git clone --recursive https://github.com/espressif/esp-matter.git
cd ~/esp-matter
git submodule update --init --depth 1
./install.sh
```

If `main` requires a different IDF version, check the esp-matter README or use a release tag compatible with v5.5.4:

```bash
cd ~/esp-matter
git fetch --tags
# Example: git checkout release/v1.4
git submodule update --init --depth 1
./install.sh
```

Test the export:

```bash
source ~/esp-idf/export.sh
export PW_ACTIVATE_SKIP_CHECKS=1
source ~/esp-matter/export.sh
echo "IDF_PATH=$IDF_PATH"
echo "ESP_MATTER_PATH=$ESP_MATTER_PATH"
```

---

## 5. Environment variables (each terminal)

Add to `~/.bashrc` so you do not repeat `source` manually:

```bash
cat >> ~/.bashrc << 'EOF'

# ESP-IDF + esp-matter (Magic Button)
export IDF_CCACHE_ENABLE=1
export PW_ACTIVATE_SKIP_CHECKS=1
alias idfenv='source ~/esp-idf/export.sh && source ~/esp-matter/export.sh'
EOF
```

Reload:

```bash
source ~/.bashrc
idfenv
```

In **each new terminal** before building:

```bash
idfenv
```

---

## 6. Get the Magic Button project

### Option A (Windows path)

```bash
cd esp32-sw-matter-ota
```

Works for builds, but copying to `~/` is faster.

### Option B — copy to `~/` (recommended for frequent builds)

```bash
cp -a /path/to/repo/esp32-sw-matter-ota ~/esp32-sw-matter-ota
cd ~/esp32-sw-matter-ota
```

---

## 7. Build the firmware

With the environment exported (`idfenv`):

```bash
# Option A — repository script
bash scripts/build_wsl.sh

# Option B — manual
cd idf
idf.py set-target esp32c6
idf.py build
```

Success: artifacts in `idf/build/` (e.g. `matter_magic_button.bin`).

---

## 8. Board USB in WSL (for future flash)

Connect the ESP32-C6 Super Mini via USB:

### On Windows

1. Install [usbipd-win](https://github.com/dorssel/usbipd-win).
2. Connect the board via USB.
3. In PowerShell (admin):

```powershell
usbipd list
usbipd bind --busid <BUSID>
usbipd attach --wsl --busid <BUSID>
```

### On WSL

```bash
ls /dev/ttyACM* /dev/ttyUSB* 2>/dev/null
```

Flash:

```bash
idfenv
cd ~/esp32-sw-matter-ota/idf   # or your project path
idf.py -p /dev/ttyACM0 erase_flash flash monitor
```

Replace `/dev/ttyACM0` with the detected port.

---

## 9. Troubleshooting

| Symptom | Possible cause | Action |
|---------|----------------|--------|
| `wsl: no distributions installed` | WSL without a distro | `wsl --install -d Ubuntu-24.04` |
| `Please set ESP_MATTER_PATH` | esp-matter not exported | `source ~/esp-matter/export.sh` or `idfenv` |
| `idf.py: command not found` | ESP-IDF not exported | `source ~/esp-idf/export.sh` or `idfenv` |
| Very slow build on `/mnt/c` | Windows FS via WSL | Copy project to `~/` |
| Permission error on `/mnt/c` | Windows metadata | Use `~/` or `git config core.fileMode false` in repo |
| `install.sh` Python failure | venv / pip | `sudo apt install python3-venv python3-pip` |
| Empty submodule | clone without `--recursive` | `git submodule update --init --recursive` |
| Wrong target | old sdkconfig | `cd idf && idf.py fullclean && idf.py set-target esp32c6` |

---

## 10. Quick checklist

- [ ] Ubuntu 24.04 on WSL2 (`wsl --list --verbose`)
- [ ] `apt` dependencies installed
- [ ] `~/esp-idf` at v5.5.4 with `./install.sh esp32c6`
- [ ] `~/esp-matter` with `./install.sh`
- [ ] `idfenv` in `~/.bashrc`
- [ ] `bash scripts/build_wsl.sh` completes without error
- [ ] (Future) usbipd + flash on Super Mini

---

## References

- [ESP-IDF — Get Started](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c6/get-started/index.html)
- [esp-matter — Developing](https://docs.espressif.com/projects/esp-matter/en/latest/esp32c6/developing.html)
- [usbipd-win](https://github.com/dorssel/usbipd-win)
- Project README: [../../README.en.md](../../README.en.md)
