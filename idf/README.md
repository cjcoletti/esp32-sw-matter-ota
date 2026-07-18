# Magic Button — firmware IDF

Firmware Matter (**On/Off Plugin Unit**) para ESP32-C6 Super Mini.

Identidade: vendor **CJC**, product **Magic Button**, VID/PID de teste `0xFFF1` / `0x8000`, versão `1.0` (`PROJECT_VER_NUMBER` 1).

## Build

```bash
source ~/esp-idf/export.sh
export PW_ACTIVATE_SKIP_CHECKS=1
source ~/esp-matter/export.sh

cd idf
idf.py set-target esp32c6
idf.py build
```

Flash e testes na placa: ver [../README.md](../README.md) e o plano **Magic Button Flash**.

## Fontes

| Arquivo | Papel |
|---------|--------|
| `app_main.cpp` | Nó Matter, endpoint Plugin Unit, callbacks |
| `app_driver.cpp` | GPIO14 FSM, GPIO8 decommission, GPIO15 LED |
| `app_priv.h` | Pinos e API do driver |
| `CHIPProjectConfig.h` | Vendor/product names e VID/PID |
