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

#include <cstdio>
#include <vector>
#include <string>
#include "clara.hpp"
#include "fmt/format.h"

namespace aquahash {
    void disp_version() { printf("%s\n", "aquahash version 1.0"); }
    void copyright() { printf("\n%s\n", "Report bugs or enhancement requests to hungptit@gmail.com"); }
    void usage() {
        printf("\nExamples:\n");
        printf("\taquahash afile\n");
        printf("\taquahash file1 file2 file3\n");
    }

    enum PARAMS : uint32_t {
        NONE = 0,
        VERBOSE = 1,
        COLOR = 1 << 1,
        AQUAHASH = 1 << 2,
        XXHASH = 1 << 3,
    };

    struct Params {
        int flags;
        bool use_xxhash() const { return (flags & XXHASH) > 0; }
        bool use_aquahash() const { return (flags & AQUAHASH) > 0; }
        bool verbose() const { return (flags & VERBOSE) > 0; }
        bool color() const { return (flags & COLOR) > 0; }
        void print() const {
            printf("verbose: %s\n", verbose() ? "yes" : "no");
            printf("color: %s\n", color() ? "yes" : "no");
            printf("use_xxhash: %s\n", use_xxhash() ? "yes" : "no");
            printf("use_aquahash: %s\n", use_aquahash() ? "yes" : "no");
        }
    };

    auto parse_input_arguments(int argc, char *argv[]) {
        Params params;
        bool verbose = false;
        bool version = false;
        bool color = false;
        bool help = false;

        bool aquahash = true;
        bool xxhash = false;

        std::vector<std::string> files;

        auto cli =
            clara::Help(help) | clara::Opt(verbose)["-v"]["--verbose"]("Display verbose information") |
            clara::Opt(verbose)["--verbose"]("Display the verbose information.") |
            clara::Opt(version)["--version"]("Display the version of aquahash command.") |
            clara::Opt(color)["--color"]("Use color text.") |
            clara::Opt(xxhash)["--xxhash"]("Use the xxHash64 algorithm to compute the checksum.") |
            clara::Opt(aquahash)["--aquahash"]("Use the AquaHash algorithm to compute the checksum.") |
            clara::Arg(files, "files")("Input files");

        auto result = cli.parse(clara::Args(argc, argv));
        if (!result) {
            fmt::print(stderr, "Invalid option: {}\n", result.errorMessage());
            exit(EXIT_FAILURE);
        }

        if (version) {
            disp_version();
            exit(EXIT_SUCCESS);
        }

        // Print out the help document.
        if (help) {
            std::ostringstream oss;
            oss << cli;
            fmt::print("{}", oss.str());
            usage();
            copyright();
            exit(EXIT_SUCCESS);
        }

        params.flags = (xxhash ? XXHASH : AQUAHASH) | (verbose ? VERBOSE : NONE) | (color ? COLOR : NONE);

        // Display input arguments in JSON format if verbose flag is on
        if (params.verbose()) {
            params.print();
        }

        return std::make_tuple(params, files);
    }

} // namespace aquahash
