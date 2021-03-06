// Copyright 2019 Hung Dang
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <array>
#include <cstdint>
#include <cstdio>
#include <immintrin.h>
#include <random>
#include <string>

namespace aquahash {
    class CharGenerator {
      public:
        std::string operator()(const size_t len) {
            std::string str(len, 0);
            for (size_t idx = 0; idx < len; ++idx) {
                str[idx] = valid_characters[rgn() % N];
            }
            return str;
        }

      private:
        std::mt19937 rgn;
        static constexpr int N = 62;
        const std::array<char, N> valid_characters = {
            {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u',
             'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
             'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}};
    };

    class AquaHashWriter {
      public:
        std::string operator()(__m128i value) {
            constexpr int BUFFER_SIZE = 32;
            char buffer[BUFFER_SIZE];
            alignas(16) uint8_t __attribute__((aligned(16))) v[sizeof(__m128i)];
            _mm_storeu_si128((__m128i *)v, value);
            char *ptr = buffer;
            for (int idx = 0; idx < sizeof(__m128i); ++idx) {
                const int pos = v[idx] * 2;
                *ptr++ = data[pos];
                *ptr++ = data[pos + 1];
            }
            return std::string(buffer, sizeof(__m128i) * 2);
        }

      private:
        const char *data =
            "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e"
            "2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d"
            "5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c"
            "8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babb"
            "bcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9ea"
            "ebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff";
    };
} // namespace aquahash
