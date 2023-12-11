# LZ77 Utilities for C++

This C++20 library provides factorizers for Lempel-Ziv 77-like text factorizations, including:

* Exact computation of LZ77 by simulating the longest previous factor (LPF) array using an enhanced suffix array
* Re-implementation of `gzip -9`, producing the exact factorization that the infamous gzip does with flag `-9`, however without any subsequent encoding.

The library is meant to aid research on the text of data compression and likely not useful in any production scenario.

### Requirements

This library is written in C++20, a corresponding compiler is required that fully supports concepts. Tests have been done only with GCC 11. This library incorporates [libsais](https://github.com/IlyaGrebnov/libsais) to compute suffix and LCP arrays. For building the [unit tests](#unit-tests), CMake is required.

### License

```
MIT License

Copyright (c) 2023 Patrick Dinklage

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

### Unit Tests

The library comes with very basic unit tests powered by [doctest](https://github.com/doctest/doctest).

Using CMake, you can build and run the benchmark using the following chain of commands in the repository root:

```sh
mkdir build; cd build
cmake ..
make
make test
```

## Usage

In case you use CMake, you can embed this repository into yours (e.g., as a git submodule) and add it like so:

```cmake
add_subdirectory(path/to/lz77)
```

You can then link against the `lz77` library, which will automatically add the include directory to your target.

### Factors

The `lz77::Factor` struct is used for communicating factors. It consists of two unsigned integer fields:

* `len`: the length of the factor. A length of zero indicates that this is a literal factor.
* `src`: the source of the factor. If the factor is literal, this contains the represented character. Otherwise, this is the *difference* between the factor's position in the unencoded input and the position to which it refers (&geq;1).

Both fields are stored as `uintmax_t`, thus this struct is not suited for storing many factors in a space-efficient manner but only for communication.

The helper functions `is_literal()` and `is_reference()` allow you to distinguish between the two types of factors. Using `literal()`, you can conveniently extract the litereal character encoded in `src`. The `num_literals()` functions is useful when you somehow need to advance a cursor over the input: it returns the actual length of the factor in the input, which is either the length of the referencing factor, or 1 if the factor is literal (even though `len` is zero).

### Factorization

The factorizers take an input and an end iterator to some sort of string data and compute the corresponding factorization. The following example computes an exact LZ77 factorization of a string using the `lz77::LPFFactorizer` and stores the factors in a vector:

```cpp
#include <lz77/lpf_factorizer.hpp>

// ...

std::vector<lz77::Factor> factors;
lz77::LPFFactorizer lpf;
lpf.factorize(str.begin(), str.end(), std::back_inserter(factors));
```

The usage of `lz77::Gzip9Factorizer` is equivalent.