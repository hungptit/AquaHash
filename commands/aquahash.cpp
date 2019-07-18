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

#include "aquahash.h"
#include "params.h"
#include "aquahash_policy.h"
#include "clara.hpp"
#include "fmt/format.h"
#include "interface.h"
#include "reader.h"
#include "utils.h"
#include "xxhash_policy.h"
#include <string>

using Params = aquahash::Params;

namespace {

    void compute(int argc, char *argv[]) {
        Params params;
        std::vector<std::string> files;
        std::tie(params, files) = aquahash::parse_input_arguments(argc, argv);
        if (params.use_aquahash()) {
            for (auto &file : files) {
                aquahash::FileReader<aquahash::AquaHashPolicy<Params>> hasher(params);
                hasher(file.data());
            }
        } else if (params.use_xxhash()) {
            // for (auto &file : files) {
            //     aquahash::FileReader<aquahash::XXHashPolicy<Params>> hasher(params);
            //     hasher(file.data());
            // }
        } else {
            fprintf(stderr, "Unrecognized hashing algorithm.");
        }
    }
} // namespace

int main(int argc, char *argv[]) {
    compute(argc, argv);
    return EXIT_SUCCESS;
}
