# Como compilar após alterar o código

**Idioma / Language:** Português · [English](en/build.md)

Todas as compilações devem ser feitas no **WSL** (Ubuntu), não no PowerShell do Windows.

## Forma rápida (recomendada)

Na raiz do repositório:

```bash
bash scripts/build_wsl.sh
```

O script carrega ESP-IDF + esp-matter, define o target `esp32c6` e executa o build.

## Forma manual

Se você já abriu um terminal WSL novo (o `~/.bashrc` já exporta o ambiente), basta:

```bash
idf.py build
```

### Primeira vez (ou após apagar `build/`)

```bash
source ~/esp-idf/export.sh
export PW_ACTIVATE_SKIP_CHECKS=1
source ~/esp-matter/export.sh
export IDF_CCACHE_ENABLE=1

cd idf
idf.py set-target esp32c6   # só uma vez
idf.py build
```

O `set-target esp32c6` só precisa ser repetido se você apagou `idf/build/` ou mudou de chip.

## O que esperar

| Situação | Tempo típico |
|----------|----------------|
| Primeiro build | ~20–30 min (Matter é pesado) |
| Após mudanças em `.cpp` / `.h` | Poucos minutos (só recompila o que mudou) |

**Saída:** `idf/build/matter_magic_button.bin` (e demais imagens em `idf/build/`).

## Se algo der errado

Rebuild completo (útil após mudar `sdkconfig`, partições ou CMake):

```bash
cd idf
idf.py fullclean
idf.py set-target esp32c6
idf.py build
```

Use `fullclean` quando alterar configuração do projeto — **não** é necessário para edições normais de código C/C++.

## Dica de desempenho

Compilar a partir de `~/` (ex.: copiar o repo para `~/esp32-sw-matter-ota`) é mais rápido que `/mnt/c/...`. Editar no Cursor no Windows e compilar em `/mnt/c/...` funciona, mas builds incrementais ficam mais lentos.

## Flash

Este guia cobre **apenas build**. Para gravar na placa, veja [flash.md](flash.md) ([EN](en/flash.md)).

## Ver também

- [wsl-esp-idf-setup.md](wsl-esp-idf-setup.md) — instalação do ambiente WSL + ESP-IDF + esp-matter ([EN](en/wsl-esp-idf-setup.md))
- [../README.md](../README.md) — visão geral do projeto ([EN](../README.en.md))
