# nanomath
> dead simple timestamp arithmetic :clock10:

[![build](https://dl.circleci.com/status-badge/img/gh/zdryan/nanomath/tree/develop.svg?style=shield&circle-token=8c8e2eb77ff0c50b0691c599af98998751d38785)](https://dl.circleci.com/status-badge/redirect/gh/zdryan/nanomath/tree/develop)
[![License: Unlicense](https://img.shields.io/badge/license-Unlicense-blue.svg)](http://unlicense.org/)

```bash
# ISO 8601 (default)
nanomath '2004-11-23T12:30:00Z + 4Y - 10D - (12h + 30m)' # 2008-11-12T00:00:00Z

# or with given unit
nanomath --format ns '2004-11-23T12:30:00Z + 4Y - 10D - (12h + 30m)' # 1226448000000000000ns

# or even...
nanomath --format m '2008-11-12T00:00:00Z - 2004-11-23T12:30:00Z' # 2087250m
```

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
