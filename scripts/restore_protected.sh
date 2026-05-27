#!/usr/bin/env bash
# Restores protected paths from current main after an upstream merge.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROTECTED="${SCRIPT_DIR}/protected_paths.txt"

if [ ! -f "${PROTECTED}" ]; then
    echo "ERROR: protected_paths.txt not found at ${PROTECTED}"
    exit 1
fi

echo "Restoring protected paths from main..."
while IFS= read -r path || [ -n "${path}" ]; do
    # Skip empty lines and comments
    [[ -z "${path}" || "${path}" == \#* ]] && continue

    # Strip trailing slash for git checkout
    clean_path="${path%/}"

    if git show "origin/main:${clean_path}" >/dev/null 2>&1; then
        git checkout origin/main -- "${clean_path}" 2>/dev/null || true
        echo "  restored: ${clean_path}"
    else
        echo "  skipped (not in origin/main): ${clean_path}"
    fi
done < "${PROTECTED}"

echo "Protected paths restored."
