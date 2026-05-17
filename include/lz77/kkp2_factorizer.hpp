/**
 * lz77/kkp2_factorizer.hpp
 * part of pdinklag/lz77
 * 
 * MIT License
 * 
 * Copyright (c) Patrick Dinklage
 * Copyright (c) 2013 Juha Karkkainen, Dominik Kempa and Simon J. Puglisi
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

#ifndef _LZ77_KKP2_FACTORIZER_HPP
#define _LZ77_KKP2_FACTORIZER_HPP

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>

#include <libsais.h>
#include <libsais64.h>

#ifdef LIBSAIS_OPENMP
#include <omp.h>
#endif

#include "emit_function.hpp"

namespace lz77 {

/**
 * \brief Computes an exact Lempel-Ziv 77 factorization using the KKP2 algorithm.
 * 
 * The algorithm first computes the suffix and next smaller value array, then computes
 * previous smaller values on the fly. 
 * It comes from the popular paper "Linear Time Lempel-Ziv Factorization: Simple, Fast, Small"
 * due to Kärkkäinen, Kempa and Puglisi [CPM 2013].
 * 
 * In the case of multiple sources being eligible for a factor, tie breaking is done based on the lexicographic order.
 * In other words, the factorization is neither leftmost nor rightmost.
 */
class KKP2Factorizer {
private:
    static constexpr size_t MAX_SIZE_32BIT = 1ULL << 31;

    static constexpr size_t STACK_BITS = 16;
    static constexpr size_t STACK_SIZE = 1ULL << STACK_BITS;
    static constexpr size_t STACK_HALF = STACK_SIZE/2;
    static constexpr size_t STACK_MASK = STACK_SIZE - 1;

    static size_t lce(std::string_view const& t, size_t const i, size_t const j) {
        auto const n = t.length();

        size_t l = 0;
        while(i + l < n && j + l < n && t[i + l] == t[j + l]) ++l;

        return l;
    }

    size_t min_ref_len_;

    template<bool require_64bit>
    void factorize(std::string_view const& t, EmitFunction emit_literal, EmitFunction emit_reference) {
        using Index = std::conditional_t<require_64bit, uint64_t, uint32_t>;
        using SignedIndex = std::make_signed_t<Index>;

        // construct index data structures
        Index const n = t.size();
        auto cs = std::make_unique<SignedIndex[]>(n+5);
        {
            auto sa = std::make_unique<Index[]>(n);
            
            if constexpr(require_64bit) {
                #ifdef LIBSAIS_OPENMP
                libsais64_omp((uint8_t const*)t.data(), (int64_t*)sa.get(), n, 0, nullptr, omp_get_max_threads());
                #else
                libsais64((uint8_t const*)t.data(), (int64_t*)sa.get(), n, 0, nullptr);
                #endif
            } else {
                #ifdef LIBSAIS_OPENMP
                libsais_omp((uint8_t const*)t.data(), (int32_t*)sa.get(), n, 0, nullptr, omp_get_max_threads());
                #else
                libsais((uint8_t const*)t.data(), (int32_t*)sa.get(), n, 0, nullptr);
                #endif
            }

            // construct
            auto stack = std::make_unique<SignedIndex[]>(STACK_SIZE + 5);
            SignedIndex top = 0;
            stack[top] = 0;

            cs[0] = -1;
            for(size_t i = 1; i <= n; i++) {
                auto const sai = sa[i-1] + 1;
                while(stack[top] > sai) --top;

                if((top & STACK_MASK) == 0) {
                    if (stack[top] < 0) {
                        // Stack empty -- use implicit.
                        top = -stack[top];
                        while (top > sai) top = cs[top];
                        stack[0] = -cs[top];
                        stack[1] = top;
                        top = 1;
                    } else if (top == STACK_SIZE) {
                        // Stack is full -- discard half.
                        for (size_t j = STACK_HALF; j <= STACK_SIZE; j++) {
                            stack[j - STACK_HALF] = stack[j];
                        }
                        stack[0] = -stack[0];
                        top = STACK_HALF;
                    }
                }

                cs[sai] = std::max(SignedIndex(0), stack[top]);
                ++top;
                stack[top] = sai;
            }
        }

        // factorize
        cs[0] = 0;
        size_t next = 1;
        for(size_t i = 1; i <= n; i++) {
            auto const psv = cs[i];
            auto const nsv = cs[psv];
            if(i == next) {
                size_t const psv_lcp = psv >= 0 ? lce(t, i-1, (size_t)psv-1) : 0;
                size_t const nsv_lcp = nsv >= 0 ? lce(t, i-1, (size_t)nsv-1) : 0;

                //select maximum
                size_t const max_lcp = std::max(psv_lcp, nsv_lcp);
                if(max_lcp >= min_ref_len_) {
                    ssize_t const max_pos = (max_lcp == psv_lcp) ? psv : nsv;
                    assert(max_pos >= 0);
                    assert(max_pos < i);
                    
                    // emit reference
                    emit_reference(Factor(i - max_pos, max_lcp));
                    next += max_lcp; //advance
                } else {
                    // emit literal
                    emit_literal(Factor(t[i-1]));
                    ++next; //advance
                }
            }
            cs[i] = nsv;
            cs[psv] = i;
        }
    }

public:
    KKP2Factorizer() : min_ref_len_(2) {
    }

    template<std::contiguous_iterator Input>
    requires (sizeof(std::iter_value_t<Input>) == 1)
    void factorize(Input begin, Input const& end, EmitFunction emit_literal, EmitFunction emit_reference) {
        std::string_view const t(begin, end);
        size_t const n = t.size();

        if(n < MAX_SIZE_32BIT) {
            factorize<false>(t, emit_literal, emit_reference);
        } else {
            factorize<true>(t, emit_literal, emit_reference);
        }
    }

    template<std::contiguous_iterator Input, std::output_iterator<Factor> Output>
    requires (sizeof(std::iter_value_t<Input>) == 1)
    void factorize(Input begin, Input const& end, Output out) {
        auto emit = [&](Factor f){ *out++ = f; };
        factorize(begin, end, emit, emit);
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
