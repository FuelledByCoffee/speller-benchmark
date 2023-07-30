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
#include <string_view>
#include <vector>

#include <fmt/color.h>
#include <fmt/std.h>

#include <dirent.h>
#include <getopt.h>

#define CS50_TEXTS "texts"

static auto error_m(std::string_view message, int code) -> void;

#ifdef COMPARE_STAFF
static bool includeStaff = true;
#else
static bool includeStaff = false;
#endif

int main(int argc, char *argv[]) {
	const char *cs50_speller = "./speller50";
	bool multithreading = true;
	int arg = 0;
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

	if (includeStaff && access(cs50_speller, X_OK) == -1)
		error_m("Staff speller cannot be opened\n", 1);

	print_results_header();

	namespace fs = std::filesystem;

	fs::path text_files(CS50_TEXTS);
	auto count = std::distance(fs::directory_iterator{text_files}, fs::directory_iterator{});

	std::vector<benchmark> records{};
	records.reserve(count + 1);
	benchmark Total{"Total", false};

	for (auto const &txt : std::filesystem::directory_iterator{text_files}) {
		if (multithreading) {
			records.emplace_back(txt.path(), includeStaff);
		}
	}

	for (auto const &txt : fs::directory_iterator{text_files}) {
		assert(txt.is_regular_file() && "Non text file found in texts dir");
		benchmark b{txt.path(), includeStaff};

		b.run();

		fmt::print("{}\n", b);

		// keep track of totals
		Total.cs50.load += b.cs50.load;
		Total.yours.load += b.yours.load;

		Total.cs50.check += b.cs50.check;
		Total.yours.check += b.yours.check;

		Total.cs50.size += b.cs50.size;
		Total.yours.size += b.yours.size;

		Total.cs50.unload += b.cs50.unload;
		Total.yours.unload += b.yours.unload;

		Total.cs50.total += b.cs50.total;
		Total.yours.total += b.yours.total;
	}

	fmt::print("\n{}\n", Total);
} // main

/// @brief displays the error and exists with code
static void error_m(std::string_view message, int code) {
	fmt::print(stderr, "Error: {}", message);
	exit(code);
}
