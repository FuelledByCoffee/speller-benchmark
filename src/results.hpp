#pragma once

#include <colors.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/os.h>

inline void print_results_header() {

	// clear screen
	fmt::print(C_CLEAR);

	// clang-format off
	// print header
	fmt::print("\t" C_CS50  "Cyan  " C_RESET "- CS50's implementation\n");
	fmt::print("\t" C_YOURS "White " C_RESET "- your implementation\n");
	fmt::print("\t" C_BOLD  "Bold  " C_RESET "- lesser time\n");
	// clang-format on

	fmt::print("\n");
	fmt::print("{: >16} ", "Filename");
	fmt::print("Status\t");
	fmt::print("Load\t\t");
	fmt::print("Check\t\t");
	fmt::print("Size\t\t");
	fmt::print("Unload\t\t");
	fmt::print("Total\t\t");
	fmt::print("\n");
}
