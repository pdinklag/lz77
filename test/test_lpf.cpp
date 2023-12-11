#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <lz77/lpf_factorizer.hpp>

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
    }
}

}
