# Upstream Sync Pipeline

**TL;DR (EN):** Every Monday at 04:17 UTC (or manually), the workflow fetches `proferabg/EdiZon-Overlay`, merges with `-X theirs`, then restores protected files and re-applies ryazhenka patches before opening a PR.

---

## Описание

Файл `.github/workflows/sync-upstream.yml` автоматически синхронизирует этот репозиторий с апстримом `proferabg/EdiZon-Overlay`.

### Расписание

- **Автоматически:** каждый понедельник в 04:17 UTC
- **Вручную:** через `workflow_dispatch` в разделе Actions

---

## Шаги пайплайна

1. **Checkout** — клонирует текущий репозиторий с полной историей
2. **Fetch upstream** — добавляет remote `proferabg/EdiZon-Overlay` и забирает ветку `main`
3. **Merge** — создаёт ветку `sync/upstream-YYYYMMDD` и мержит upstream с `-X theirs` (при конфликте выигрывает upstream)
4. **restore_protected.sh** — откатывает все пути из `protected_paths.txt` к версии из `origin/main`
5. **apply_ryazhenka_patches.sh** — идемпотентно применяет все ryazhenka-специфичные изменения
6. **Push + PR** — пушит ветку и открывает Pull Request в main

---

## Защищённые пути

Список хранится в `scripts/protected_paths.txt`. Эти пути никогда не перезаписываются upstream:

```
.github/
scripts/
docs/
.gitmodules
.gitignore
README.md
Makefile
libs/libryazhahand
```

---

## apply_ryazhenka_patches.sh — что делает

| Патч | Описание |
|---|---|
| APP_VERSION | Синхронизирует версию из `.ryazhenka-version` в Makefile |
| include libryazhahand | Заменяет `ultrahand.mk` на `ryazhahand.mk` с fallback |
| RYZH подпись | Заменяет `ULTR` → `RYZH` в правиле `.ovl` |
| -lpng -lz | Добавляет зависимости libpng в `LIBS` |
| .gitmodules | Перезаписывает на libryazhahand |
| Очистка stale | Удаляет `libs/libultrahand`, `libs/libtesla` если появились |
| asmjit silence | Заменяет `#pragma message` в `cpuinfo.cpp` на комментарий |
| Ребрендинг | Заменяет `libultrahand`/`Ultrahand-Overlay` в `.md`/`.txt` вне защищённых директорий |

---

## Разрешение конфликтов при sync

Стратегия `-X theirs` означает, что при конфликте побеждает upstream. Это нормальное поведение для `source/` и `include/` — наши изменения там минимальны.

После merge `restore_protected.sh` восстанавливает всё важное из `origin/main`, а `apply_ryazhenka_patches.sh` заново применяет патчи — таким образом даже полный override упстрима не сломает сборку.

---

## Ручной запуск

```bash
# Локально (для тестирования)
git remote add upstream https://github.com/proferabg/EdiZon-Overlay.git
git fetch upstream main
git merge upstream/main -X theirs --no-edit
bash scripts/restore_protected.sh
bash scripts/apply_ryazhenka_patches.sh
```
