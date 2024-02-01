/**
 * lz77/lpf_factorizer.hpp
 * part of pdinklag/lz77
 * 
 * MIT License
 * 
 * Copyright (c) 2023 Patrick Dinklage
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

#ifndef _LZ77_LPF_FACTOR_HPP
#define _LZ77_LPF_FACTOR_HPP

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>

#include <libsais.h>
#include <libsais64.h>

#include "factor.hpp"

namespace lz77 {

/**
 * \brief Computes an exact Lempel-Ziv 77 factorization of the input by simulating the longest previous factor (LPF) array
 * 
 * The algorithm first computes the suffix array, its inverse and the LCP array, and then uses it to simulate a scan of the LPF array
 * to compute greedily the Lempel-Ziv 77 parse.
 * 
 * In the case of multiple sources being eligible for a factor, tie breaking is done based on the lexicographic order.
 * In other words, the factorization is neither leftmost nor rightmost.
 */
class LPFFactorizer {
private:
    static constexpr size_t MAX_SIZE_32BIT = 1ULL << 31;

    static size_t lce(std::string_view const& t, size_t const i, size_t const j) {
        auto const n = t.length();

        size_t l = 0;
        while(i + l < n && j + l < n && t[i + l] == t[j + l]) ++l;

        return l;
    }

    size_t min_ref_len_;

    template<bool require_64bit, std::output_iterator<Factor> Output>
    void factorize(std::string_view const& t, Output& out) {
        using Index = std::conditional_t<require_64bit, uint64_t, uint32_t>;

        // construct suffix array, inverse suffix array and lcp array
        Index const n = t.size();
        auto sa = std::make_unique<Index[]>(n);
        auto isa = std::make_unique<Index[]>(n);

        if constexpr(require_64bit) {
            libsais64((uint8_t const*)t.data(), (int64_t*)sa.get(), n, 0, nullptr);
        } else {
            libsais((uint8_t const*)t.data(), (int32_t*)sa.get(), n, 0, nullptr);
        }
        for(Index i = 0; i < n; i++) isa[sa[i]] = i;

        // factorize
        for(size_t i = 0; i < n;) {
            // get SA position for suffix i
            size_t const cur_pos = isa[i];

            // compute PSV and NSV
            ssize_t psv_pos = (ssize_t)cur_pos - 1;
            while (psv_pos >= 0 && sa[psv_pos] > i) --psv_pos;
            size_t const psv_lcp = psv_pos >= 0 ? lce(t, i, (size_t)sa[psv_pos]) : 0;

            size_t nsv_pos = cur_pos + 1;
            while(nsv_pos < n && sa[nsv_pos] > i) ++nsv_pos;
            size_t const nsv_lcp = nsv_pos < n ? lce(t, i, (size_t)sa[nsv_pos]) : 0;

            //select maximum
            size_t const max_lcp = std::max(psv_lcp, nsv_lcp);
            if(max_lcp >= min_ref_len_) {
                ssize_t const max_pos = (max_lcp == psv_lcp) ? psv_pos : nsv_pos;
                assert(max_pos >= 0);
                assert(max_pos < n);
                assert(sa[max_pos] < i);
                
                // emit reference
                *out++ = Factor(i - sa[max_pos], max_lcp);
                i += max_lcp; //advance
            } else {
                // emit literal
                *out++ = Factor(t[i]);
                ++i; //advance
            }
        }
    }

public:
    LPFFactorizer() : min_ref_len_(2) {
    }

    template<std::contiguous_iterator Input, std::output_iterator<Factor> Output>
    requires (sizeof(std::iter_value_t<Input>) == 1)
    void factorize(Input begin, Input const& end, Output out) {
        std::string_view const t(begin, end);
        size_t const n = t.size();

        if(n < MAX_SIZE_32BIT) {
            factorize<false>(t, out);
        } else {
            factorize<true>(t, out);
        }
    }

    /**
     * \brief Reports the minimum length of a referencing factor
     * 
     * If a referencing factor is shorter than this length, a literal factor is emitted instead
     * 
     * \return the minimum reference length
     */
    size_t min_reference_length() const { return min_ref_len_; }

    /**
     * \brief Sets the minimum length of a referencing factor
     * 
     * If a referencing factor is shorter than this length, a literal factor is emitted instead
     * 
     * \param min_ref_len the minimum reference length
     */
    void min_reference_length(size_t min_ref_len) { min_ref_len_ = min_ref_len; }
};

}

#endif
