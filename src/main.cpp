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
 *
 * - Johannes "Fuelled_By_Coffee" Eckhoff j.eckhoff@gmail.com
 */

#include <benchmark.hpp>
#include <colors.hpp>
#include <version.hpp>

#include <cmath>
#include <string_view>
#include <vector>

#include <fmt/color.h>

#include <dirent.h>
#include <getopt.h>

#define CS50_TEXTS "texts"

static auto error_m(std::string_view message, int code) -> void;

#ifdef COMPARE_STAFF
static bool includeStaff = true;
#else
static bool includeStaff = false;
#endif

int main() {
    const char *cs50_speller = "./speller50";
    // bool multithreading = true;
    // int arg = 0;
    // while ((arg = getopt(argc, argv, "1ts:")) != -1) {
    //     if (arg == '1')
    //         multithreading = false;
    //     else if (arg == 't')
    //         includeStaff = true;
    // }

    // make sure speller program exists
    if (access("speller", X_OK) == -1)
        error_m("speller binary does not exist, please compile it and place it "
                "in the current directory\n",
                1);

    if (includeStaff && access(cs50_speller, X_OK) == -1)
        error_m("Staff speller cannot be opened\n", 1);

    // setup benchmarks to benchmark
    std::vector<benchmark> benchmarks{};

    DIR *dirp = opendir(CS50_TEXTS);
    if (!dirp)
        error_m("Could not read " CS50_TEXTS "\n", 1);

    for (dirent *dir = NULL; (dir = readdir(dirp)) != NULL;) {
        if (dir->d_name[0] == '.')
            continue;

        benchmarks.emplace_back(dir->d_name, includeStaff);
    }

    closedir(dirp);

    // clear screen
    fmt::print(C_CLEAR);

    // print header
    fmt::print("\t" C_CS50 "Cyan   " C_RESET "- CS50's implementation\n");
    fmt::print("\t" C_YOURS "Yellow " C_RESET "- your implementation\n");
    fmt::print("\t" C_BOLD "Bold   " C_RESET "- lesser time\n");

    fmt::print("\n");
    fmt::print("{: >16} ", "Filename");
    fmt::print("Status\t");
    fmt::print("Load\t\t");
    fmt::print("Check\t\t");
    fmt::print("Size\t\t");
    fmt::print("Unload\t\t");
    fmt::print("Total\t\t");
    fmt::print("\n");

    benchmark average{};
    average.filename = "Average";

    for (auto &benchmark : benchmarks) {

        fmt::print("{}\n", benchmark);

        // keep track of totals
        average.cs50.load += benchmark.cs50.load / benchmarks.size();
        average.yours.load += benchmark.yours.load / benchmarks.size();

        average.cs50.check += benchmark.cs50.check / benchmarks.size();
        average.yours.check += benchmark.yours.check / benchmarks.size();

        average.cs50.size += benchmark.cs50.size / benchmarks.size();
        average.yours.size += benchmark.yours.size / benchmarks.size();

        average.cs50.unload += benchmark.cs50.unload / benchmarks.size();
        average.yours.unload += benchmark.yours.unload / benchmarks.size();

        average.cs50.total += benchmark.cs50.total / benchmarks.size();
        average.yours.total += benchmark.yours.total / benchmarks.size();
    }

    // display averages
    fmt::print("\n{}\n", average);
} // main

/**
 * @brief displays the error and exists with the errno
 */
static void error_m(std::string_view message, int code) {
    fmt::print(stderr, "Error: {}", message);
    exit(code);
}
