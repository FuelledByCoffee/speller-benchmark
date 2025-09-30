#include "results.hpp"
#include <fmt/format.h>

namespace fmt {

// https://github.com/fmtlib/fmt/issues/1260#issuecomment-1404324163
template <typename... Args>
static void print_indented(int indent, fmt::format_string<Args...> format_str,
                           Args &&...args) {
	fmt::print("{:{}}", "", indent);
	fmt::print(format_str, std::forward<Args>(args)...);
}

} // namespace fmt

void print_results_header() {

	// clear screen
	fmt::print(C_CLEAR);


	// print header
	fmt::print_indented(8, "{}\n", fmt::styled("Cyan    - CS50's implementation", cs50_color));
	fmt::print_indented(8, "{}\n",             "Default - your implementation");
	fmt::print_indented(8, "{}\n", fmt::styled("Bold    - lesser time", fmt::emphasis::bold));

	fmt::print("\n");
	fmt::print("{: >16}  ", "Filename");
	fmt::print("{: <11}", "Status");
	fmt::print("{: <16}", "Load");
	fmt::print("{: <16}", "Check");
	fmt::print("{: <16}", "Size");
	fmt::print("{: <16}", "Unload");
	fmt::print("{: <15}", "Total");
	fmt::print("\n");
}
