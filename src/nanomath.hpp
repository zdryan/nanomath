#pragma once

#include "date/date.h"
#include "date/tz.h"
#include <chrono>
#include <concepts>
#include <cstdlib>
#include <format>
#include <iostream>
#include <iterator>
#include <numbers>
#include <numeric>
#include <ranges>
#include <ratio>
#include <regex>
#include <sstream>
#include <stack>
#include <stdexcept>

namespace nanomath
{

inline constexpr auto count(auto &&duration)
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

inline constexpr int64_t specifier_factor(std::string_view specifier)
{
    if (specifier == "ns")
    {
        return 1;
    }
    else if (specifier == "us")
    {
        return MICROSECONDS;
    }
    else if (specifier == "ms")
    {
        return MILLISECONDS;
    }
    else if (specifier == "s")
    {
        return SECONDS;
    }
    else if (specifier == "m")
    {
        return MINUTES;
    }
    else if (specifier == "h")
    {
        return HOURS;
    }
    else if (specifier == "D")
    {
        return DAYS;
    }
    else if (specifier == "W")
    {
        return WEEKS;
    }
    else if (specifier == "M")
    {
        return MONTHS;
    }
    else if (specifier == "Y")
    {
        return YEARS;
    }

    throw std::runtime_error(std::format("unknown specifier [{}]", specifier));
}

inline std::string format(const std::chrono::sys_time<std::chrono::nanoseconds> &time_point, std::string_view type)
{
    if (type == "iso")
    {
        return std::format("{:%Y-%m-%d %H:%M:%S}", time_point);
    }
    else if (type == "ns")
    {
        const auto ns = time_point.time_since_epoch().count();
        return std::format("{}ns", ns);
    }
    else if (type == "us")
    {
        const auto us = std::chrono::duration_cast<std::chrono::microseconds>(time_point.time_since_epoch()).count();
        return std::format("{}us", us);
    }
    else if (type == "m")
    {
        const auto m = std::chrono::duration_cast<std::chrono::minutes>(time_point.time_since_epoch()).count();
        return std::format("{}m", m);
    }
    throw std::format_error("invalid format: '" + std::string{type} + "'");
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

inline std::string normalize_iso8601(std::string_view input)
{
    std::string result;
    std::match_results<decltype(input.cbegin())>::difference_type last_pos = 0;
    auto match_end = input.cbegin();
    static const auto ISO_8601_PATTERN =
        std::regex{R"(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(Z|[+-]\d{2}:\d{2}|\s[a-zA-Z]+/[a-zA-Z]+))"};
    std::cregex_iterator begin{input.cbegin(), input.cend(), ISO_8601_PATTERN}, end;
    std::for_each(begin, end, [&](const auto &match) {
        const auto pos = match.position(0);
        auto match_begin = match_end;
        std::advance(match_begin, pos - last_pos);
        result.append(match_end, match_begin);

        const auto designator = match.str(1);
        std::istringstream ss{match.str()};
        if (std::regex_match(designator.begin(), designator.end(), std::regex{R"([+-]\d{2}:\d{2})"}))
        {
            date::sys_time<std::chrono::nanoseconds> time;
            ss >> date::parse("%Y-%m-%dT%H:%M:%S%z", time);
            result.append(std::to_string(time.time_since_epoch().count()) + "ns");
        }
        else if (std::regex_match(designator.begin(), designator.end(), std::regex{R"((?:Z|\s[a-zA-Z]+/[a-zA-Z]+))"}))
        {
            std::string abbrev;
            date::local_time<std::chrono::nanoseconds> time;
            ss >> date::parse("%Y-%m-%dT%H:%M:%S %Z", time, abbrev);
            if (designator == "Z")
            {
                const auto count = time.time_since_epoch().count();
                result.append(std::to_string(count) + "ns");
            }
            else
            {
                const auto count = date::locate_zone(abbrev)->to_sys(time).time_since_epoch().count();
                result.append(std::to_string(count) + "ns");
            }
        }
        else
        {
            throw std::runtime_error("invalid timezone designator");
        }

        const auto match_len = match.length(0);
        last_pos = pos + match_len;
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

    if (const auto specifier_len = std::distance(begin, end); specifier_len)
    {
        auto specifier = input.substr(std::distance(input.cbegin(), begin), specifier_len);
        val *= specifier_factor(specifier);
    }

    return {val, std::distance(input.cbegin(), end)};
}

inline std::chrono::nanoseconds parse(std::string_view expression)
{
    const auto normalized = nanomath::normalize_iso8601(expression);
    std::stack<char> operators;
    std::stack<int64_t> operands;
    auto begin = normalized.cbegin();

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

    while (begin != normalized.cend())
    {
        if (std::isspace(*begin))
        {
            ++begin;
        }
        else if (std::isdigit(*begin))
        {
            const auto [count, len] = parse_duration(std::string_view{begin, normalized.cend()});
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
            throw std::runtime_error(std::format("invalid expression: '{}'", expression));
        }
    }

    while (!operators.empty())
        evaluate();

    assert(operands.size() == 1);
    return std::chrono::nanoseconds{operands.top()};
}

} // namespace nanomath
