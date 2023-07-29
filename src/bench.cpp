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

#include <version.hpp>
#include <record.hpp>
#include <colors.hpp>

#include <array>
#include <cmath>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <iomanip>

#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <fmt/std.h>

#include <dirent.h>
#include <getopt.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CS50_SPELLER "./speller50"
#define CS50_TEXTS   "texts"

// max number of files that can be benchmarked
#define MAX_FILES 50


static auto run_benchmark(record &mrecord) -> void;
static auto error_m(std::string_view message, int code) -> void;

#ifdef COMPARE_STAFF
static bool includeStaff = true;
#else
static bool includeStaff = false;
#endif

static const char *cs50_speller = CS50_SPELLER;

int main(int argc, char *argv[]) {
    bool multithreading = true;
    int arg = 0;
    while ((arg = getopt(argc, argv, "1ts:")) != -1) {
        if (arg == '1')
            multithreading = false;
        else if (arg == 't')
            includeStaff = true;
        else if (arg == 's') {
            cs50_speller = optarg;
            includeStaff = true;
        }
    }

    // make sure speller program exists
    if (access("speller", X_OK) == -1)
        error_m("speller binary does not exist, please compile it and place it "
                "in the current directory\n",
                1);

    if (includeStaff && access(cs50_speller, X_OK) == -1)
        error_m("Staff speller cannot be opened\n", 1);

    // setup records to benchmark
    std::array<record, MAX_FILES> records{};

    int num_records = 0;

    DIR *dirp = opendir(CS50_TEXTS);
    if (!dirp)
        error_m("Could not read " CS50_TEXTS "\n", 1);

    for (dirent *dir = NULL; (dir = readdir(dirp)) != NULL;) {
        if (dir->d_name[0] == '.')
            continue;

        records[num_records].filename = dir->d_name;

        std::string path(CS50_TEXTS "/");
        path += dir->d_name;
        records[num_records++].path = path;
    }

    closedir(dirp);

    // clear screen
    fmt::print(C_CLEAR);

    // print header
    fmt::print("\t" C_CS50  "Cyan   " C_RESET "- CS50's implementation\n");
    fmt::print("\t" C_YOURS "Yellow " C_RESET "- your implementation\n");
    fmt::print("\t" C_BOLD  "Bold   " C_RESET "- lesser time\n");

    fmt::print("\n");
    // fmt::print("%16.16s ", "Filename");
    fmt::print("{: >16} ", "Filename");
    fmt::print("Status\t");
    fmt::print("Load\t\t");
    fmt::print("Check\t\t");
    fmt::print("Size\t\t");
    fmt::print("Unload\t\t");
    fmt::print("Total\t\t");
    fmt::print("\n");

    float ytload = 0, ytcheck = 0, ytsize = 0, ytunload = 0, yttotal = 0;
    float ctload = 0, ctcheck = 0, ctsize = 0, ctunload = 0, cttotal = 0;

    // benchmark the files and output records
    pthread_t threads[MAX_FILES]; // one thread per benchmark
    if (multithreading) {
        for (int i = 0; i < num_records; i++) {
            pthread_create(&threads[i], NULL, PCAST run_benchmark, &records[i]);
        }
    }

    for (int i = 0; i < num_records; i++) {

        if (multithreading)
            pthread_join(threads[i], NULL);
        else
            run_benchmark(records[i]);

        // print data
        fmt::print("{}\n", records[i]);

        // keep track of totals
        ctload += records[i].load.second;
        ytload += records[i].load.first;

        ctcheck += records[i].check.second;
        ytcheck += records[i].check.first;

        ctsize += records[i].size.second;
        ytsize += records[i].size.first;

        ctunload += records[i].unload.second;
        ytunload += records[i].unload.first;

        cttotal += records[i].total.second;
        yttotal += records[i].total.first;
    }

    // display averages
    fmt::print("\n");
    fmt::print("{: >16} \t", "Average:");

    //  print_row(&();
} // main

static void run_benchmark(record &mrecord) {
    std::string user_command = fmt::format("./speller {}", mrecord.path.generic_string());
    std::string cs50_command =
        fmt::format("{} {}", cs50_speller, mrecord.path.generic_string());

    // execute and parse user results
    FILE *fpipe = popen(user_command.c_str(), "r");
    if (!fpipe) {
        mrecord.success = false;
        return;
    }
    mrecord.success = true;

    char buffer[200];

    // consume till stats
    while (fgets(buffer, sizeof(buffer), fpipe))
        if (strstr(buffer, "WORDS MISSPELLED") != NULL)
            break;

    sscanf(buffer, "WORDS MISSPELLED: %d\n", &mrecord.misspelled.second);

    // parse output
    fscanf(fpipe,
           "WORDS IN DICTIONARY:  %d\n"
           "WORDS IN TEXT:        %d\n"
           "TIME IN load:         %f\n"
           "TIME IN check:        %f\n"
           "TIME IN size:         %f\n"
           "TIME IN unload:       %f\n"
           "TIME IN TOTAL:        %f\n",
           &mrecord.dictionary.second, &mrecord.text.second, &mrecord.load.second,
           &mrecord.check.second, &mrecord.size.second, &mrecord.unload.second, &mrecord.total.second);

    pclose(fpipe);

    if (!includeStaff)
        return;

    // execute and parse cs50 results
    fpipe = popen(cs50_command.c_str(), "r");
    if (!fpipe) {
        mrecord.success = false;
        return;
    }

    // consume till stats
    while (fgets(buffer, sizeof(buffer), fpipe))
        if (strstr(buffer, "WORDS MISSPELLED") != NULL)
            break;

    sscanf(buffer, "WORDS MISSPELLED: %d\n", &mrecord.misspelled.second);

    // parse output
    fscanf(fpipe,
           "WORDS IN DICTIONARY:  %d\n"
           "WORDS IN TEXT:        %d\n"
           "TIME IN load:         %f\n"
           "TIME IN check:        %f\n"
           "TIME IN size:         %f\n"
           "TIME IN unload:       %f\n"
           "TIME IN TOTAL:        %f\n"

           ,
           &mrecord.dictionary.second, &mrecord.text.second, &mrecord.load.second,
           &mrecord.check.second, &mrecord.size.second, &mrecord.unload.second,
           &mrecord.total.second);

    pclose(fpipe);
}


/**
 * @brief displays the error and exists with the errno
 */
static void error_m(std::string_view message, int code) {
    fmt::print(stderr, "Error: {}", message);
    exit(code);
}
