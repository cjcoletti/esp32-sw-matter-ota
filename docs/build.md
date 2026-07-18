# Como compilar após alterar o código

Todas as compilações devem ser feitas no **WSL** (Ubuntu), não no PowerShell do Windows.

## Forma rápida (recomendada)

Na raiz do repositório:

```bash
cd /mnt/c/mnt/caminho-do-repo/esp32-sw-matter-ota
bash scripts/build_wsl.sh
```

O script carrega ESP-IDF + esp-matter, define o target `esp32c6` e executa o build.

## Forma manual

Se você já abriu um terminal WSL novo (o `~/.bashrc` já exporta o ambiente), basta:

```bash
cd /mnt/c/mnt/caminho-do-repo/esp32-sw-matter-ota/idf
idf.py build
```

### Primeira vez (ou após apagar `build/`)

```bash
source ~/esp-idf/export.sh
export PW_ACTIVATE_SKIP_CHECKS=1
source ~/esp-matter/export.sh
export IDF_CCACHE_ENABLE=1

cd /mnt/c/mnt/caminho-do-repo/esp32-sw-matter-ota/idf
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

Este projeto está no passo **só build**. Não execute `flash`, `erase_flash` nem `monitor` até o plano **Magic Button Flash**.

## Ver também

- [wsl-esp-idf-setup.md](wsl-esp-idf-setup.md) — instalação do ambiente WSL + ESP-IDF + esp-matter
- [../README.md](../README.md) — visão geral do projeto
