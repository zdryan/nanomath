#include "nanomath.hpp"
#include <gtest/gtest.h>
#include <stdexcept>

namespace nanomath::test
{

using namespace ::testing;

class NanoMathTest : public Test
{
};

TEST_F(NanoMathTest, unambiguous_duration)
{
    EXPECT_EQ(parse("1ns").count(), 1);
    EXPECT_EQ(parse("1us").count(), MICROSECONDS);
    EXPECT_EQ(parse("1ms").count(), MILLISECONDS);
    EXPECT_EQ(parse("1s").count(), SECONDS);
    EXPECT_EQ(parse("1m").count(), MINUTES);
    EXPECT_EQ(parse("1h").count(), HOURS);
    EXPECT_EQ(parse("1D").count(), DAYS);
    EXPECT_EQ(parse("1W").count(), WEEKS);
    EXPECT_EQ(parse("1M").count(), MONTHS);
    EXPECT_EQ(parse("1Y").count(), YEARS);
}

TEST_F(NanoMathTest, unambiguous_negative_duration)
{
    EXPECT_EQ(parse("-1ns").count(), -1);
    EXPECT_EQ(parse("-1us").count(), -MICROSECONDS);
    EXPECT_EQ(parse("-1ms").count(), -MILLISECONDS);
    EXPECT_EQ(parse("-1s").count(), -SECONDS);
    EXPECT_EQ(parse("-1m").count(), -MINUTES);
    EXPECT_EQ(parse("-1h").count(), -HOURS);
    EXPECT_EQ(parse("-1D").count(), -DAYS);
    EXPECT_EQ(parse("-1W").count(), -WEEKS);
    EXPECT_EQ(parse("-1M").count(), -MONTHS);
    EXPECT_EQ(parse("-1Y").count(), -YEARS);
}

TEST_F(NanoMathTest, unambiguous_duration_arithmetic)
{
    EXPECT_EQ(parse("1Y + 1M + 1W + 1D + 1h + 1m + 1s + 1ms + 1us + 1ns").count(),
              YEARS + MONTHS + WEEKS + DAYS + HOURS + MINUTES + SECONDS + MILLISECONDS + MICROSECONDS + 1);
    EXPECT_EQ(parse("1Y - 1M - 1W - 1D - 1h - 1m - 1s - 1ms - 1us - 1ns").count(),
              YEARS - MONTHS - WEEKS - DAYS - HOURS - MINUTES - SECONDS - MILLISECONDS - MICROSECONDS - 1);
    EXPECT_EQ(parse("1Y - 1M + 1W - 1D + 1h - 1m + 1s - 1ms + 1us - 1ns").count(),
              YEARS - MONTHS + WEEKS - DAYS + HOURS - MINUTES + SECONDS - MILLISECONDS + MICROSECONDS - 1);
}

TEST_F(NanoMathTest, parenthesized_duration_arithmetic)
{
    EXPECT_EQ(parse("1Y - (1M + 1W) - (1D + 1h) - (1m + 1s) - (1ms + 1us) - 1ns").count(),
              YEARS - (MONTHS + WEEKS) - (DAYS + HOURS) - (MINUTES + SECONDS) - (MILLISECONDS + MICROSECONDS) - 1);
    EXPECT_EQ(parse("1Y - (1M + 1W) - ((1D + 1h) - (1m + 1s)) - ((1ms + 1us) - 1ns)").count(),
              YEARS - (MONTHS + WEEKS) - ((DAYS + HOURS) - (MINUTES + SECONDS)) - ((MILLISECONDS + MICROSECONDS) - 1));
}

TEST_F(NanoMathTest, invalid_specifier)
{
    EXPECT_THROW(parse("1z"), std::runtime_error);
}

TEST_F(NanoMathTest, timezone_designators)
{
    EXPECT_EQ(parse("1970-01-01T00:00:00Z").count(), 0);
    EXPECT_EQ(parse("1970-01-01T00:00:00+00:00").count(), 0);
    EXPECT_EQ(parse("1970-01-01T00:00:00 Etc/UTC").count(), 0);

    EXPECT_EQ(parse("1970-01-01T00:00:00-10:00").count(), HOURS * 10);
    EXPECT_EQ(parse("1970-01-01T00:00:00 Pacific/Honolulu").count(), HOURS * 10);
}

TEST_F(NanoMathTest, invalid_timezone_designator)
{
    EXPECT_THROW(parse("1970-01-01T00:00:00Y").count(), std::runtime_error);
    EXPECT_THROW(parse("1970-01-01T00:00:00+0000"), std::runtime_error);
    EXPECT_THROW(parse("1970-01-01T00:00:00 Foo/Bar"), std::runtime_error);
}

TEST_F(NanoMathTest, unambiguous_date_time)
{
    EXPECT_EQ(parse("1970-01-01T00:00:00Z").count(), 0);
    EXPECT_EQ(parse("1970-01-01T00:00:01Z").count(), SECONDS);
    EXPECT_EQ(parse("1970-01-01T00:01:00Z").count(), MINUTES);
    EXPECT_EQ(parse("1970-01-01T01:00:00Z").count(), HOURS);
    EXPECT_EQ(parse("1970-01-02T00:00:00Z").count(), DAYS);
    EXPECT_EQ(parse("1970-02-01T00:00:00Z").count(), DAYS * 31);
    EXPECT_EQ(parse("1971-01-01T00:00:00Z").count(), DAYS * 365);
}

TEST_F(NanoMathTest, unambiguous_mixed_arithmetic)
{
    EXPECT_EQ(parse("1970-01-01T00:00:00Z + 1Y - 1M + 1W - 1D + 1h - 1m + 1s - 1ms + 1us - 1ns").count(),
              YEARS - MONTHS + WEEKS - DAYS + HOURS - MINUTES + SECONDS - MILLISECONDS + MICROSECONDS - 1);
}

TEST_F(NanoMathTest, unambiguous_mixed_arithmetic_whitespace)
{
    EXPECT_EQ(parse("1970-01-01T00:00:00Z+1ns").count(), 1);
    EXPECT_EQ(parse("1970-01-01T00:00:00Z+  1ns").count(), 1);
    EXPECT_EQ(parse("1970-01-01T00:00:00Z  +1ns").count(), 1);
    EXPECT_EQ(parse("1970-01-01T00:00:00Z  +  1ns").count(), 1);
}

TEST_F(NanoMathTest, parenthesized_mixed_arithmetic)
{
    EXPECT_EQ(parse("1970-01-01T00:00:00Z + 1Y - (1M + 1W) - (1D + 1h) - (1m + 1s) - (1ms + 1us) - 1ns").count(),
              YEARS - (MONTHS + WEEKS) - (DAYS + HOURS) - (MINUTES + SECONDS) - (MILLISECONDS + MICROSECONDS) - 1);
}

TEST_F(NanoMathTest, parenthesized_mixed_arithmetic_whitespace)
{
    EXPECT_EQ(parse("(1970-01-01T00:00:00Z+1ns)").count(), 1);
    EXPECT_EQ(parse("(1970-01-01T00:00:00Z+  1ns)").count(), 1);
    EXPECT_EQ(parse("(1970-01-01T00:00:00Z  +1ns)").count(), 1);
    EXPECT_EQ(parse("(1970-01-01T00:00:00Z  +  1ns)").count(), 1);
}

} // namespace nanomath::test
