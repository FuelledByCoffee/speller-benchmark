#include "results.hpp"

#include <colors.hpp>
#include <fmt/core.h>
#include <fmt/format.h>

void print_results_header() {

	// clear screen
	fmt::print(C_CLEAR);

	// clang-format off
	// print header
	fmt::print(cs50_color, 			  "\tCyan   - CS50's implementation\n");
	fmt::print(your_color, 			  "\tWhite  - your implementation\n");
	fmt::print(fmt::emphasis::bold, "\tBold   - lesser time\n");
	// clang-format on

	fmt::print("\n");
	fmt::print("{: >16}  ", "Filename");
	fmt::print("{: <10}", "Status");
	fmt::print("{: <14}", "Load");
	fmt::print("{: <14}", "Check");
	fmt::print("{: <14}", "Size");
	fmt::print("{: <14}", "Unload");
	fmt::print("{: <14}", "Total");
	fmt::print("\n");
}
