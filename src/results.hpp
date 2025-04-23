#pragma once

#include <fmt/color.h>

// output constants
#define C_CLEAR "\033[2J\033[1;1H"

const inline auto cs50_color = fg(fmt::color::dark_cyan);
const inline auto your_color = fg(fmt::color::ghost_white);

void print_results_header();
