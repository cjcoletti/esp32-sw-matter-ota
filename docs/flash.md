# Como gravar (flash) o firmware na placa

Guia para gravar o **Magic Button** na **ESP32-C6 Super Mini** a partir do **WSL2** no Windows.

| Item | Valor |
|------|--------|
| Placa | ESP32-C6 Super Mini |
| Target IDF | `esp32c6` |
| Flash | 4 MB (`CONFIG_ESPTOOLPY_FLASHSIZE_4MB`) |
| Imagem principal | `idf/build/matter_magic_button.bin` |

---

## 1. Pré-requisitos

1. Ambiente WSL com ESP-IDF + esp-matter configurado — ver [wsl-esp-idf-setup.md](wsl-esp-idf-setup.md).
2. Firmware compilado com sucesso — ver [build.md](build.md).
3. Cabo USB **dados** (não só carga) conectando a placa ao PC.
4. Driver USB serial no Windows (a Super Mini costuma aparecer como **USB JTAG/serial**; o Windows 11 em geral já reconhece).

---

## 2. Expor a porta USB no WSL (usbipd)

O WSL não vê a USB do Windows automaticamente. Use [usbipd-win](https://github.com/dorssel/usbipd-win).

### Instalar usbipd (Windows)

No **PowerShell como administrador**:

```powershell
winget install dorssel.usbipd-win
```

Reabra o terminal após instalar.

### Conectar a placa ao WSL

1. Conecte a ESP32-C6 Super Mini via USB.
2. No **PowerShell como administrador**:

```powershell
usbipd list
```

Procure a linha da placa (ex.: `Espressif`, `USB JTAG/serial`, `ESP32-C6`). Anote o **BUSID** (ex.: `2-3`).

3. Na primeira vez, vincule o dispositivo:

```powershell
usbipd bind --busid 2-3
```

4. Anexe ao WSL (com o Ubuntu aberto ou parado — o WSL precisa existir):

```powershell
usbipd attach --wsl --busid 2-3
```

5. No **Ubuntu (WSL)**, confira a porta:

```bash
ls /dev/ttyACM* /dev/ttyUSB* 2>/dev/null
```

Na Super Mini o usual é **`/dev/ttyACM0`**. Guarde essa porta — use `-p` nos comandos abaixo.

### Desanexar quando terminar

No PowerShell (admin):

```powershell
usbipd detach --busid 2-3
```

---

## 3. Preparar o ambiente no WSL

Em um terminal Ubuntu:

```bash
source ~/esp-idf/export.sh
export PW_ACTIVATE_SKIP_CHECKS=1
source ~/esp-matter/export.sh
export IDF_CCACHE_ENABLE=1
```

Se o `~/.bashrc` já foi configurado, um terminal novo já carrega isso.

Vá para o diretório do firmware:

```bash
cd /mnt/caminho-do-repo/esp32-sw-matter-ota/idf
```

(Substitua pelo caminho do seu clone.)

---

## 4. Gravar o firmware

Defina a porta (ajuste se não for `ttyACM0`):

```bash
export ESPPORT=/dev/ttyACM0
```

### Primeira gravação (recomendado)

Apaga a flash inteira e grava tudo de novo — ideal na primeira vez ou após problemas de boot:

```bash
idf.py -p $ESPPORT erase_flash flash
```

### Gravações seguintes

Se já gravou antes e só alterou o firmware:

```bash
idf.py -p $ESPPORT flash
```

### Build + flash em um passo

Depois de mudar o código:

```bash
idf.py -p $ESPPORT build flash
```

### Monitor serial (logs e pairing Matter)

Para ver logs, QR code de comissionamento e mensagens de erro:

```bash
idf.py -p $ESPPORT monitor
```

Sair do monitor: **Ctrl + ]**

Build, flash e monitor juntos:

```bash
idf.py -p $ESPPORT build flash monitor
```

---

## 5. Modo download (se o flash falhar)

Se `idf.py flash` não encontrar a placa ou der timeout:

1. Mantenha **BOOT** pressionado.
2. Pressione e solte **RESET** (ou desconecte/reconecte USB).
3. Solte **BOOT**.
4. Repita `idf.py -p $ESPPORT flash`.

Na Super Mini os botões dependem do layout da placa; consulte o silk da sua versão.

---

## 6. O que deve acontecer após o flash

1. A placa reinicia e abre a **janela de comissionamento Matter** (BLE).
2. No monitor você deve ver logs do esp-matter e, quando aplicável, **QR code** ou **código manual** para pairing.
3. O **LED (GPIO15)** pode indicar estado / Identify.
4. O **interruptor (GPIO14)** e o **decommission (GPIO8)** passam a responder conforme o [README](../README.md).

---

## 7. Comissionar no Matter (após flash)

### App Matter (Home Assistant, Google Home, Alexa)

Use o QR ou o código numérico exibido no `idf.py monitor`.

No Home Assistant o dispositivo aparece como tomada (`switch` / `device_class=outlet`). Para ícone de interruptor: *Show as → Switch*.

### chip-tool (lab, se tiver host tools instalados)

Valores padrão de laboratório Matter:

```text
chip-tool pairing ble-wifi <node_id> <ssid> <pass> 20202021 3840
```

Substitua `<ssid>`, `<pass>` e `<node_id>` pelos seus valores.

---

## 8. Teste rápido de hardware

| GPIO | Função | Teste |
|------|--------|--------|
| 14 | Interruptor | Pulsador: clique alterna OnOff. Interruptor fixo: posição segue o estado |
| 15 | LED status | Acompanha OnOff / Identify |
| 8 | Decommission | Manter em GND ≥ 5 s → factory reset Matter (LED pisca rápido) |

Após decommission, remova o nó do hub e comissione de novo.

---

## 9. Solução de problemas

| Sintoma | Causa provável | Ação |
|---------|----------------|------|
| `Could not open /dev/ttyACM0` | USB não anexada ao WSL | `usbipd attach --wsl --busid <BUSID>` |
| Porta some após fechar WSL | Normal com usbipd | Anexar de novo com `usbipd attach` |
| `Permission denied` na porta | Permissão serial | `sudo usermod -aG dialout $USER` e reabrir o terminal, ou use `sudo idf.py ...` pontualmente |
| `Serial port not found` | Cabo só carga / driver | Troque cabo; confira no `usbipd list` |
| `Timed out waiting for packet header` | Placa não em download | Modo BOOT + RESET (secção 5) |
| `Please build project binary` | Sem build | `idf.py build` ou ver [build.md](build.md) |
| `Please set ESP_MATTER_PATH` | Ambiente não exportado | `source ~/esp-matter/export.sh` |
| Monitor sem QR / sem BLE | Já comissionado | Factory reset (GPIO8) ou `erase_flash` |
| Flash OK mas não conecta Wi-Fi | Credenciais no pairing | Refaça comissionamento com SSID/senha corretos |

### Permissão permanente da porta serial (opcional)

```bash
sudo usermod -aG dialout $USER
```

Feche e abra o terminal WSL para aplicar.

---

## 10. Checklist rápido

- [ ] `idf.py build` concluiu sem erro
- [ ] Placa conectada e visível em `usbipd list`
- [ ] `usbipd attach --wsl` feito
- [ ] `/dev/ttyACM0` (ou similar) existe no WSL
- [ ] `idf.py -p $ESPPORT erase_flash flash` OK
- [ ] `idf.py -p $ESPPORT monitor` mostra boot e janela de pairing
- [ ] Dispositivo comissionado no hub Matter

---

## Ver também

- [build.md](build.md) — compilar após alterar código
- [wsl-esp-idf-setup.md](wsl-esp-idf-setup.md) — instalação do ambiente
- [../README.md](../README.md) — hardware, OTA e decommission
