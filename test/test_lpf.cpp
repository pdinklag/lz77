/**
 * test_lpf.cpp
 * part of pdinklag/lz77
 * 
 * MIT License
 * 
 * Copyright (c) Patrick Dinklage
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <lz77/lpf_factorizer.hpp>
#include "decode.hpp"

namespace lz77::test {

TEST_SUITE("lpf_factorizer") {
    std::string text = "ananasbananapanamabahamascabana";

    TEST_CASE("LPFFactorizer") {
        LPFFactorizer lpf;
        std::vector<Factor> factors;
        factors.reserve(17);
        auto out = std::back_inserter(factors);
        lpf.factorize(text.begin(), text.end(), out);

        CHECK(factors.size() == 17);
        CHECK(factors[0] == Factor('a'));
        CHECK(factors[1] == Factor('n'));
        CHECK(factors[2] == Factor(2, 3));
        CHECK(factors[3] == Factor('s'));
        CHECK(factors[4] == Factor('b'));
        CHECK(factors[5] == Factor(7, 5));
        CHECK(factors[6] == Factor('p'));
        CHECK(factors[7] == Factor(6, 3));
        CHECK(factors[8] == Factor('m'));
        CHECK(factors[9] == Factor('a'));
        CHECK(factors[10] == Factor(12, 2));
        CHECK(factors[11] == Factor('h'));
        CHECK(factors[12] == Factor(6, 3));
        CHECK(factors[13] == Factor('s'));
        CHECK(factors[14] == Factor('c'));
        CHECK(factors[15] == Factor(9, 3));
        CHECK(factors[16] == Factor(15, 2));

        auto const dec = decode(factors.begin(), factors.end());
        CHECK(dec == text);
    }
}

}
