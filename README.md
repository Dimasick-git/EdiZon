# EdiZon Overlay — Ryazhahand Fork

**TL;DR (EN):** Nintendo Switch overlay for managing cheats and viewing system info. Built on [libryazhahand](https://github.com/dimasick-git/libryazhahand). Per-overlay config at `/config/edizon/`. Two release channels: `latest-build` (every main push) and `v*.*.*` (tagged releases). Upstream: `proferabg/EdiZon-Overlay`, synced weekly.

---

## О проекте

EdiZon Overlay — это оверлей для Nintendo Switch, позволяющий управлять чит-кодами (через dmntcht) и просматривать системную информацию (температуры, частоты, IP). Форк использует **libryazhahand** вместо libryazhahand, что даёт более чистые заголовки и поддержку тем через `/config/ryazhahand/`.

Оригинал: [@WerWolv](https://github.com/WerWolv/EdiZon)  
Апстрим форка: [proferabg/EdiZon-Overlay](https://github.com/proferabg/EdiZon-Overlay)  
Поддержка форка: Dimasick-git

---

## Конфигурация — таблица путей

| Назначение | Путь |
|---|---|
| Глобальные темы, звуки, обои (libryazhahand) | `/config/ryazhahand/` |
| Per-overlay overrides (UI, цвета) | `/config/edizon/` |

---

## Каналы релизов

| Канал | Тег | Триггер | Описание |
|---|---|---|---|
| `latest-build` | `latest-build` | Каждый push в `main` | Автоматическая сборка с суффиксом `+ryazh.<sha>` |
| Версионный релиз | `v*.*.*` | Push тега `v*` или `workflow_dispatch` | Стабильная версия, синхронизирована с `.ryazhenka-version` |

Все релизы содержат два файла:
- `ovlEdiZon.ovl` — скопировать в `/switch/.overlays/`
- `EdiZon-Overlay-*.zip` — архив со структурой папок Switch

---

## Структура репозитория

```
EdiZon/
├── .github/
│   └── workflows/
│       ├── build.yml          # CI: сборка на каждый push/PR + auto-release
│       ├── release.yml        # Релиз по тегу v* или вручную
│       └── sync-upstream.yml  # Еженедельный sync из upstream
├── docs/
│   └── SYNC.md                # Описание sync-pipeline
├── include/                   # Заголовки проекта
├── libs/
│   └── libryazhahand/         # Git submodule (dimasick-git/libryazhahand)
├── scripts/
│   ├── apply_ryazhenka_patches.sh
│   ├── protected_paths.txt
│   └── restore_protected.sh
├── source/                    # Исходники оверлея
├── .gitmodules
├── .ryazhenka-version         # Единый источник версии
└── Makefile
```

---

## Версионирование

Версия хранится в `.ryazhenka-version` в корне репозитория. Makefile читает её автоматически:

```makefile
APP_VERSION := $(shell cat $(TOPDIR)/.ryazhenka-version | tr -d '[:space:]')
```

Для изменения версии достаточно обновить `.ryazhenka-version` и закоммитить.

---

## Использование субменю

В файле чит-кодов можно создавать подменю с помощью тегов:

```
[--SectionStart:Item Codes--]
[Items x999]
040A0000 01DB2A08 52807CE0
[--SectionEnd:Item Codes--]
```

Чтобы отключить субменю, добавьте в начало файла:

```
[--DisableSubmenus--]
00000000 00000000 00000000
```

---

## Включение читов по умолчанию

Добавьте `:ENABLED` к названию чита:

```
[60 FPS Mod:ENABLED]
00000000 00000000 00000000
```

Чит включится при каждом открытии оверлея.

---

## Сборка

Требуется [devkitPro](https://devkitpro.org/) с devkitA64 и libnx.

```bash
git clone --recurse-submodules https://github.com/Dimasick-git/EdiZon.git
cd EdiZon
make
```

Артефакт: `out/ovlEdiZon.ovl`

---

## Upstream Sync

Репозиторий синхронизируется с апстримом каждый понедельник в 04:17 UTC автоматически. Подробнее — в [docs/SYNC.md](docs/SYNC.md).

---

## Лицензия

GPL-2.0 (оригинальный код WerWolv). Изменения форка — те же условия.
