#pragma once

#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <fmt/std.h>

#include <filesystem>
#include <iostream>
#include <string>
#include <utility>

struct record {
    std::string filename{};
    std::filesystem::path path{};

    std::pair<int, int> misspelled{};
    std::pair<int, int> dictionary{};
    std::pair<int, int> text{};

    std::pair<float, float> load{};
    std::pair<float, float> check{};
    std::pair<float, float> size{};
    std::pair<float, float> unload{};
    std::pair<float, float> total{};

    bool success{};

    friend auto operator<<(std::ostream &os, record const &rec)
        -> std::ostream &;
};

template <> struct fmt::formatter<record> : ostream_formatter {};
