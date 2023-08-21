
#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/std.h>

#include <benchmark.hpp>
#include <results.hpp>

#include <cstdio>
#include <iterator>
#include <limits>
#include <memory>
#include <ostream>
#include <string_view>
#include <thread>

/// @brief bold or not for smaller value
[[nodiscard]] static auto compare_times(float num1, float num2)
		-> std::pair<fmt::text_style, fmt::text_style> {

	constexpr static auto epsilon = std::numeric_limits<float>::epsilon();

	// no staff solution or just small diff
	if (fminf(num1, num2) < epsilon || fabsf(num1 - num2) <= epsilon)
		return {fmt::text_style{}, fmt::text_style{}};

	if (num1 < num2)
		return {fmt::text_style{fmt::emphasis::bold}, fmt::text_style{}};

	// num2 wins, is smaller
	return {fmt::text_style{}, fmt::text_style{fmt::emphasis::bold}};
}


//-----------------------------------------------------------------------------
void record::run(std::string_view speller, std::filesystem::path const &path) {

	std::string const command = fmt::format("./{} {}", speller, path.c_str());

	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"),
	                                              pclose);

	// execute and parse user results
	if (!pipe) {
		fmt::print(stderr, "Opening text file {} in thread {} failed\n",
		           path.filename(), std::this_thread::get_id());
		success = false;
		return;
	}

	std::array<char, 200> buffer;

	// consume till stats
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
		if (strstr(buffer.data(), "WORDS MISSPELLED") != NULL) break;

	sscanf(buffer.data(), "WORDS MISSPELLED: %d\n", &misspelled);

	// parse output
	if (fscanf(pipe.get(),
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
		os << fmt::format(fmt::emphasis::bold | fg(fmt::color::yellow), "{: <10}",
		                  "MISMATCH");
	} else {
		os << fmt::format(fmt::emphasis::bold | fg(fmt::color::lawn_green),
		                  "{: <10}", "OK");
	}

	auto print_val = [&os](float cs50, float yours) {
		auto [staff_bold, your_bold] = compare_times(cs50, yours);
		os << fmt::format("{:.3f} ", fmt::styled(cs50, staff_bold | cs50_color));
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
