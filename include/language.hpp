#pragma once

#include <cstdio>
#include <sys/stat.h>

namespace edz::language {

#ifndef EDIZON_LANGUAGE_PATH
#define EDIZON_LANGUAGE_PATH "/config/edizon/language.ini"
#endif

inline constexpr const char* config_path = EDIZON_LANGUAGE_PATH;

// Fresh installations always start in Russian. The user's choice is restored
// only when a valid language preference was previously saved.
inline bool russian = true;

inline bool is_russian() {
    return russian;
}

inline void set_russian(bool value) {
    russian = value;
}

inline void load() {
    FILE* file = std::fopen(config_path, "r");
    if (!file) return;

    char value[8] = {};
    if (std::fscanf(file, "language=%7s", value) == 1)
        russian = !(value[0] == 'e' || value[0] == 'E');
    std::fclose(file);
}

inline void save() {
    ::mkdir("/config", 0777);
    ::mkdir("/config/edizon", 0777);

    FILE* file = std::fopen(config_path, "w");
    if (!file) return;

    std::fprintf(file, "language=%s\n", russian ? "ru" : "en");
    std::fclose(file);
}

} // namespace edz::language
