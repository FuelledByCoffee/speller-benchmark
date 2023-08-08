#include "results.hpp"

#include <colors.hpp>
#include <fmt/core.h>
#include <fmt/format.h>

void print_results_header() {

	// clear screen
	fmt::print(C_CLEAR);

	using namespace fmt::literals;

	// clang-format off
	// print header
	fmt::print(cs50_color, 			  "\tCyan   - CS50's implementation\n");
	fmt::print(your_color, 			  "\tWhite  - your implementation\n");
	fmt::print(fmt::emphasis::bold, "\tBold   - lesser time\n");
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
