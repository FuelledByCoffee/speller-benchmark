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
 * @FuelledByCoffee
 * add multithreading
 * fix uninitialized memory bug
 */

#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED

#include <version.h>

#include <alloca.h>
#include <dirent.h>
#include <getopt.h>
#include <math.h>
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

// output constants
#define C_CLEAR  "\033[2J\033[1;1H"
#define C_RESET  "\e[m"
#define C_GREEN  "\e[32m"
#define C_YELLOW "\e[33m"
#define C_CYAN   "\e[36m"
#define C_BLUE   "\e[34m"
#define C_RED    "\e[31m"
#define C_WHITE  "\e[37m"
#define C_BOLD   "\e[1m"
#define C_BWHITE "\e[47m"
#define C_CS50   C_CYAN
#define C_YOURS  C_YELLOW
#define PCAST    (void *(*)(void *))

#ifndef _GNU_SOURCE
#define strdupa(a) strcpy((char *)alloca(strlen(a) + 1), a)
#endif

typedef struct {
	char *filename;
	char *path;

	int misspelled, cs50_misspelled;
	int dictionary, cs50_dictionary;
	int text, cs50_text;

	float load, cs50_load;
	float check, cs50_check;
	float size, cs50_size;
	float unload, cs50_unload;
	float total, cs50_total;

	bool success;
} record;

static void  run_benchmark(record *mrecord);
static char *compare_times(float num1, float num2);
static void  print_row(record const *rec);
static void  error_m(char *error, int errno);


static bool  includeStaff = false;
static char *cs50_speller = CS50_SPELLER;


int main(int argc, char *argv[]) {
	bool multithreading = true;
	int  arg            = 0;
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
	record records[MAX_FILES];

	// If no staff solution, then cs50 values would be left uninitialized
	memset(records, 0, sizeof records);

	int num_records = 0;

	DIR *dirp = opendir(CS50_TEXTS);
	if (!dirp) error_m("Could not read " CS50_TEXTS "\n", 1);

	for (struct dirent *dir = NULL; (dir = readdir(dirp)) != NULL;) {
		if (dir->d_name[0] == '.') continue;

		records[num_records].filename = strdupa(dir->d_name);

		char path[512];
		strcpy(path, CS50_TEXTS "/");
		strcat(path, dir->d_name);
		records[num_records++].path = strdupa(path);
	}

	closedir(dirp);

	// clear screen
	printf(C_CLEAR);

	// print header
	printf("\t" C_CS50 "Cyan   " C_RESET "- CS50's implementation\n");
	printf("\t" C_YOURS "Yellow " C_RESET "- your implementation\n");
	printf("\t" C_BOLD "Bold   " C_RESET "- lesser time\n");


	// printf("\n\tRecord struct size: %zu\n", sizeof(record));

	printf("\n");
	printf("%16.16s ", "Filename");
	printf("Status\t");
	printf("Load\t\t");
	printf("Check\t\t");
	printf("Size\t\t");
	printf("Unload\t\t");
	printf("Total\t\t");
	printf("\n");

	float ytload = 0, ytcheck = 0, ytsize = 0, ytunload = 0, yttotal = 0;
	float ctload = 0, ctcheck = 0, ctsize = 0, ctunload = 0, cttotal = 0;


	// benchmark the files and output records
	pthread_t threads[num_records]; // one thread per benchmark
	if (multithreading) {
		for (int i = 0; i < num_records; i++) {
			pthread_create(&threads[i], NULL, PCAST run_benchmark, &records[i]);
		}
	}

	for (int i = 0; i < num_records; i++) {

		if (multithreading)
			pthread_join(threads[i], NULL);
		else
			run_benchmark(&records[i]);

		// print filename and flush buffer
		printf("%16.16s ", records[i].filename);

		// print status
		if (records[i].success)
			printf(C_BOLD C_GREEN "OK\t");
		else
			printf(C_BOLD C_RED "ERROR\t");
		printf(C_RESET);

		// print data
		print_row(&records[i]);

		// keep track of totals
		ctload += records[i].cs50_load;
		ytload += records[i].load;

		ctcheck += records[i].cs50_check;
		ytcheck += records[i].check;

		ctsize += records[i].cs50_size;
		ytsize += records[i].size;

		ctunload += records[i].cs50_unload;
		ytunload += records[i].unload;

		cttotal += records[i].cs50_total;
		yttotal += records[i].total;
	}

	// display averages
	printf("\n");
	printf("%15.15s \t", "Average:");

	print_row(&(record){.load        = ytload / num_records,
	                    .cs50_load   = ctload / num_records,
	                    .check       = ytcheck / num_records,
	                    .cs50_check  = ctcheck / num_records,
	                    .size        = ytsize / num_records,
	                    .cs50_size   = ctsize / num_records,
	                    .unload      = ytunload / num_records,
	                    .cs50_unload = ctunload / num_records,
	                    .total       = yttotal / num_records,
	                    .cs50_total  = cttotal / num_records});

	return 0;
}


static void run_benchmark(record *mrecord) {
	char user_command[250];
	char cs50_command[250];

	sprintf(user_command, "./speller %s", mrecord->path);
	sprintf(cs50_command, "%s %s", cs50_speller, mrecord->path);

	// execute and parse user results
	FILE *fpipe = popen(user_command, "r");
	if (!fpipe) {
		mrecord->success = false;
		return;
	}
	mrecord->success = true;

	char buffer[200];

	// consume till stats
	while (fgets(buffer, sizeof(buffer), fpipe))
		if (strstr(buffer, "WORDS MISSPELLED") != NULL) break;

	sscanf(buffer, "WORDS MISSPELLED: %d\n", &mrecord->misspelled);

	// parse output
	fscanf(fpipe,
	       "WORDS IN DICTIONARY:  %d\n"
	       "WORDS IN TEXT:        %d\n"
	       "TIME IN load:         %f\n"
	       "TIME IN check:        %f\n"
	       "TIME IN size:         %f\n"
	       "TIME IN unload:       %f\n"
	       "TIME IN TOTAL:        %f\n",
	       &mrecord->dictionary, &mrecord->text, &mrecord->load, &mrecord->check,
	       &mrecord->size, &mrecord->unload, &mrecord->total);

	pclose(fpipe);

	if (!includeStaff) return;

	// execute and parse cs50 results
	fpipe = popen(cs50_command, "r");
	if (!fpipe) {
		mrecord->success = false;
		return;
	}

	// consume till stats
	while (fgets(buffer, sizeof(buffer), fpipe))
		if (strstr(buffer, "WORDS MISSPELLED") != NULL) break;

	sscanf(buffer, "WORDS MISSPELLED: %d\n", &mrecord->cs50_misspelled);

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
	       &mrecord->cs50_dictionary, &mrecord->cs50_text, &mrecord->cs50_load,
	       &mrecord->cs50_check, &mrecord->cs50_size, &mrecord->cs50_unload,
	       &mrecord->cs50_total);

	pclose(fpipe);
}


/**
 * @brief displays the error and exists with the errno
 */
static void error_m(char *error, int errno) {
	fprintf(stderr, "Error: %s", error);
	exit(errno);
}

//! @param rec record to print
//! @param prec how many decimals to print for each value
static void print_row(record const *rec) {
	printf("%s", compare_times(rec->cs50_load, rec->load));
	printf(C_CS50 "%.*f\t" C_RESET, 3, rec->cs50_load);
	printf("%s", compare_times(rec->load, rec->cs50_load));
	printf(C_YOURS "%.*f\t" C_RESET, 3, rec->load);

	printf("%s", compare_times(rec->cs50_check, rec->check));
	printf(C_CS50 "%.*f\t" C_RESET, 3, rec->cs50_check);
	printf("%s", compare_times(rec->check, rec->cs50_check));
	printf(C_YOURS "%.*f\t" C_RESET, 3, rec->check);

	printf("%s", compare_times(rec->cs50_size, rec->size));
	printf(C_CS50 "%.*f\t" C_RESET, 3, rec->cs50_size);
	printf("%s", compare_times(rec->size, rec->cs50_size));
	printf(C_YOURS "%.*f\t" C_RESET, 3, rec->size);

	printf("%s", compare_times(rec->cs50_unload, rec->unload));
	printf(C_CS50 "%.*f\t" C_RESET, 3, rec->cs50_unload);
	printf("%s", compare_times(rec->unload, rec->cs50_unload));
	printf(C_YOURS "%.*f\t" C_RESET, 3, rec->unload);

	printf("%s", compare_times(rec->cs50_total, rec->total));
	printf(C_CS50 "%.*f\t" C_RESET, 3, rec->cs50_total);
	printf("%s", compare_times(rec->total, rec->cs50_total));
	printf(C_YOURS "%.*f\t" C_RESET, 3, rec->total);

	printf("\n");
}

/**
 * @returns string constant bold if num1 is less
 */
static char *compare_times(float num1, float num2) {
	const float epsilon = 0.0001F;

	// no staff solution or just small diff
	if (fminf(num1, num2) < epsilon || fabsf(num1 - num2) <= epsilon)
		return "";

	if (num1 < num2) return C_BOLD;

	// num2 greater
	return "";
}
