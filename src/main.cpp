/**
 * Benchmarks speller program against the staff's implementation
 * with all the texts available in the pset5/texts folder
 *
 * - Azaan (azaan@outlook.com) 2012
 *
 *. Thanks Azaan!!
 *.
 *. @curiouskiwi
 *. updated 1 Apr 2020 for new directories
 *. also removed directories from the list of files
 *
 * @Fuelled_By_Coffee
 * add multithreading
 * fix uninitialized memory bug
 * Port to C++
 *
 * - Johannes "Fuelled_By_Coffee" Eckhoff j.eckhoff@gmail.com
 */

#include <benchmark.hpp>
#include <colors.hpp>
#include <results.hpp>
#include <version.hpp>

#include <cassert>
#include <filesystem>
#include <iterator>
#include <numeric>
#include <string_view>
#include <vector>

#include <fmt/std.h>

#include <getopt.h>
#include <unistd.h>

#define CS50_TEXTS "texts"

namespace fs = std::filesystem;

[[nodiscard("Only point of this is using the return value")]] static auto
file_count(fs::path const &dir) noexcept ->
		typename std::iterator_traits<fs::directory_iterator>::difference_type;

#ifdef COMPARE_STAFF
static bool includeStaff = true;
#else
static bool includeStaff = false;
#endif

auto main(int argc, char *argv[]) -> int {
	std::string_view cs50_speller   = "./speller50";
	bool             multithreading = true;
	int              arg            = 0;
	while ((arg = getopt(argc, argv, "1sy")) != -1) {
		if (arg == '1')
			multithreading = false;
		else if (arg == 's')
			includeStaff = true;
		else if (arg == 'y')
			includeStaff = false;
		else if (arg == '?')
			throw fmt::system_error(1, "Invalid flag\n");
	}

	// make sure speller program exists
	if (access("speller", X_OK) == -1)
		throw fmt::system_error(
				1, "speller binary does not exist, please compile it and place it "
					"in the current directory\n");

	if (includeStaff && access(cs50_speller.data(), X_OK) == -1)
		throw fmt::system_error(1, "Staff speller cannot be opened\n");

	print_results_header();

	fs::path   text_files(CS50_TEXTS);
	auto const count = file_count(text_files);
	if (count == 0) throw fmt::system_error(2, "Directory not found");

	std::vector<benchmark>   records;
	std::vector<std::thread> threads;
	records.reserve(count);
	threads.reserve(count);

	for (auto const &txt : fs::directory_iterator{text_files}) {
		assert(txt.is_regular_file() && "Non text file found in texts dir");
		records.emplace_back(txt.path(), includeStaff);
	}

	for (auto &b : records) {
		if (!multithreading) {
			b.run();
			fmt::print("{}\n", b);
			std::fflush(stdout);
		} else {
			threads.emplace_back(&benchmark::run, &b);
		}
	}

	if (multithreading) {
		for (auto &t : threads) {
			t.join();
		}

		std::ranges::sort(records);
		fmt::print("{}\n", fmt::join(records, "\n"));
	}


	benchmark total = std::reduce(std::cbegin(records), std::cend(records));

	total.txt         = fs::path{"Total"};
	benchmark average = total / count;
	average.txt       = fs::path{"Average"};

	fmt::print("\n{}\n{}\n", total, average);
} // main

static auto file_count(fs::path const &dir) noexcept ->
		typename std::iterator_traits<fs::directory_iterator>::difference_type {
	try {
		return std::distance(fs::directory_iterator{dir},
		                     fs::directory_iterator{});
	} catch (const std::exception &e) {
		fmt::print(stderr, "{}\n", e.what());
		return 0;
	}
}
