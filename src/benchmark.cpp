
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/std.h>

#include <benchmark.hpp>
#include <colors.hpp>

#include <cstdio>
#include <iterator>
#include <limits>
#include <memory>
#include <ostream>
#include <string_view>
#include <thread>

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

auto operator<<(std::ostream &os, benchmark const &rec) -> std::ostream & {

	/**
	 * @returns string constant bold if num1 is less
	 */
	auto compare_times = [epsilon = std::numeric_limits<float>::epsilon()](
										float num1, float num2) {
		// no staff solution or just small diff
		if (fminf(num1, num2) < epsilon || fabsf(num1 - num2) <= epsilon)
			return "";

		if (num1 < num2) return C_BOLD;

		// num2 greater
		return "";
	};

	// using namespace fmt::literals;

	fmt::format_to_n(std::ostream_iterator<decltype(rec.txt)::value_type>(os),
	                 16, "{: >16}", rec.txt.stem().native());
	os << ": ";

	// print status
	if (rec.yours.success)
		os << fmt::format(fmt::emphasis::bold | fg(fmt::color::lawn_green),
		                  "OK\t");
	else
		os << fmt::format(fmt::emphasis::bold | fg(fmt::color::red), "ERROR\t");
	os << fmt::format(C_RESET);

	auto print_val = [&os, compare_times](float cs50, float yours) {
		auto bold = compare_times(cs50, yours);
		os << bold << C_CS50 << std::fixed << std::setprecision(3) << cs50
			<< C_RESET << '\t';

		bold = compare_times(yours, cs50);
		os << bold << C_YOURS << std::fixed << std::setprecision(3) << yours
			<< C_RESET << '\t';
	};

	print_val(rec.cs50.load, rec.yours.load);
	print_val(rec.cs50.check, rec.yours.check);
	print_val(rec.cs50.size, rec.yours.size);
	print_val(rec.cs50.unload, rec.yours.unload);
	print_val(rec.cs50.total, rec.yours.total);

	return os;
}
