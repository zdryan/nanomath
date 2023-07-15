# nanomath
> dead simple timestamp arithmetic :clock10:

[![CircleCI](https://dl.circleci.com/status-badge/img/gh/zdryan/nanomath/tree/main.svg?style=shield)](https://dl.circleci.com/status-badge/redirect/gh/zdryan/nanomath/tree/main)
[![License: Unlicense](https://img.shields.io/badge/license-Unlicense-blue.svg)]()

```bash
# ISO 8601 (default)
nanomath '2004-11-23T12:30:00Z + 4Y - 10D - (12h + 30m)' # 2008-11-12T00:00:00Z

# or with given unit
nanomath --format ns '2004-11-23T12:30:00Z + 4Y - 10D - (12h + 30m)' # 1226448000000000000ns

# or even...
nanomath --format m '2008-11-12T00:00:00Z - 2004-11-23T12:30:00Z' # 2087250m
```

- Supports `ns` (nanoseconds), `us` (microseconds), `ms` (milliseconds), `s` (seconds), `m` (minutes), `h` (hours), `D` (days), `W` (weeks), `M` (months) and `Y` (years) suffixes.
- Timezone designators (according to the following [TZ identifiers](https://en.wikipedia.org/wiki/List_of_tz_database_time_zones)) are supported, albeit with minimal testing.

:wrench: *nanomath* is currently under active development. 

## Build
```bash
git clone https://github.com/zdryan/nanomath
cd nanomath
cmake -S . -B build
make -C build
```

## Install
```bash
cd nanomath
cmake --install build
```
