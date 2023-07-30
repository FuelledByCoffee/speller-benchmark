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

	auto operator+=(record const &other) -> record & {
		load   += other.load;
		check  += other.check;
		size   += other.size;
		unload += other.unload;
		total  += other.total;

		return *this;
	}

	friend auto operator+(record lhs, record rhs) -> record {
		lhs += rhs;
		return lhs;
	}

	template <typename Int>
	auto operator/=(Int divisor) -> record & {
		load   /= divisor;
		check  /= divisor;
		size   /= divisor;
		unload /= divisor;
		total  /= divisor;

		return *this;
	}
};

class benchmark {
 public:
	benchmark() = default;

	benchmark(std::filesystem::path const &txtfile, bool bench_staff) //
		: txt(txtfile), m_staff(bench_staff) {}

	void run() {
		if (m_staff) cs50.run("speller50", txt);
		yours.run("speller", txt);
	}

	auto operator+=(benchmark const &other) -> benchmark & {
		yours += other.yours;
		cs50  += other.cs50;
		return *this;
	}

	friend auto operator+(benchmark lhs, benchmark rhs) -> benchmark {
		lhs += rhs;
		return lhs;
	}

	template <typename Int = int>
	friend auto operator/(benchmark b, Int divisor) -> benchmark {
		b.yours /= divisor;
		b.cs50  /= divisor;

		return b;
	}

	friend auto operator<<(std::ostream &os, benchmark const &results)
			-> std::ostream &;


 public:
	std::filesystem::path txt;

	record yours{};
	record cs50{};

	bool m_staff = false;
};

template <>
struct fmt::formatter<benchmark> : ostream_formatter {};
