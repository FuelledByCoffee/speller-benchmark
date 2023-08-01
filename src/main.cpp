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

#include <assert.h>
#include <filesystem>
#include <iterator>
#include <numeric>
#include <string_view>
#include <vector>

#include <fmt/color.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include <unistd.h>
#include <getopt.h>

#define CS50_TEXTS "texts"

namespace fs = std::filesystem;

static auto file_count(fs::path dir) noexcept ->
		typename std::iterator_traits<fs::directory_iterator>::difference_type;


static auto error_m(std::string_view message, int code) -> void;

#ifdef COMPARE_STAFF
static bool includeStaff = true;
#else
static bool includeStaff = false;
#endif

auto main(int argc, char *argv[]) -> int {
	std::string_view cs50_speller   = "./speller50";
	bool             multithreading = true;
	int              arg            = 0;
	while ((arg = getopt(argc, argv, "1t:")) != -1) {
		if (arg == '1')
			multithreading = false;
		else if (arg == 't')
			includeStaff = true;
	}

	// make sure speller program exists
	if (access("speller", X_OK) == -1)
		error_m("speller binary does not exist, please compile it and place it "
		        "in the current directory\n",
		        1);

	if (includeStaff && access(cs50_speller.data(), X_OK) == -1)
		error_m("Staff speller cannot be opened\n", 1);

	print_results_header();

	fs::path   text_files(CS50_TEXTS);
	auto const count = file_count(text_files);
	if (count == 0) exit(2);

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
		} else {
			threads.emplace_back(&benchmark::run, &b);
		}
	}

	if (multithreading) {
		for (auto &t : threads) {
			t.join();
		}

		std::sort(std::begin(records), std::end(records));
		fmt::print("{}\n", fmt::join(records, "\n"));
	}


	benchmark total = std::reduce(std::cbegin(records), std::cend(records));

	total.txt         = fs::path{"Total"};
	benchmark average = total / count;
	average.txt       = fs::path{"Average"};

	fmt::print("\n{}\n", total);
	fmt::print("{}\n", average);
} // main

/// @brief displays the error and exists with code
static void error_m(std::string_view message, int code) {
	fmt::print(stderr, "Error: {}", message);
	exit(code);
}

static auto file_count(fs::path dir) noexcept ->
		typename std::iterator_traits<fs::directory_iterator>::difference_type {
	fs::directory_iterator i;
	try {
		i = fs::directory_iterator{dir};
	} catch (const std::exception &e) {
		fmt::print(stderr, "{}\n", e.what());
		return 0;
	}
	return std::distance(i, fs::directory_iterator{});
}
