#!/usr/bin/env bash
# Build Magic Button firmware in WSL (no flash).
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IDF_DIR="$(cd "${SCRIPT_DIR}/../idf" && pwd)"

if [[ -z "${IDF_PATH:-}" ]]; then
  # shellcheck disable=SC1090
  source "${HOME}/esp-idf/export.sh"
fi
export PW_ACTIVATE_SKIP_CHECKS=1
if [[ -z "${ESP_MATTER_PATH:-}" ]]; then
  # esp-matter/export.sh references ESP_MATTER_PATH before it is set
  set +u
  # shellcheck disable=SC1090
  source "${HOME}/esp-matter/export.sh"
  set -u
fi

cd "${IDF_DIR}"
idf.py set-target esp32c6
idf.py build
echo "Build OK. Artifacts in ${IDF_DIR}/build (flash is a separate plan)."
