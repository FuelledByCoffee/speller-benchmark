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
#include <cstddef>
#include <fmt/base.h>
#include <fmt/core.h>
#include <results.hpp>
#include <version.hpp>

#include <cassert>
#include <filesystem>
#include <iterator>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/std.h>

#include <getopt.h>
#include <unistd.h>

#include <version.hpp>

#define CS50_TEXTS "texts"

static constexpr struct option long_options[] = {
	  {      "version",       no_argument, nullptr, 'v'},
	  {"single-thread",       no_argument, nullptr, '1'},
	  {"staff-speller", optional_argument, nullptr, 's'},
	  { "your-speller", optional_argument, nullptr, 'y'},
	  {		nullptr,				 0, nullptr,   0}
};

namespace fs = std::filesystem;

static auto file_count(fs::path const &dir) noexcept ->
	  typename std::iterator_traits<fs::directory_iterator>::difference_type;

//-----------------------------------------------------------------------------
auto main(int argc, char *argv[]) -> int {
	fs::path cs50_speller   = "./speller50";
	fs::path your_speller   = "./speller";
	bool     multithreading = true;
	bool     includeStaff   = false;
	int      arg            = 0;
	while ((arg = getopt_long(argc, argv, "v1s::y::", long_options, nullptr))
	       != -1) {
		if (arg == 'v') {
			fmt::println("{}.{}", benchmark_VERSION_MAJOR,
			             benchmark_VERSION_MINOR);
			return 0;
		} else if (arg == '1') {
			multithreading = false;
		} else if (arg == 's') {
			includeStaff = true;
			if (optarg) cs50_speller = optarg;
		} else if (arg == 'y') {
			if (optarg) your_speller = optarg;
		} else if (arg == '?') {
			fmt::print("Invalid flag\n");
			return 1;
		}
	}

	// make sure speller program exists
	if (access(your_speller.c_str(), X_OK) == -1) {
		fmt::print(stderr,
		           "Could not locate speller binary\n" //
		           "Please launch the benchmark utility from the directory "
		           "where your speller binary is located or specify its path "
		           "with --your-speller\n");
		return 1;
	}

	if (includeStaff && access(cs50_speller.c_str(), X_OK) == -1) {
		fmt::print(stderr, "Staff speller cannot be opened\n");
		includeStaff = false;
	}

	print_results_header();

	fs::path text_files(your_speller.parent_path() / CS50_TEXTS);
	auto     count = static_cast<std::size_t>(file_count(text_files));
	if (count == 0) throw fmt::system_error(2, "Directory not found");

	std::vector<benchmark>   records;
	std::vector<std::thread> threads;
	records.reserve(count);
	threads.reserve(count);

	for (auto const &txt : fs::directory_iterator{text_files}) {
		assert(txt.is_regular_file() && "Non text file found in texts dir");
		records.emplace_back(txt.path(), includeStaff, cs50_speller,
		                     your_speller);
	}

	for (auto &b : records) {
		auto printer = [](auto const &bench) { fmt::print("{}\n", bench); };
		if (!multithreading)
			b.run(printer);
		else
			threads.emplace_back(&benchmark::run<decltype(printer)>, &b,
			                     printer);
	}

	if (multithreading)
		for (auto &t : threads) t.join();

	benchmark total = std::reduce(std::cbegin(records), std::cend(records));

	total.txt         = "Total";
	benchmark average = total / count;
	average.txt       = "Average";

	fmt::print("\n{}\n{}\n", total, average);
} // main

//-----------------------------------------------------------------------------
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
