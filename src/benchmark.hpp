#pragma once

#include <fmt/ostream.h>

#include <filesystem>
#include <mutex>
#include <ostream>
#include <string_view>
#include <thread>

struct record {
	void run(std::string_view speller, std::filesystem::path const &txtfile);

	int misspelled{};
	int dictionary{};
	int text{};

	float load{};
	float check{};
	float size{};
	float unload{};
	float total{};

	bool success = true;
};

class benchmark {
  public:
	benchmark() = default;
	benchmark(std::filesystem::path const &txtfile, bool bench_staff) //
		: txt(txtfile), m_staff(bench_staff) {
		// if (multithread) {
		//     t1 = std::thread(&benchmark::run, this);
		// }
	}

	void run() {
		if (m_staff)
			cs50.run("speller50", txt);
		yours.run("speller", txt);
	}

	friend auto operator<<(std::ostream &os, benchmark const &results)
		-> std::ostream &;

	std::filesystem::path txt;

	record yours{};
	record cs50{};

	bool m_staff = false;
};

template <> struct fmt::formatter<benchmark> : ostream_formatter {};
