
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <fmt/std.h>

#include <benchmark.hpp>
#include <colors.hpp>

#include <iterator>
#include <memory>
#include <ostream>
#include <string_view>
#include <thread>

#include <cstdio>

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
	auto compare_times = [](float num1, float num2) {
		const float epsilon = 0.0001F;

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
	std::fflush(stdout);

	// print status
	if (rec.yours.success)
		os << fmt::format(fmt::emphasis::bold | fg(fmt::color::lawn_green),
		                  "OK\t");
	else
		os << fmt::format(fmt::emphasis::bold | fg(fmt::color::red), "ERROR\t");
	os << fmt::format(C_RESET);

	// load
	os << compare_times(rec.cs50.load, rec.yours.load); // bold?
	os << C_CS50 << std::fixed << std::setprecision(3) << rec.cs50.load
		<< C_RESET << '\t';

	os << compare_times(rec.yours.load, rec.cs50.load); // bold?
	os << C_YOURS << std::fixed << std::setprecision(3) << rec.yours.load
		<< C_RESET << '\t';

	// check
	os << compare_times(rec.cs50.check, rec.yours.check); // bold?
	os << C_CS50 << std::fixed << std::setprecision(3) << rec.cs50.check
		<< C_RESET << '\t';

	os << compare_times(rec.yours.check, rec.cs50.check); // bold?
	os << C_YOURS << std::fixed << std::setprecision(3) << rec.yours.check
		<< C_RESET << '\t';

	// size
	os << compare_times(rec.cs50.size, rec.yours.load); // bold?
	os << C_CS50 << std::fixed << std::setprecision(3) << rec.cs50.size
		<< C_RESET << '\t';

	os << compare_times(rec.yours.size, rec.cs50.size); // bold?
	os << C_YOURS << std::fixed << std::setprecision(3) << rec.yours.size
		<< C_RESET << '\t';

	// unload
	os << compare_times(rec.cs50.unload, rec.yours.unload); // bold?
	os << C_CS50 << std::fixed << std::setprecision(3) << rec.cs50.unload
		<< C_RESET << '\t';

	os << compare_times(rec.yours.unload, rec.cs50.unload); // bold?
	os << C_YOURS << std::fixed << std::setprecision(3) << rec.yours.unload
		<< C_RESET << '\t';

	// total
	os << compare_times(rec.cs50.total, rec.yours.total); // bold?
	os << C_CS50 << std::fixed << std::setprecision(3) << rec.cs50.total
		<< C_RESET << '\t';

	os << compare_times(rec.yours.total, rec.cs50.total); // bold?
	os << C_YOURS << std::fixed << std::setprecision(3) << rec.yours.total
		<< C_RESET << '\t';

	return os;
}
