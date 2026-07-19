# Magic Button — Matter (ESP32-C6 Super Mini)

Interruptor Matter Wi-Fi (**On/Off Plugin Unit**) com **ESP-IDF + esp-matter**.


| Campo        | Valor          |
| ------------ | -------------- |
| Vendor name  | `CJC`          |
| Product name | `Magic Button` |


## Estrutura

```text
esp32-sw-matter-ota/
├── README.md
├── .gitignore
└── idf/                 # firmware ESP-IDF + esp-matter
    ├── CMakeLists.txt
    ├── partitions.csv
    ├── sdkconfig.defaults
    ├── sdkconfig.defaults.esp32c6
    └── main/
```



## Hardware


| Sinal        | GPIO   | Notas                                                                                        |
| ------------ | ------ | -------------------------------------------------------------------------------------------- |
| Interruptor  | **14** | Pull-up; LOW = fechado. Aceita **pulsador** ou **interruptor simples** (detecção automática) |
| LED status   | **15** | Espelha OnOff; Identify; feedback de decommission                                            |
| Decommission | **8**  | Só pulsador; hold ≥ **5 s** → factory reset Matter                                           |




### Detecção do interruptor (GPIO14)


| Hardware            | Detecção                           | Comportamento                             |
| ------------------- | ---------------------------------- | ----------------------------------------- |
| Pulsador            | Pressão curta e soltura (< 800 ms) | Cada clique **alterna** OnOff             |
| Interruptor simples | Pressão contínua ≥ 800 ms          | OnOff **segue** a posição física (LOW=ON) |


Interruptor simples já fechado no boot: detectado automaticamente após ~800 ms.

## Ambiente de stack da Espressif

Repos da Espressif:

```bash
source ~/esp-idf/export.sh
export PW_ACTIVATE_SKIP_CHECKS=1
source ~/esp-matter/export.sh
export IDF_CCACHE_ENABLE=1
```

Requer ESP-IDF **stable** e `esp-matter` compatível, com `ESP_MATTER_PATH` definido pelo `export.sh`.

## Build

Compilar o código:

```bash
# Opção A — script
bash scripts/build_wsl.sh

# Opção B — manual
source ~/esp-idf/export.sh
export PW_ACTIVATE_SKIP_CHECKS=1
source ~/esp-matter/export.sh
cd idf
idf.py set-target esp32c6
idf.py build
```



### Pré-requisito para máquinas com Windows

Caso utilize Windows, é necessário uma distro WSL 2 para ter plena compatibilidade com o ESP-IDF stable (ex. 5.5.4) e esp-matter exportáveis.

**Guia completo:** [docs/wsl-esp-idf-setup.md](docs/wsl-esp-idf-setup.md) (instalar Ubuntu, ESP-IDF, esp-matter e compilar o projeto).

## Pairing (após flash)

Valores padrão de lab Matter:

```text
chip-tool pairing ble-wifi <node_id> <ssid> <pass> 20202021 3840
```

Ou app Matter (Home Assistant / Google Home / Alexa) com QR/código do monitor.

## OTA (Over The Air)

O firmware já habilita `CONFIG_ENABLE_OTA_REQUESTOR` e usa partições `ota_0` / `ota_1`. Isso garante que o dispositivo possa ser atualizado pela rede sem fio.

Para um novo release: incrementar `PROJECT_VER_NUMBER` (e a string `PROJECT_VER`) em `idf/CMakeLists.txt`, gerar a imagem Matter OTA e publicar via Home Assistant (detalhes no plano de flash).

## Decommission

Manter GPIO8 em GND por ≥ 5 s: LED pisca rapidamente e o dispositivo faz `esp_matter::factory_reset()`. Remova o nó do hub e comissione novamente.