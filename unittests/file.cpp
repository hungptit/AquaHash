#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "aquahash.h"
#include "fmt/format.h"
#include "interface.h"
#include "utils.h"
#include <string>
#include <tuple>
#include <vector>
#include <x86intrin.h>
#include "reader.h"
#include "aquahash_policy.h"

TEST_CASE("Basic tests") {
    using Hasher = aquahash::FileReader<aquahash::AquaHashPolicy>;
    Hasher hasher(0);
    hasher("file.cpp");
}
