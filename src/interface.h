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

#include "aquahash.h"
#include "utils.h"
#include <string>
#include <vector>

namespace aquahash {
    template <typename T> T convert(const __m128i h) noexcept {
        alignas(16) uint64_t v[2];
        _mm_store_si128((__m128i *)v, h);
        return v[0];            // Take the first part of the hash code.
    }

    template <typename T> struct hash;

    template <> struct hash<std::string> {
        using result_type = std::size_t;
        const __m128i kSeed = _mm_setzero_si128();
        result_type operator()(const std::string &key) const noexcept {
            return convert<std::size_t>(AquaHash::Hash((uint8_t *)(key.data()), key.size(), kSeed));
        }
    };

    template <> struct hash<std::vector<int>> {
        using result_type = std::size_t;
        const __m128i kSeed = _mm_setzero_si128();

        result_type operator()(const std::vector<int> &key) const noexcept {
            return convert<std::size_t>(
                AquaHash::Hash((uint8_t *)(key.data()), key.size() * sizeof(int), kSeed));
        }
    };
} // namespace aquahash
