#pragma once

#include <fmt/ostream.h>

#include <filesystem>
#include <ostream>
#include <string_view>

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

	constexpr auto operator+=(record const &other) -> record & {
		load   += other.load;
		check  += other.check;
		size   += other.size;
		unload += other.unload;
		total  += other.total;

		return *this;
	}

	constexpr auto operator+(record other) const -> record {
		return other += *this;
	}

	template <typename Int = int>
	constexpr auto operator/=(Int divisor) -> record & {
		auto div = static_cast<float>(divisor);
		load   /= div;
		check  /= div;
		size   /= div;
		unload /= div;
		total  /= div;

		return *this;
	}
};

class benchmark {
public:
	benchmark() = default;

	benchmark(std::filesystem::path const &txtfile, bool bench_staff) //
		: txt(txtfile), m_staff(bench_staff) {}

	template <typename F>
	void run(F&& callback) {
		if (m_staff) cs50.run("speller50", txt);
		yours.run("speller", txt);
		callback(*this);
	}

	friend auto operator<(benchmark const &left, benchmark const &right) {
		return left.yours.total < right.yours.total;
	}

	friend auto operator>(benchmark const &left, benchmark const &right) {
		return right < left;
	}

	friend auto operator<=(benchmark const &left, benchmark const &right) {
		return !(right < left);
	}

	friend auto operator>=(benchmark const &left, benchmark const &right) {
		return !(right > left);
	}

	friend auto operator==(benchmark const &left, benchmark const &right) {
		return left.yours.total == right.yours.total;
	}

	friend auto operator!=(benchmark const &left, benchmark const &right) {
		return !(left == right);
	}

	auto operator+=(benchmark const &other) -> benchmark & {
		yours += other.yours;
		cs50  += other.cs50;
		return *this;
	}

	auto operator+(benchmark rhs) const -> benchmark { return rhs += *this; }

	template <typename Int = int>
	auto operator/=(Int divisor) -> benchmark & {
		yours /= divisor;
		cs50  /= divisor;

		return *this;
	}

	template <typename Int = int>
	friend auto operator/(benchmark b, Int divisor) -> benchmark {
		return b /= divisor;
	}

	friend auto operator<<(std::ostream    &os,
	                       benchmark const &results) -> std::ostream &;

	std::filesystem::path txt;

private:
	record yours{};
	record cs50{};

	bool m_staff = false;
};

template <>
struct fmt::formatter<benchmark> : ostream_formatter {};
