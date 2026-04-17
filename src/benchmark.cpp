#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/std.h>

#include <boost/process/v1.hpp>

#include <benchmark.hpp>
#include <results.hpp>

#include <iterator>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>

/// @brief bold or not for smaller value
[[nodiscard]] static auto compare_times(float num1, float num2)
	  -> std::pair<fmt::text_style, fmt::text_style> {

	constexpr static auto epsilon = std::numeric_limits<float>::epsilon();

	// no staff solution or just small diff
	if (std::min(num1, num2) <= epsilon || std::abs(num1 - num2) <= epsilon)
		return {{}, {}};

	if (num1 < num2) return {{fmt::emphasis::bold}, {}};

	// num2 wins, is smaller
	return {{}, {fmt::emphasis::bold}};
}

//-----------------------------------------------------------------------------
void record::run(std::filesystem::path const &speller,
                 std::filesystem::path const &path) {

	namespace bp = boost::process::v1;

	auto working_dir = speller.parent_path();
	if (working_dir.empty()) working_dir = std::filesystem::current_path();

	bp::ipstream pipe;
	bp::child    c(speller.native().c_str(), path.c_str(), bp::std_out > pipe,
	               bp::start_dir(working_dir.string()));
	std::string  line;

	// Find and parse "WORDS MISSPELLED: <number>"
	while (std::getline(pipe, line)) {
		if (line.find("WORDS MISSPELLED") != std::string::npos) {
			std::istringstream iss(line);
			std::string        unused;
			// iss >> WORDS >> MISSPELLED >> <number>
			iss >> unused >> unused >> misspelled;
			break;
		}
	}

	// Parse the remaining stats sequentially
	auto parse_stat = [&pipe, &line](auto &value) {
		std::getline(pipe, line);
		std::istringstream iss(line.substr(line.find(':') + 1));
		iss >> value;
	};

	parse_stat(dictionary);
	parse_stat(text);
	parse_stat(load);
	parse_stat(check);
	parse_stat(size);
	parse_stat(unload);
	parse_stat(total);

	c.wait();
}

//-----------------------------------------------------------------------------
auto operator<<(std::ostream &os, benchmark const &rec) -> std::ostream & {

	fmt::format_to_n(std::ostream_iterator<decltype(rec.txt)::value_type>(os),
	                 16, "{: >16}", rec.txt.stem().native());
	os << ": ";

	// Format status with color
	auto print_status = [&os](const char *label, auto style) {
		os << fmt::format(style, "{: <10}", label);
	};

	if (!rec.yours.success) {
		print_status("ERROR", fmt::emphasis::bold | fg(fmt::color::red));
	} else if (rec.cs50.dictionary != 0 // a stand in for include staff
	           && rec.cs50.misspelled != rec.yours.misspelled) {
		print_status("MISMATCH",
		             fmt::emphasis::bold | fg(fmt::color::dark_golden_rod));
	} else {
		print_status("OK", fmt::emphasis::bold | fg(fmt::color::lawn_green));
	}

	// Format timing comparisons
	auto print_val = [&os](float cs50, float yours) {
		auto [staff_bold, your_bold] = compare_times(cs50, yours);
		os << fmt::format("{:6.3f} {:6.3f}   ",
		                  fmt::styled(cs50, staff_bold | cs50_color),
		                  fmt::styled(yours, your_bold));
	};

	print_val(rec.cs50.load, rec.yours.load);
	print_val(rec.cs50.check, rec.yours.check);
	print_val(rec.cs50.size, rec.yours.size);
	print_val(rec.cs50.unload, rec.yours.unload);
	print_val(rec.cs50.total, rec.yours.total);

	return os;
}
