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
    [[ -z "${path}" || "${path}" == \#* ]] && continue
    clean_path="${path%/}"

    # Use git ls-tree to detect path — works for both files and gitlinks (submodules)
    if git ls-tree origin/main "${clean_path}" 2>/dev/null | grep -q "${clean_path}"; then
        git checkout origin/main -- "${clean_path}" 2>/dev/null || true
        echo "  restored: ${clean_path}"
    else
        echo "  skipped (not in origin/main): ${clean_path}"
    fi
done < "${PROTECTED}"

echo "Protected paths restored."
