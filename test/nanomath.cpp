#include "nanomath.hpp"
#include <gtest/gtest.h>

namespace nanomath::test
{

using namespace ::testing;

class NanoMathTest : public Test
{
};

TEST_F(NanoMathTest, duration)
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

TEST_F(NanoMathTest, negative_duration)
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

TEST_F(NanoMathTest, whitespace)
{
    EXPECT_EQ(parse("  1Y  ").count(), YEARS);
    EXPECT_EQ(parse("  1971-01-01T00:00:00Z  ").count(), YEARS);
}

TEST_F(NanoMathTest, duration_arithmetic)
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

TEST_F(NanoMathTest, default_timezone_designator)
{
    EXPECT_EQ(parse("1971-01-01T00:00:00").count(), YEARS);
}

TEST_F(NanoMathTest, timezone_designator)
{
    EXPECT_EQ(parse("1970-01-01T00:00:00Z").count(), 0);
    EXPECT_EQ(parse("1970-01-01T00:00:00+00").count(), 0);
    EXPECT_EQ(parse("1970-01-01T00:00:00+00:00").count(), 0);
    EXPECT_EQ(parse("1970-01-01T00:00:00 Etc/UTC").count(), 0);

    EXPECT_EQ(parse("1970-01-01T00:00:00-10").count(), HOURS * 10);
    EXPECT_EQ(parse("1970-01-01T00:00:00-10:00").count(), HOURS * 10);
    EXPECT_EQ(parse("1970-01-01T00:00:00 Pacific/Honolulu").count(), HOURS * 10);
}

TEST_F(NanoMathTest, invalid_timezone_designator)
{
    EXPECT_THROW(parse("1970-01-01T00:00:00Y").count(), std::runtime_error);
    EXPECT_THROW(parse("1970-01-01T00:00:00+0000"), std::runtime_error);
    EXPECT_THROW(parse("1970-01-01T00:00:00 Foo/Bar"), std::runtime_error);
}

TEST_F(NanoMathTest, date_time)
{
    EXPECT_EQ(parse("1970-01-01T00:00:00Z").count(), 0);
    EXPECT_EQ(parse("1970-01-01T00:00:01Z").count(), SECONDS);
    EXPECT_EQ(parse("1970-01-01T00:01:00Z").count(), MINUTES);
    EXPECT_EQ(parse("1970-01-01T01:00:00Z").count(), HOURS);
    EXPECT_EQ(parse("1970-01-02T00:00:00Z").count(), DAYS);
    EXPECT_EQ(parse("1970-02-01T00:00:00Z").count(), DAYS * 31);
    EXPECT_EQ(parse("1971-01-01T00:00:00Z").count(), DAYS * 365);
}

TEST_F(NanoMathTest, mixed_arithmetic)
{
    EXPECT_EQ(parse("1970-01-01T00:00:00Z + 1Y - 1M + 1W - 1D + 1h - 1m + 1s - 1ms + 1us - 1ns").count(),
              YEARS - MONTHS + WEEKS - DAYS + HOURS - MINUTES + SECONDS - MILLISECONDS + MICROSECONDS - 1);
}

TEST_F(NanoMathTest, parenthesized_mixed_arithmetic)
{
    EXPECT_EQ(parse("1970-01-01T00:00:00Z + 1Y - (1M + 1W) - (1D + 1h) - (1m + 1s) - (1ms + 1us) - 1ns").count(),
              YEARS - (MONTHS + WEEKS) - (DAYS + HOURS) - (MINUTES + SECONDS) - (MILLISECONDS + MICROSECONDS) - 1);
}

TEST_F(NanoMathTest, format)
{
    const auto ns = parse("1971-01-01T00:00:00");
    EXPECT_STREQ(format(ns).c_str(), "1971-01-01T00:00:00Z");
    EXPECT_STREQ(format(ns, Unit::NANOSECONDS).c_str(), "31536000000000000ns");
    EXPECT_STREQ(format(ns, Unit::MICROSECONDS).c_str(), "31536000000000us");
    EXPECT_STREQ(format(ns, Unit::SECONDS).c_str(), "31536000s");
    EXPECT_STREQ(format(ns, Unit::MINUTES).c_str(), "525600m");
    EXPECT_STREQ(format(ns, Unit::HOURS).c_str(), "8760h");
    EXPECT_STREQ(format(ns, Unit::DAYS).c_str(), "365D");
    EXPECT_STREQ(format(ns, Unit::WEEKS).c_str(), "52.14W");
    EXPECT_STREQ(format(ns, Unit::MONTHS).c_str(), "11.99M");
    EXPECT_STREQ(format(ns, Unit::YEARS).c_str(), "1Y");
}

} // namespace nanomath::test
