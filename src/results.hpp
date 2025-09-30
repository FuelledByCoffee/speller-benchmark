#pragma once

#include <fmt/color.h>

// output constants
#define C_CLEAR "\033[2J\033[1;1H"

inline auto cs50_color = fg(fmt::color::dark_cyan);

void print_results_header();
