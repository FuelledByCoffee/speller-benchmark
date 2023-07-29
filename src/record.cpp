
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <fmt/std.h>

#include <colors.hpp>
#include <record.hpp>

#include <string_view>

auto operator<<(std::ostream &os, record const &rec) -> std::ostream & {

    /**
     * @returns string constant bold if num1 is less
     */
    auto compare_times = [](float num1, float num2) {
        const float epsilon = 0.0001F;

        // no staff solution or just small diff
        if (fminf(num1, num2) < epsilon || fabsf(num1 - num2) <= epsilon)
            return "";

        if (num1 < num2)
            return C_BOLD;

        // num2 greater
        return "";
    };

    using namespace fmt::literals;

    os << fmt::format("{: >16} ", rec.filename);
    std::fflush(stdout);

    // print status
    if (rec.success)
        os << fmt::format(fmt::emphasis::bold | fg(fmt::color::green), "OK\t");
    else
        os << fmt::format(fmt::emphasis::bold | fg(fmt::color::red), "ERROR\t");
    os << fmt::format(C_RESET);

    // load
    os << compare_times(rec.load.first, rec.load.second); // bold?
    os << C_CS50 << std::fixed << std::setprecision(3) << rec.load.first
       << C_RESET << '\t';

    os << compare_times(rec.load.second, rec.load.first); // bold?
    os << C_YOURS << std::fixed << std::setprecision(3) << rec.load.second
       << C_RESET << '\t';

    // check
    os << compare_times(rec.check.first, rec.load.second); // bold?
    os << C_CS50 << std::fixed << std::setprecision(3) << rec.check.first
       << C_RESET << '\t';

    os << compare_times(rec.check.second, rec.check.first); // bold?
    os << C_YOURS << std::fixed << std::setprecision(3) << rec.check.second
       << C_RESET << '\t';

    // size
    os << compare_times(rec.size.first, rec.load.second); // bold?
    os << C_CS50 << std::fixed << std::setprecision(3) << rec.size.first
       << C_RESET << '\t';

    os << compare_times(rec.size.second, rec.size.first); // bold?
    os << C_YOURS << std::fixed << std::setprecision(3) << rec.check.second
       << C_RESET << '\t';

    // unload
    os << compare_times(rec.unload.first, rec.unload.second); // bold?
    os << C_CS50 << std::fixed << std::setprecision(3) << rec.unload.first
       << C_RESET << '\t';

    os << compare_times(rec.unload.second, rec.unload.first); // bold?
    os << C_YOURS << std::fixed << std::setprecision(3) << rec.unload.second
       << C_RESET << '\t';

    // total
    os << compare_times(rec.total.first, rec.total.second); // bold?
    os << C_CS50 << std::fixed << std::setprecision(3) << rec.total.second
       << C_RESET << '\t';

    os << compare_times(rec.total.second, rec.total.first); // bold?
    os << C_YOURS << std::fixed << std::setprecision(3) << rec.total.first
       << C_RESET << '\t';

    return os;
}
