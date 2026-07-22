# Building after code changes

**Language / Idioma:** [Português](../build.md) · English

All builds must run in **WSL** (Ubuntu), not in Windows PowerShell.

## Quick method (recommended)

From the repository root:

```bash
bash scripts/build_wsl.sh
```

The script loads ESP-IDF + esp-matter, sets target `esp32c6`, and runs the build.

## Manual method

If you already opened a new WSL terminal (`~/.bashrc` exports the environment), run:

```bash
idf.py build
```

### First time (or after deleting `build/`)

```bash
source ~/esp-idf/export.sh
export PW_ACTIVATE_SKIP_CHECKS=1
source ~/esp-matter/export.sh
export IDF_CCACHE_ENABLE=1

cd idf
idf.py set-target esp32c6   # once
idf.py build
```

You only need to repeat `set-target esp32c6` if you deleted `idf/build/` or changed chips.

## What to expect

| Situation | Typical time |
|-----------|--------------|
| First build | ~20–30 min (Matter is heavy) |
| After changes to `.cpp` / `.h` | A few minutes (rebuilds only what changed) |

**Output:** `idf/build/matter_magic_button.bin` (and other images under `idf/build/`).

## If something goes wrong

Full rebuild (useful after changing `sdkconfig`, partitions, or CMake):

```bash
cd idf
idf.py fullclean
idf.py set-target esp32c6
idf.py build
```

Use `fullclean` when you change project configuration — **not** required for normal C/C++ edits.

## Performance tip

Building from `~/` (e.g. copy the repo to `~/esp32-sw-matter-ota`) is faster than `/mnt/c/...`. Editing in Cursor on Windows and building from `/mnt/c/...` works, but incremental builds are slower.

## Flash

This guide covers **build only**. Do not run `flash`, `erase_flash`, or `monitor` until you follow the flash guide — see [flash.md](flash.md).

## See also

- [wsl-esp-idf-setup.md](wsl-esp-idf-setup.md) — WSL + ESP-IDF + esp-matter setup
- [../../README.en.md](../../README.en.md) — project overview
