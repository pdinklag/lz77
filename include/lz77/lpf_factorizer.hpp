#pragma once

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>

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
    size_t min_ref_len_;

public:
    LPFFactorizer() : min_ref_len_(2) {
    }

    template<std::contiguous_iterator Input, std::output_iterator<Factor> Output>
    requires (sizeof(std::iter_value_t<Input>) == 1)
    void factorize(Input begin, Input const& end, Output out) {
        std::string_view const t(begin, end);
        size_t const n = t.size();

        // construct suffix array, inverse suffix array and lcp array
        {
            auto sa = std::make_unique<uint64_t[]>(n);
            auto lcp = std::make_unique<uint64_t[]>(n);
            auto isa = std::make_unique<uint64_t[]>(n);

            libsais64((uint8_t const*)t.data(), (int64_t*)sa.get(), n, 0, nullptr);
            libsais64_plcp((uint8_t const*)t.data(), (int64_t const*)sa.get(), (int64_t*)isa.get(), n);
            libsais64_lcp((int64_t const*)isa.get(), (int64_t const*)sa.get(), (int64_t*)lcp.get(), n);
            for(uint64_t i = 0; i < n; i++) isa[sa[i]] = i;

            // factorize
            for(size_t i = 0; i < n;) {
                // get SA position for suffix i
                size_t const cur_pos = isa[i];

                // compute naively PSV
                // search "upwards" in LCP array
                // include current, exclude last
                size_t psv_lcp = lcp[cur_pos];
                ssize_t psv_pos = (ssize_t)cur_pos - 1;
                if (psv_lcp > 0) {
                    while (psv_pos >= 0 && sa[psv_pos] > sa[cur_pos]) {
                        psv_lcp = std::min<size_t>(psv_lcp, lcp[psv_pos--]);
                    }
                }

                // compute naively NSV
                // search "downwards" in LCP array
                // exclude current, include last
                size_t nsv_lcp = 0;
                size_t nsv_pos = cur_pos + 1;
                if (nsv_pos < n) {
                    nsv_lcp = SIZE_MAX;
                    do {
                        nsv_lcp = std::min<size_t>(nsv_lcp, lcp[nsv_pos]);
                        if (sa[nsv_pos] < sa[cur_pos]) {
                            break;
                        }
                    } while (++nsv_pos < n);

                    if (nsv_pos >= n) {
                        nsv_lcp = 0;
                    }
                }

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