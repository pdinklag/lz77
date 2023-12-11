#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <lz77/gzip9_factorizer.hpp>

namespace lz77::test {

TEST_SUITE("gzip_factorizer") {
    std::string text = "ananasbananapanamabahamascabana";

    TEST_CASE("Gzip9Factorizer") {
        Gzip9Factorizer gzip9;
        std::vector<Factor> factors;
        factors.reserve(21);
        auto out = std::back_inserter(factors);
        gzip9.factorize(text.begin(), text.end(), out);

        CHECK(factors.size() == 21);
        CHECK(factors[0] == Factor('a'));
        CHECK(factors[1] == Factor('n'));
        CHECK(factors[2] == Factor('a'));
        CHECK(factors[3] == Factor('n'));
        CHECK(factors[4] == Factor('a'));
        CHECK(factors[5] == Factor('s'));
        CHECK(factors[6] == Factor('b'));
        CHECK(factors[7] == Factor('a'));
        CHECK(factors[8] == Factor(7, 4));
        CHECK(factors[9] == Factor('p'));
        CHECK(factors[10] == Factor(4, 3));
        CHECK(factors[11] == Factor('m'));
        CHECK(factors[12] == Factor('a'));
        CHECK(factors[13] == Factor('b'));
        CHECK(factors[14] == Factor('a'));
        CHECK(factors[15] == Factor('h'));
        CHECK(factors[16] == Factor(6, 3));
        CHECK(factors[17] == Factor('s'));
        CHECK(factors[18] == Factor('c'));
        CHECK(factors[19] == Factor('a'));
        CHECK(factors[20] == Factor(21, 4));

        /*
        auto data = gzip9.gather_data();
        CHECK(data["data"]["chain_length_max"] == 4);
        CHECK(data["data"]["chain_length_sum"] == 14);
        CHECK(data["data"]["chain_num"] == 8);
        CHECK(data["data"]["nice_num"] == 0);
        CHECK(data["data"]["good_num"] == 0);
        CHECK(data["data"]["greedy_skips"] == 2);
        CHECK(data["data"]["match_ops"] == 14);
        */
    }
}

}
