#!/usr/bin/env bash
# Idempotent: applies all ryazhenka-specific patches after an upstream sync.
# Safe to run multiple times.
set -euo pipefail

TOPDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

# ── 1. Sync APP_VERSION in Makefile from .ryazhenka-version ──────────────────
if [ -f "${TOPDIR}/.ryazhenka-version" ]; then
    VER=$(cat "${TOPDIR}/.ryazhenka-version" | tr -d '[:space:]')
    echo "Setting APP_VERSION to ${VER} in Makefile..."
    sed -i "s|^APP_VERSION\s*:=.*|APP_VERSION\t\t:=\t${VER}|" "${TOPDIR}/Makefile" 2>/dev/null || true
fi

# ── 2. Fix Makefile: include libryazhahand, not libultrahand ─────────────────
echo "Patching Makefile includes..."
if grep -q "libs/libultrahand/ultrahand.mk" "${TOPDIR}/Makefile" && \
   ! grep -q "libs/libryazhahand/ryazhahand.mk" "${TOPDIR}/Makefile"; then
    sed -i \
        's|include \${TOPDIR}/libs/libultrahand/ultrahand.mk|ifneq ($(wildcard ${TOPDIR}/libs/libryazhahand/ryazhahand.mk),)\ninclude ${TOPDIR}/libs/libryazhahand/ryazhahand.mk\nelse ifneq ($(wildcard ${TOPDIR}/libs/libultrahand/ultrahand.mk),)\ninclude ${TOPDIR}/libs/libultrahand/ultrahand.mk\nendif|' \
        "${TOPDIR}/Makefile"
fi

# ── 3. Fix Makefile: RYZH signature instead of ULTR ─────────────────────────
echo "Patching .ovl signature..."
sed -i "s|printf 'ULTR'|printf 'RYZH'|g" "${TOPDIR}/Makefile"
sed -i "s|Ultrahand signature has been added|Ryazhahand signature has been added.|g" "${TOPDIR}/Makefile"

# ── 4. Fix Makefile: add -lpng -lz to LIBS ───────────────────────────────────
if grep -qE "^LIBS\s*:=\s*-lnx" "${TOPDIR}/Makefile" && \
   ! grep -qE "^LIBS\s*:=.*-lpng" "${TOPDIR}/Makefile"; then
    echo "Adding -lpng -lz to LIBS..."
    sed -i 's|^LIBS\s*:=\s*-lnx|LIBS\t:= -lpng -lz -lnx|' "${TOPDIR}/Makefile"
fi

# ── 5. Fix .gitmodules: replace libultrahand with libryazhahand ───────────────
echo "Patching .gitmodules..."
cat > "${TOPDIR}/.gitmodules" <<'EOF'
[submodule "libs/libryazhahand"]
	path = libs/libryazhahand
	url = https://github.com/dimasick-git/libryazhahand.git
	branch = main
EOF

# ── 6. Remove libs/libultrahand and libs/libtesla if pulled in by upstream ───
for stale in libs/libultrahand libs/libtesla; do
    if [ -d "${TOPDIR}/${stale}" ]; then
        echo "Removing stale submodule dir: ${stale}"
        git rm -rf --cached "${stale}" 2>/dev/null || true
        rm -rf "${TOPDIR}/${stale}"
    fi
done

# ── 7. Silence #pragma message in asmjit/core/cpuinfo.cpp ────────────────────
CPUINFO="${TOPDIR}/source/asmjit/core/cpuinfo.cpp"
if [ -f "${CPUINFO}" ]; then
    echo "Silencing asmjit #pragma message in cpuinfo.cpp..."
    sed -i 's|#pragma message("\[asmjit\].*")|// asmjit: runtime CPU detection pragma silenced|g' "${CPUINFO}"
fi

# ── 8. Replace libultrahand/Ultrahand-Overlay refs in docs ───────────────────
echo "Replacing branding in .md/.txt files (outside protected dirs)..."
find "${TOPDIR}" \( \
    -path "${TOPDIR}/.github" -o \
    -path "${TOPDIR}/scripts" -o \
    -path "${TOPDIR}/docs" -o \
    -path "${TOPDIR}/libs" \
    \) -prune -o \( -name "*.md" -o -name "*.txt" \) -print | while read -r f; do
    sed -i \
        -e 's|libultrahand|libryazhahand|g' \
        -e 's|Ultrahand-Overlay|Ryazhahand-Overlay|g' \
        "${f}"
done

echo "Ryazhenka patches applied successfully."
