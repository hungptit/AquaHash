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

#include <aquahash.h>
#include <stdio.h>
#include <unistd.h>
#include <utils.h>

namespace aquahash {
    template <typename Params> class AquaHashPolicy {
      public:
        static constexpr size_t BUFFER_SIZE = 1 << 16;
        AquaHashPolicy(Params &&args)
            : count(0),
              seed(_mm_setzero_si128()),
              hashcode(seed),
              aqua(seed),
              writer(),
              params(std::forward<Params>(args)) {}

        /* Update the hash code */
        void process(const char *buffer, const size_t len) {
            ++count;
            if ((len < BUFFER_SIZE) && (count == 1)) {
                hashcode = AquaHash::Hash(reinterpret_cast<const uint8_t *>(buffer), len, seed);
                return;
            }
            aqua.Update(reinterpret_cast<const uint8_t *>(buffer), len);
        }

        /* Finalize the process and return the hash string. */
        void finalize(const std::string &filename) {
            if (count > 1) hashcode = aqua.Finalize();
            if (params.color()) {
                printf("\033[1;32m%s\033[0m  \033[1;34m%s\033[0m\n", writer(hashcode), filename.data());
            } else {
                printf("%s  %s\n", writer(hashcode), filename.data());
            }
        }

      private:
        size_t count;
        __m128i seed;
        __m128i hashcode;
        AquaHash aqua;
        HashCodeWriter writer;
        Params params;
    };
} // namespace aquahash
