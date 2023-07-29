#pragma once

#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <fmt/std.h>

#include <filesystem>
#include <iostream>
#include <string>
#include <utility>

struct record {
    std::string filename;
    std::filesystem::path path;

    std::pair<int, int> misspelled;
    std::pair<int, int> dictionary;
    std::pair<int, int> text;

    std::pair<float, float> load;
    std::pair<float, float> check;
    std::pair<float, float> size;
    std::pair<float, float> unload;
    std::pair<float, float> total;

    bool success;

    friend auto operator<<(std::ostream &os, record const &rec)
        -> std::ostream &;
};

template <> struct fmt::formatter<record> {
    // Presentation format: 'f' - fixed, 'e' - exponential.
    char presentation = 'f';

    // Parses format specifications of the form ['f' | 'e'].
    constexpr auto parse(format_parse_context &ctx)
        -> format_parse_context::iterator {
        // [ctx.begin(), ctx.end()) is a character range that contains a part of
        // the format string starting from the format specifications to be
        // parsed, e.g. in
        //
        //   fmt::format("{:f} - point of interest", point{1, 2});
        //
        // the range will contain "f} - point of interest". The formatter should
        // parse specifiers until '}' or the end of the range. In this example
        // the formatter should parse the 'f' specifier and return an iterator
        // pointing to '}'.

        // Please also note that this character range may be empty, in case of
        // the "{}" format string, so therefore you should check ctx.begin()
        // for equality with ctx.end().

        // Parse the presentation format and store it in the formatter:
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'f' || *it == 'e'))
            presentation = *it++;

        // Check if reached the end of the range:
        if (it != end && *it != '}')
            ctx.on_error("invalid format");

        // Return an iterator past the end of the parsed range:
        return it;
    }

    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    auto format(const record &p, format_context &ctx) const
        -> format_context::iterator {

        auto compare_times = [](float num1, float num2) -> fmt::emphasis {
            const float epsilon = 0.0001F;

            // no staff solution or just small diff
            if (fminf(num1, num2) < epsilon || fabsf(num1 - num2) <= epsilon)
                return {};

            if (num1 < num2)
                return fmt::emphasis::bold;

            // num2 greater
            return {};
        };

        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "({:.1f}, {:.1f})", p.load.first, p.load.second);
    }
};
