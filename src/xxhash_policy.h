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

#define XXH_INLINE_ALL
#include "xxhash.h"
#include <stdio.h>
#include <unistd.h>
#include <utils.h>
#include <params.h>

#define XXHSUM64_DEFAULT_SEED 0

namespace aquahash {
    class XXHashWriter {
      private:
        char buffer[16];

      public:
        const char *operator()(U64 value) {
            alignas(16) uint8_t v[8];
            memcpy(v, &value, sizeof(U64));
            sprintf(buffer, "%02x%02x%02x%02x%02x%02x%02x%02x", v[0], v[1], v[2], v[3], v[4], v[5], v[6],
                    v[7]);
            return buffer;
        }
    };

    class XXHashPolicy {
      public:
        static constexpr size_t BUFFER_SIZE = 1 << 16;
        XXHashPolicy(const Params &args) : writer(), params(args) {
            (void)XXH64_reset(&state64, XXHSUM64_DEFAULT_SEED);
        }

        /* Update the hash code */
        void process(const char *buffer, const size_t len) { (void)XXH64_update(&state64, buffer, len); }

        /* Finalize the process and return the hash string. */
        void finalize(const std::string &filename) {
            U64 const hashcode = XXH64_digest(&state64);
            if (params.color()) {
                printf("\033[1;32m%s\033[0m  \033[1;34m%s\033[0m\n", writer(hashcode), filename.data());
            } else {
                printf("%s  %s\n", writer(hashcode), filename.data());
            }
        }

      private:
        XXH64_state_t state64;
        XXHashWriter writer;
        Params params;
    };
} // namespace aquahash
