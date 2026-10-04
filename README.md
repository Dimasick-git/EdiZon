# EdiZon Overlay

Оверлей для управления читами Atmosphere и просмотра температур, частот и сети. Версия **1.0.17**: новая libryazhahand, компактные списки и экран информации.

Скопируйте содержимое `EdiZon-Overlay.zip` в корень SD-карты. Нужны Atmosphere и Ryazhahand-Overlay. Настройка языка хранится в `/config/edizon/language.ini`; существующие читы и настройки сохраняются.

Сборка: devkitA64, актуальный libnx и portlibs. Выполните `git submodule update --init --recursive`, затем `make`. GitHub Actions собирает закреплённый libnx с API HOS 23 и проверяет формат и подпись оверлея. Релизы публикуются только для тегов `v*`.

Основано на [EdiZon-Overlay](https://github.com/proferabg/EdiZon-Overlay) и [EdiZon](https://github.com/WerWolv/EdiZon). Библиотека: [libryazhahand](https://github.com/Dimasick-git/libryazhahand). Авторы: WerWolv, proferabg, ppkantorski; поддержка форка — Dimasick-git. Лицензия находится в `LICENSE`.
