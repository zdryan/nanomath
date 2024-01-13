#pragma once

#include "date/date.h"
#include "date/tz.h"
#include <chrono>
#include <cmath>
#include <concepts>
#include <format>
#include <ranges>
#include <regex>
#include <stack>
#include <unordered_map>

namespace nanomath
{

inline constexpr int64_t count(const auto &duration)
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}

inline constinit auto MICROSECONDS = count(std::chrono::microseconds{1});
inline constinit auto MILLISECONDS = count(std::chrono::milliseconds{1});
inline constinit auto SECONDS = count(std::chrono::seconds{1});
inline constinit auto MINUTES = count(std::chrono::minutes{1});
inline constinit auto HOURS = count(std::chrono::hours{1});
inline constinit auto DAYS = count(std::chrono::days{1});
inline constinit auto WEEKS = count(std::chrono::weeks{1});
inline constinit auto MONTHS = count(std::chrono::months{1});
inline constinit auto YEARS = count(std::chrono::days{365});

enum class Unit
{
    NANOSECONDS,
    MICROSECONDS,
    MILLISECONDS,
    SECONDS,
    MINUTES,
    HOURS,
    DAYS,
    WEEKS,
    MONTHS,
    YEARS
};

inline Unit string_to_unit(std::string_view unit)
{
    // clang-format off
    static const std::unordered_map<std::string_view, Unit> units =
    {
        {"ns", Unit::NANOSECONDS},
	{"µs", Unit::MICROSECONDS},
        {"us", Unit::MICROSECONDS},
        {"ms", Unit::MILLISECONDS},
        {"s", Unit::SECONDS},
        {"m", Unit::MINUTES},
        {"h", Unit::HOURS},
        {"D", Unit::DAYS},
        {"W", Unit::WEEKS},
        {"M", Unit::MONTHS},
        {"Y", Unit::YEARS}
    };
    // clang-format on
    if (units.contains(unit))
        return units.at(unit);
    throw std::format_error(std::format("unknown unit: '[{}]'", unit));
}

inline constexpr int64_t factor(Unit unit)
{
    switch (unit)
    {
    case Unit::NANOSECONDS:
        return 1;
    case Unit::MICROSECONDS:
        return MICROSECONDS;
    case Unit::MILLISECONDS:
        return MILLISECONDS;
    case Unit::SECONDS:
        return SECONDS;
    case Unit::MINUTES:
        return MINUTES;
    case Unit::HOURS:
        return HOURS;
    case Unit::DAYS:
        return DAYS;
    case Unit::WEEKS:
        return WEEKS;
    case Unit::MONTHS:
        return MONTHS;
    case Unit::YEARS:
        return YEARS;
    }
    __builtin_unreachable();
}

template <typename... Args> inline std::string format(double count, std::string_view unit)
{
    double int_count;
    if (std::modf(count, &int_count) != 0)
        return std::format("{:.2f}{}", count, unit);
    else
        return std::format("{}{}", static_cast<int64_t>(count), unit);
}

inline std::string format(const std::chrono::nanoseconds &nanos)
{
    const auto time_point = std::chrono::sys_time<std::chrono::nanoseconds>(nanos);
    return std::format("{:%Y-%m-%dT%H:%M:%S}Z", std::chrono::time_point_cast<std::chrono::seconds>(time_point));
}

inline std::string format(const std::chrono::nanoseconds &nanos, Unit unit)
{
    std::ostringstream oss;
    switch (unit)
    {
    case Unit::NANOSECONDS:
        return std::format("{}ns", nanos.count());
    case Unit::MICROSECONDS:
        return format(nanos.count() / static_cast<double>(MICROSECONDS), "us");
    case Unit::MILLISECONDS:
        return format(nanos.count() / static_cast<double>(MILLISECONDS), "ms");
    case Unit::SECONDS:
        return format(nanos.count() / static_cast<double>(SECONDS), "s");
    case Unit::MINUTES:
        return format(nanos.count() / static_cast<double>(MINUTES), "m");
    case Unit::HOURS:
        return format(nanos.count() / static_cast<double>(HOURS), "h");
    case Unit::DAYS:
        return format(nanos.count() / static_cast<double>(DAYS), "D");
    case Unit::WEEKS:
        return format(nanos.count() / static_cast<double>(WEEKS), "W");
    case Unit::MONTHS:
        return format(nanos.count() / static_cast<double>(MONTHS), "M");
    case Unit::YEARS:
        return format(nanos.count() / static_cast<double>(YEARS), "Y");
    }
    __builtin_unreachable();
}

template <std::signed_integral T> constexpr auto add_overflow(const T &lhs, const T &rhs)
{
    T result;
    if (__builtin_saddl_overflow(lhs, rhs, &result))
        throw std::overflow_error("addition overflow");
    return result;
}

template <std::signed_integral T> constexpr auto sub_overflow(const T &lhs, const T &rhs)
{
    T result;
    if (__builtin_ssubl_overflow(lhs, rhs, &result))
        throw std::overflow_error("subtraction overflow");
    return result;
}

inline std::string replace(std::string_view input)
{
    std::string result;
    std::match_results<decltype(input.cbegin())>::difference_type last_pos = 0;
    auto match_end = input.cbegin();
    static const auto ISO_8601_PATTERN =
        std::regex{R"(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(Z|[+-]\d{2}(?::\d{2})?|\s[a-zA-Z]+/[a-zA-Z]+)?)"};
    std::cregex_iterator begin{input.cbegin(), input.cend(), ISO_8601_PATTERN}, end;
    std::for_each(begin, end, [&](const auto &match) {
        const auto pos = match.position(0);
        auto match_begin = match_end;
        std::advance(match_begin, pos - last_pos);
        result.append(match_end, match_begin);

        const auto designator = match.str(1);
        std::istringstream iss{match.str()};
        std::ostringstream oss;
        if (designator.empty() || designator == "Z")
        {
            date::sys_time<std::chrono::nanoseconds> time;
            iss >> date::parse("%Y-%m-%dT%H:%M:%S", time);
            oss << time.time_since_epoch();
        }
        else if (std::regex_match(designator.begin(), designator.end(), std::regex{R"([+-]\d{2}(?::\d{2})?)"}))
        {
            date::sys_time<std::chrono::nanoseconds> time;
            iss >> date::parse("%Y-%m-%dT%H:%M:%S%z", time);
            oss << time.time_since_epoch();
        }
        else if (std::regex_match(designator.begin(), designator.end(), std::regex{R"(\s[a-zA-Z]+/[a-zA-Z]+)"}))
        {
            std::string abbrev;
            date::local_time<std::chrono::nanoseconds> time;
            iss >> date::parse("%Y-%m-%dT%H:%M:%S %Z", time, abbrev);
            oss << date::locate_zone(abbrev)->to_sys(time).time_since_epoch();
        }
        else
        {
            throw std::format_error(std::format("invalid timezone designator: '{}'", designator));
        }
        result.append(oss.str());

        const auto match_len = match.length(0);
        last_pos = pos + match_len;
        if (pos > 0 && std::isdigit(input[pos - 1]))
            throw std::format_error(
                std::format("invalid leading date time character: '[{}]{}'", input[pos - 1], match.str()));
        if (static_cast<std::size_t>(last_pos) < input.length() && std::isdigit(input[last_pos]))
            throw std::format_error(
                std::format("invalid trailing date time character: '{}[{}]'", match.str(), input[last_pos]));

        match_end = match_begin;
        std::advance(match_end, match_len);
    });
    result.append(match_end, input.cend());
    return result;
}

inline std::pair<int64_t, std::size_t> parse_duration(std::string_view input)
{
    int64_t val = 0;
    auto begin = input.cbegin();
    do
    {
        val = val * 10 + (*begin++ - '0');
    } while (begin != input.cend() && std::isdigit(*begin));

    auto end = begin;
    while (begin != input.cend() && std::isalpha(*end))
        ++end;

    if (const auto unit_len = std::distance(begin, end); unit_len)
    {
        auto unit = input.substr(std::distance(input.cbegin(), begin), unit_len);
        val *= factor(string_to_unit(unit));
    }

    return {val, std::distance(input.cbegin(), end)};
}

inline std::chrono::nanoseconds parse(std::string_view expression)
{
    const auto replaced = nanomath::replace(expression);
    std::stack<char> operators;
    std::stack<int64_t> operands;
    auto begin = replaced.cbegin();

    const auto evaluate = [&]() {
        const auto rhs = operands.top();
        operands.pop();
        int64_t lhs = 0;
        if (!operands.empty())
        {
            lhs = operands.top();
            operands.pop();
        }
        const auto count = operators.top() == '+' ? add_overflow(lhs, rhs) : sub_overflow(lhs, rhs);
        operands.push(count);
        operators.pop();
    };

    while (begin != replaced.cend())
    {
        if (std::isspace(*begin))
        {
            ++begin;
        }
        else if (std::isdigit(*begin))
        {
            const auto [count, len] = parse_duration(std::string_view{begin, replaced.cend()});
            operands.push(count);
            begin += len;
        }
        else if (*begin == '(')
        {
            operators.push(*begin++);
        }
        else if (*begin == ')')
        {
            while (!operators.empty() && operators.top() != '(')
                evaluate();
            operators.pop();
            ++begin;
        }
        else if (*begin == '+' || *begin == '-')
        {
            if (!operators.empty() && operators.top() != '(')
                evaluate();
            operators.push(*begin);
            ++begin;
        }
        else
        {
            throw std::format_error(std::format("invalid expression: '{}'", expression));
        }
    }

    while (!operators.empty())
        evaluate();

    assert(operands.size() == 1);
    return std::chrono::nanoseconds{operands.top()};
}

} // namespace nanomath
