/**
 * decode.hpp
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

#include <cassert>
#include <concepts>
#include <iterator>
#include <lz77/factor.hpp>

namespace lz77::test {

template<std::input_iterator It>
requires std::same_as<std::iter_value_t<It>, Factor>
std::string decode(It begin, It const end) {
    std::string dec;
    while(begin != end) {
        auto const f = *begin++;
        if(f.is_reference()) {
            auto const src = dec.length() - f.src;
            for(size_t i = 0; i < f.len; i++) {
                dec.push_back(dec[src + i]);
            }
        } else {
            dec.push_back(f.literal());
        }
    }
    return dec;
}

}
