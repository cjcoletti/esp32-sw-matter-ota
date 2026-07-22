# Guia: WSL + ESP-IDF + esp-matter (Ubuntu 24.04)

**Idioma / Language:** Português · [English](en/wsl-esp-idf-setup.md)

Ambiente de desenvolvimento para compilar o firmware **Magic Button** (ESP32-C6 Super Mini + Matter) no Windows via **WSL2**.

| Item | Valor recomendado |
|------|-------------------|
| Distro WSL | Ubuntu 24.04 LTS |
| ESP-IDF | **v5.5.4** (stable, compatível com o `dependencies.lock` do projeto) |
| esp-matter | branch **release/v1.4** ou **main** compatível com IDF 5.5.x |
| Target do firmware | `esp32c6` |

---

## 1. Instalar o WSL e o Ubuntu

No **PowerShell** ou **Terminal** do Windows (como administrador):

```powershell
wsl --install -d Ubuntu-24.04
```

Reinicie o PC se o instalador pedir.

Na primeira abertura do Ubuntu, crie **usuário** e **senha** Linux.

Verifique:

```powershell
wsl --list --verbose
```

Saída esperada (exemplo):

```text
  NAME            STATE   VERSION
* Ubuntu-24.04    Running 2
```

Se a versão padrão não for 2:

```powershell
wsl --set-default-version 2
```

---

## 2. Atualizar o Ubuntu e instalar dependências

Abra o Ubuntu (menu Iniciar ou `wsl` no terminal) e execute:

```bash
sudo apt update && sudo apt upgrade -y

sudo apt install -y \
  git wget curl flex bison gperf python3 python3-pip python3-venv \
  cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0 \
  pkg-config unzip
```

---

## 3. Clonar e instalar o ESP-IDF (v5.5.4)

Mantenha os SDKs em `~/` (home do Linux), **não** em `/mnt/c/` — builds ficam mais rápidos e evitam problemas de permissão.

```bash
cd ~
git clone -b v5.5.4 --recursive https://github.com/espressif/esp-idf.git
cd ~/esp-idf
./install.sh esp32c6
```

O `install.sh` baixa o toolchain RISC-V do ESP32-C6 e demais ferramentas. Pode levar vários minutos.

Teste o export:

```bash
source ~/esp-idf/export.sh
idf.py --version
```

---

## 4. Clonar e instalar o esp-matter

```bash
cd ~
git clone --recursive https://github.com/espressif/esp-matter.git
cd ~/esp-matter
git submodule update --init --depth 1
./install.sh
```

Se o branch `main` exigir outra versão do IDF, consulte o README do esp-matter ou use um release tag compatível com v5.5.4:

```bash
cd ~/esp-matter
git fetch --tags
# Exemplo: git checkout release/v1.4
git submodule update --init --depth 1
./install.sh
```

Teste o export:

```bash
source ~/esp-idf/export.sh
export PW_ACTIVATE_SKIP_CHECKS=1
source ~/esp-matter/export.sh
echo "IDF_PATH=$IDF_PATH"
echo "ESP_MATTER_PATH=$ESP_MATTER_PATH"
```

---

## 5. Variáveis de ambiente (cada terminal)

Adicione ao `~/.bashrc` para não repetir os `source` manualmente:

```bash
cat >> ~/.bashrc << 'EOF'

# ESP-IDF + esp-matter (Magic Button)
export IDF_CCACHE_ENABLE=1
export PW_ACTIVATE_SKIP_CHECKS=1
alias idfenv='source ~/esp-idf/export.sh && source ~/esp-matter/export.sh'
EOF
```

Recarregue:

```bash
source ~/.bashrc
idfenv
```

Em **cada terminal novo** antes de compilar:

```bash
idfenv
```

---

## 6. Obter o projeto Magic Button

### Opção A (Windows)

```bash
cd esp32-sw-matter-ota
```

Funciona para build, mas é mais lento que copiar para `~/`.

### Opção B — copiar para `~/` (recomendado para build frequente)

```bash
cp -a /mnt/caminho-do-repo/esp32-sw-matter-ota ~/esp32-sw-matter-ota
cd ~/esp32-sw-matter-ota
```

---

## 7. Compilar o firmware

Com o ambiente exportado (`idfenv`):

```bash
# Opção A — script do repositório
bash scripts/build_wsl.sh

# Opção B — manual
cd idf
idf.py set-target esp32c6
idf.py build
```

Sucesso: artefatos em `idf/build/` (ex.: `matter_magic_button.bin`).

---

## 8. USB da placa no WSL (para flash futuro)

Conectar a ESP32-C6 Super Mini na USB:

### No Windows

1. Instale [usbipd-win](https://github.com/dorssel/usbipd-win).
2. Conecte a placa via USB.
3. No PowerShell (admin):

```powershell
usbipd list
usbipd bind --busid <BUSID>
usbipd attach --wsl --busid <BUSID>
```

### No WSL

```bash
ls /dev/ttyACM* /dev/ttyUSB* 2>/dev/null
```

Flash:

```bash
idfenv
cd ~/esp32-sw-matter-ota/idf   # ou caminho do projeto
idf.py -p /dev/ttyACM0 erase_flash flash monitor
```

Substitua `/dev/ttyACM0` pela porta detectada.

---

## 9. Solução de problemas

| Sintoma | Possível causa | Ação |
|---------|----------------|------|
| `wsl: no distributions installed` | WSL sem distro | `wsl --install -d Ubuntu-24.04` |
| `Please set ESP_MATTER_PATH` | esp-matter não exportado | `source ~/esp-matter/export.sh` ou `idfenv` |
| `idf.py: command not found` | ESP-IDF não exportado | `source ~/esp-idf/export.sh` ou `idfenv` |
| Build muito lento em `/mnt/c` | FS do Windows via WSL | Copie o projeto para `~/` |
| Erro de permissão em `/mnt/c` | Metadados Windows | Use `~/` ou `git config core.fileMode false` no repo |
| `install.sh` falha em Python | venv / pip | `sudo apt install python3-venv python3-pip` |
| Submodule vazio | clone sem `--recursive` | `git submodule update --init --recursive` |
| Target errado | sdkconfig antigo | `cd idf && idf.py fullclean && idf.py set-target esp32c6` |

---

## 10. Checklist rápido

- [ ] Ubuntu 24.04 no WSL2 (`wsl --list --verbose`)
- [ ] Dependências `apt` instaladas
- [ ] `~/esp-idf` em v5.5.4 com `./install.sh esp32c6`
- [ ] `~/esp-matter` com `./install.sh`
- [ ] `idfenv` no `~/.bashrc`
- [ ] `bash scripts/build_wsl.sh` conclui sem erro
- [ ] (Futuro) usbipd + flash na Super Mini

---

## Referências

- [ESP-IDF — Get Started](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c6/get-started/index.html)
- [esp-matter — Developing](https://docs.espressif.com/projects/esp-matter/en/latest/esp32c6/developing.html)
- [usbipd-win](https://github.com/dorssel/usbipd-win)
- README do projeto: [../README.md](../README.md) ([EN](../README.en.md))
