#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/std.h>

#include <boost/process.hpp>

#include <benchmark.hpp>
#include <results.hpp>

#include <cstdio>
#include <iterator>
#include <limits>
#include <ostream>
#include <string>
#include <string_view>

/// @brief bold or not for smaller value
[[nodiscard]] static auto compare_times(float num1, float num2)
	  -> std::pair<fmt::text_style, fmt::text_style> {

	constexpr static auto epsilon = std::numeric_limits<float>::epsilon();

	// no staff solution or just small diff
	if (fminf(num1, num2) <= epsilon || fabsf(num1 - num2) <= epsilon)
		return {{}, {}};

	if (num1 < num2) return {{fmt::emphasis::bold}, {}};

	// num2 wins, is smaller
	return {{}, {fmt::emphasis::bold}};
}

//-----------------------------------------------------------------------------
void record::run(std::string_view speller, std::filesystem::path const &path) {

	namespace bp = boost::process;

	bp::ipstream pipe;
	bp::child   c(speller.data(), path.c_str(), bp::std_out > pipe);
	std::string line;

	// consume until stats
	while (std::getline(pipe, line))
		if (strstr(line.data(), "WORDS MISSPELLED") != NULL) break;

	sscanf(line.data(), "WORDS MISSPELLED: %d\n", &misspelled);

	line.reserve(1000);
	pipe.read(line.data(), 1000);
	if (sscanf(line.data(),
	           "WORDS IN DICTIONARY:  %d\n"
	           "WORDS IN TEXT:        %d\n"
	           "TIME IN load:         %f\n"
	           "TIME IN check:        %f\n"
	           "TIME IN size:         %f\n"
	           "TIME IN unload:       %f\n"
	           "TIME IN TOTAL:        %f\n",
	           &dictionary, &text, &load, &check, &size, &unload, &total)
	    != 7) {
		success = false;
	}
	c.wait();
}

//-----------------------------------------------------------------------------
auto operator<<(std::ostream &os, benchmark const &rec) -> std::ostream & {

	fmt::format_to_n(std::ostream_iterator<decltype(rec.txt)::value_type>(os),
	                 16, "{: >16}", rec.txt.stem().native());
	os << ": ";

	// print status
	if (!rec.yours.success) {
		os << fmt::format(fmt::emphasis::bold | fg(fmt::color::red), "{: <10}",
		                  "ERROR");
	} else if (rec.cs50.dictionary != 0 // a stand in for include staff
	           && rec.cs50.misspelled != rec.yours.misspelled) {
		os << fmt::format(fmt::emphasis::bold | fg(fmt::color::yellow),
		                  "{: <10}", "MISMATCH");
	} else {
		os << fmt::format(fmt::emphasis::bold | fg(fmt::color::lawn_green),
		                  "{: <10}", "OK");
	}

	auto print_val = [&os](float cs50, float yours) {
		auto [staff_bold, your_bold] = compare_times(cs50, yours);
		os << fmt::format("{:.3f} ",
		                  fmt::styled(cs50, staff_bold | cs50_color));
		os << fmt::format("{:.3f}   ",
		                  fmt::styled(yours, your_bold | your_color));
	};

	print_val(rec.cs50.load, rec.yours.load);
	print_val(rec.cs50.check, rec.yours.check);
	print_val(rec.cs50.size, rec.yours.size);
	print_val(rec.cs50.unload, rec.yours.unload);
	print_val(rec.cs50.total, rec.yours.total);

	return os;
}
