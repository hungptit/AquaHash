// Copyright 2018 J. Andrew Rogers
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

#include <cassert>
#include <cstdint>
#include <cstring>
#include <immintrin.h>
#include <limits>
#include <stdint.h>

class AquaHash {
  private:
    static constexpr unsigned int THRESHOLD = 64;
    static constexpr size_t MAXLEN = std::numeric_limits<size_t>::max() - 1;
    // sentinel to prevent double finalization
    static constexpr size_t FINALIZED = MAXLEN + 1;

    // 64 bit constants
    struct Constants {
        static constexpr int64_t MASK_32BITS = 0xffffffffLL;
        static constexpr int64_t MASK_16BITS = 0xffffLL;
        static constexpr int64_t MASK_8BITS = 0xffLL;

        // 64bits constants
        static constexpr int64_t CONSTANT_64_1 = 0xa11202c9b468bea1;
        static constexpr int64_t CONSTANT_64_2 = 0xd75157a01452495b;
        static constexpr int64_t CONSTANT_64_3 = 0xb1293b3305418592;
        static constexpr int64_t CONSTANT_64_4 = 0xd210d232c6429b69;
        static constexpr int64_t CONSTANT_64_5 = 0xbd3dc2b7b87c4715;
        static constexpr int64_t CONSTANT_64_6 = 0x6a6c9527ac2e0e4e;
        static constexpr int64_t CONSTANT_64_7 = 0xcc96ed1674eaaa03;
        static constexpr int64_t CONSTANT_64_8 = 0x1e863f24b2a8316a;
        static constexpr int64_t CONSTANT_64_9 = 0x8e51ef21fabb4522;
        static constexpr int64_t CONSTANT_64_10 = 0xe43d7a0656954b6c;
        static constexpr int64_t CONSTANT_64_11 = 0x56082007c71ab18f;
        static constexpr int64_t CONSTANT_64_12 = 0x76435569a03af7fa;
        static constexpr int64_t CONSTANT_64_13 = 0xd2600de7157abc68;
        static constexpr int64_t CONSTANT_64_14 = 0x6339e901c3031efb;

        // 32 bits constants
        static constexpr int32_t CONSTANT_32_1 = static_cast<int32_t>(CONSTANT_64_3 >> 32 & MASK_32BITS);
        static constexpr int32_t CONSTANT_32_2 = CONSTANT_64_3 & MASK_32BITS;
        static constexpr int32_t CONSTANT_32_3 = static_cast<int32_t>(CONSTANT_64_4 >> 32 & MASK_32BITS);

        // 16 bits constants
        static constexpr int16_t CONSTANT_16_1 = static_cast<int16_t>(CONSTANT_64_5 >> 48 & MASK_16BITS); // 0xbd3d
        static constexpr int16_t CONSTANT_16_2 = static_cast<int16_t>(CONSTANT_64_5 >> 32 & MASK_16BITS); // 0xc2b7
        static constexpr int16_t CONSTANT_16_3 = static_cast<int16_t>(CONSTANT_64_5 >> 16 & MASK_16BITS); // 0xb87c
        static constexpr int16_t CONSTANT_16_4 = static_cast<int16_t>(CONSTANT_64_5 & MASK_16BITS);       // 0x4715

        static constexpr int16_t CONSTANT_16_5 = (CONSTANT_64_6 >> 48 & MASK_16BITS); // 0x6a6c
        static constexpr int16_t CONSTANT_16_6 = static_cast<int16_t>(CONSTANT_64_6 >> 32 & MASK_16BITS); // 0x9527
        static constexpr int16_t CONSTANT_16_7 = static_cast<int16_t>(CONSTANT_64_6 >> 16 & MASK_16BITS); // 0xac2e

        // 8 bits constants
        static constexpr int8_t CONSTANT_8_01 = static_cast<int8_t>(CONSTANT_64_7 >> 56 & MASK_8BITS); // 0xcc
        static constexpr int8_t CONSTANT_8_02 = static_cast<int8_t>(CONSTANT_64_7 >> 48 & MASK_8BITS); // 0x96
        static constexpr int8_t CONSTANT_8_03 = static_cast<int8_t>(CONSTANT_64_7 >> 40 & MASK_8BITS); // 0xed,
        static constexpr int8_t CONSTANT_8_04 = static_cast<int8_t>(CONSTANT_64_7 >> 32 & MASK_8BITS); // 0x16,
        static constexpr int8_t CONSTANT_8_05 = static_cast<int8_t>(CONSTANT_64_7 >> 24 & MASK_8BITS); // 0x74,
        static constexpr int8_t CONSTANT_8_06 = static_cast<int8_t>(CONSTANT_64_7 >> 16 & MASK_8BITS); // 0xea,
        static constexpr int8_t CONSTANT_8_07 = static_cast<int8_t>(CONSTANT_64_7 >> 8 & MASK_8BITS);  // 0xaa,
        static constexpr int8_t CONSTANT_8_08 = static_cast<int8_t>(CONSTANT_64_7 >> 0 & MASK_8BITS);  // 0x03,
        static constexpr int8_t CONSTANT_8_09 = static_cast<int8_t>(CONSTANT_64_8 >> 56 & MASK_8BITS); // 0x1e,
        static constexpr int8_t CONSTANT_8_10 = static_cast<int8_t>(CONSTANT_64_8 >> 48 & MASK_8BITS); // 0x86,
        static constexpr int8_t CONSTANT_8_11 = static_cast<int8_t>(CONSTANT_64_8 >> 40 & MASK_8BITS); // 0x3f,
        static constexpr int8_t CONSTANT_8_12 = static_cast<int8_t>(CONSTANT_64_8 >> 32 & MASK_8BITS); // 0x24,
        static constexpr int8_t CONSTANT_8_13 = static_cast<int8_t>(CONSTANT_64_8 >> 24 & MASK_8BITS); // 0xb2,
        static constexpr int8_t CONSTANT_8_14 = static_cast<int8_t>(CONSTANT_64_8 >> 16 & MASK_8BITS); // 0xa8,
        static constexpr int8_t CONSTANT_8_15 = static_cast<int8_t>(CONSTANT_64_8 >> 8 & MASK_8BITS);  // 0x31,
    };

    // 4 x 128-bit hashing lanes
    __m128i block[4];

    // input block buffer
    __m128i input[4];

    // initialization vector
    __m128i initialize;

    // cumulative input bytes
    size_t input_bytes;

  public:
    // Reference implementation of AquaHash small key algorithm
    static __m128i SmallKeyAlgorithm(const uint8_t *key, const size_t bytes, __m128i initialize = _mm_setzero_si128()) {
        assert(bytes <= MAXLEN);
        __m128i hash = initialize;

        // bulk hashing loop -- 128-bit block size
        const __m128i *ptr128 = reinterpret_cast<const __m128i *>(key);
        if (bytes / sizeof(hash)) {
            __m128i temp = _mm_set_epi64x(Constants::Constants::CONSTANT_64_1, Constants::CONSTANT_64_2);
            for (uint32_t i = 0; i < bytes / sizeof(hash); ++i) {
                __m128i b = _mm_loadu_si128(ptr128++);
                hash = _mm_aesenc_si128(hash, b);
                temp = _mm_aesenc_si128(temp, b);
            }
            hash = _mm_aesenc_si128(hash, temp);
        }

        // AES sub-block processor
        const uint8_t *ptr8 = reinterpret_cast<const uint8_t *>(ptr128);
        if (bytes & 8) {
            __m128i b = _mm_set_epi64x(*reinterpret_cast<const uint64_t *>(ptr8), Constants::Constants::CONSTANT_64_1);
            hash = _mm_xor_si128(hash, b);
            ptr8 += 8;
        }

        if (bytes & 4) {
            __m128i b = _mm_set_epi32(Constants::CONSTANT_32_1, Constants::CONSTANT_32_2,
                                      *reinterpret_cast<const uint32_t *>(ptr8), Constants::CONSTANT_32_3);
            hash = _mm_xor_si128(hash, b);
            ptr8 += 4;
        }

        if (bytes & 2) {
            __m128i b = _mm_set_epi16(Constants::CONSTANT_16_1, Constants::CONSTANT_16_2, Constants::CONSTANT_16_3,
                                      Constants::CONSTANT_16_4, Constants::CONSTANT_16_5, Constants::CONSTANT_16_6,
                                      *reinterpret_cast<const uint16_t *>(ptr8), Constants::CONSTANT_16_7);
            hash = _mm_xor_si128(hash, b);
            ptr8 += 2;
        }

        if (bytes & 1) {
            __m128i b = _mm_set_epi8(Constants::CONSTANT_8_01, Constants::CONSTANT_8_02, Constants::CONSTANT_8_03,
                                     Constants::CONSTANT_8_04, Constants::CONSTANT_8_05, Constants::CONSTANT_8_06,
                                     Constants::CONSTANT_8_07, Constants::CONSTANT_8_08, Constants::CONSTANT_8_09,
                                     Constants::CONSTANT_8_10, Constants::CONSTANT_8_11, Constants::CONSTANT_8_12,
                                     Constants::CONSTANT_8_13, Constants::CONSTANT_8_14,
                                     *reinterpret_cast<const uint8_t *>(ptr8), Constants::CONSTANT_8_15);
            hash = _mm_xor_si128(hash, b);
        }

        // this algorithm construction requires no less than three AES rounds to finalize
        hash = _mm_aesenc_si128(hash, _mm_set_epi64x(Constants::CONSTANT_64_9, Constants::Constants::CONSTANT_64_10));
        hash = _mm_aesenc_si128(
            hash, _mm_set_epi64x(Constants::Constants::CONSTANT_64_11, Constants::Constants::CONSTANT_64_12));
        return _mm_aesenc_si128(
            hash, _mm_set_epi64x(Constants::Constants::CONSTANT_64_13, Constants::Constants::CONSTANT_64_14));
    }

    // Reference implementation of AquaHash large key algorithm
    static __m128i LargeKeyAlgorithm(const uint8_t *key, const size_t bytes, __m128i initialize = _mm_setzero_si128()) {
        assert(bytes <= MAXLEN);

        // initialize 4 x 128-bit hashing lanes, for a 512-bit block size
        __m128i block[4] = {
            _mm_xor_si128(initialize, _mm_set_epi64x(Constants::Constants::CONSTANT_64_1, Constants::CONSTANT_64_2)),
            _mm_xor_si128(initialize, _mm_set_epi64x(Constants::CONSTANT_64_3, Constants::CONSTANT_64_4)),
            _mm_xor_si128(initialize, _mm_set_epi64x(Constants::CONSTANT_64_5, Constants::CONSTANT_64_6)),
            _mm_xor_si128(initialize, _mm_set_epi64x(Constants::CONSTANT_64_7, Constants::CONSTANT_64_8))};

        // bulk hashing loop -- 512-bit block size
        const __m128i *ptr128 = reinterpret_cast<const __m128i *>(key);
        for (size_t block_counter = 0; block_counter < bytes / sizeof(block); block_counter++) {
            block[0] = _mm_aesenc_si128(block[0], _mm_loadu_si128(ptr128++));
            block[1] = _mm_aesenc_si128(block[1], _mm_loadu_si128(ptr128++));
            block[2] = _mm_aesenc_si128(block[2], _mm_loadu_si128(ptr128++));
            block[3] = _mm_aesenc_si128(block[3], _mm_loadu_si128(ptr128++));
        }

        // process remaining AES blocks
        if (bytes & 32) {
            block[0] = _mm_aesenc_si128(block[0], _mm_loadu_si128(ptr128++));
            block[1] = _mm_aesenc_si128(block[1], _mm_loadu_si128(ptr128++));
        }

        if (bytes & 16) {
            block[2] = _mm_aesenc_si128(block[2], _mm_loadu_si128(ptr128++));
        }

        // AES sub-block processor
        const uint8_t *ptr8 = reinterpret_cast<const uint8_t *>(ptr128);
        if (bytes & 8) {
            __m128i b = _mm_set_epi64x(*reinterpret_cast<const uint64_t *>(ptr8), Constants::Constants::CONSTANT_64_1);
            block[3] = _mm_aesenc_si128(block[3], b);
            ptr8 += 8;
        }

        if (bytes & 4) {
            __m128i b = _mm_set_epi32(Constants::CONSTANT_32_1, Constants::CONSTANT_32_2,
                                      *reinterpret_cast<const uint32_t *>(ptr8), Constants::CONSTANT_32_3);
            block[0] = _mm_aesenc_si128(block[0], b);
            ptr8 += 4;
        }

        if (bytes & 2) {
            __m128i b = _mm_set_epi16(Constants::CONSTANT_16_1, Constants::CONSTANT_16_2, Constants::CONSTANT_16_3,
                                      Constants::CONSTANT_16_4, Constants::CONSTANT_16_5, Constants::CONSTANT_16_6,
                                      *reinterpret_cast<const uint16_t *>(ptr8), Constants::CONSTANT_16_7);
            block[1] = _mm_aesenc_si128(block[1], b);
            ptr8 += 2;
        }

        if (bytes & 1) {
            __m128i b = _mm_set_epi8(
                Constants::CONSTANT_8_01, Constants::CONSTANT_8_02, Constants::CONSTANT_8_03, Constants::CONSTANT_8_04,
                Constants::CONSTANT_8_05, Constants::CONSTANT_8_06, Constants::CONSTANT_8_07, Constants::CONSTANT_8_08,
                Constants::CONSTANT_8_09, Constants::CONSTANT_8_10, Constants::CONSTANT_8_11, Constants::CONSTANT_8_12,
                Constants::CONSTANT_8_13, Constants::CONSTANT_8_14, *ptr8, Constants::CONSTANT_8_15);
            block[2] = _mm_aesenc_si128(block[2], b);
        }

        // indirectly mix hashing lanes
        const __m128i mix = _mm_xor_si128(_mm_xor_si128(block[0], block[1]), _mm_xor_si128(block[2], block[3]));
        block[0] = _mm_aesenc_si128(block[0], mix);
        block[1] = _mm_aesenc_si128(block[1], mix);
        block[2] = _mm_aesenc_si128(block[2], mix);
        block[3] = _mm_aesenc_si128(block[3], mix);

        // reduction from 512-bit block size to 128-bit hash
        __m128i hash = _mm_aesenc_si128(_mm_aesenc_si128(block[0], block[1]), _mm_aesenc_si128(block[2], block[3]));

        // this algorithm construction requires no less than one round to finalize
        return _mm_aesenc_si128(hash, _mm_set_epi64x(Constants::CONSTANT_64_9, Constants::Constants::CONSTANT_64_10));
    }

    // NON-INCREMENTAL HYBRID ALGORITHM

    static __m128i Hash(const uint8_t *key, const size_t bytes, __m128i initialize = _mm_setzero_si128()) {
        return bytes < THRESHOLD ? SmallKeyAlgorithm(key, bytes, initialize)
                                 : LargeKeyAlgorithm(key, bytes, initialize);
    }

    // INCREMENTAL HYBRID ALGORITHM

    // Initialize a new incremental hashing object
    AquaHash(const __m128i initialize = _mm_setzero_si128())
        : block{_mm_xor_si128(initialize,
                              _mm_set_epi64x(Constants::Constants::CONSTANT_64_1, Constants::CONSTANT_64_2)),
                _mm_xor_si128(initialize, _mm_set_epi64x(Constants::CONSTANT_64_3, Constants::CONSTANT_64_4)),
                _mm_xor_si128(initialize, _mm_set_epi64x(Constants::CONSTANT_64_5, Constants::CONSTANT_64_6)),
                _mm_xor_si128(initialize, _mm_set_epi64x(Constants::CONSTANT_64_7, Constants::CONSTANT_64_8))},
          initialize(initialize),
          input_bytes(0) {}

    // Initialize an existing hashing object -- all previous state is destroyed
    void Initialize(const __m128i initialize = _mm_setzero_si128()) {
        this->initialize = initialize;
        this->input_bytes = 0;
        block[0] =
            _mm_xor_si128(initialize, _mm_set_epi64x(Constants::Constants::CONSTANT_64_1, Constants::CONSTANT_64_2));
        block[1] = _mm_xor_si128(initialize, _mm_set_epi64x(Constants::CONSTANT_64_3, Constants::CONSTANT_64_4));
        block[2] = _mm_xor_si128(initialize, _mm_set_epi64x(Constants::CONSTANT_64_5, Constants::CONSTANT_64_6));
        block[3] = _mm_xor_si128(initialize, _mm_set_epi64x(Constants::CONSTANT_64_7, Constants::CONSTANT_64_8));
    }

    // Append key to existing hashing object state
    void Update(const uint8_t *key, size_t bytes) {
        assert(input_bytes != FINALIZED);
        assert(bytes <= MAXLEN && MAXLEN - input_bytes >= bytes);

        if (bytes == 0) return;

        // input buffer may be partially filled
        if (input_bytes % sizeof(input)) {
            // pointer to first unused byte in input buffer
            uint8_t *ptr8 = reinterpret_cast<uint8_t *>(input) + (input_bytes % sizeof(input));

            // compute initial copy size from key to input buffer
            size_t copy_size = sizeof(input) - (input_bytes % sizeof(input));
            if (copy_size > bytes) copy_size = bytes;

            // append new key bytes to input buffer
            memcpy(ptr8, key, copy_size);
            input_bytes += copy_size;
            bytes -= copy_size;

            // input buffer not filled by update
            if (input_bytes % sizeof(input)) return;

            // update key pointer to first byte not in the input buffer
            key += copy_size;

            // hash input buffer
            block[0] = _mm_aesenc_si128(block[0], input[0]);
            block[1] = _mm_aesenc_si128(block[1], input[1]);
            block[2] = _mm_aesenc_si128(block[2], input[2]);
            block[3] = _mm_aesenc_si128(block[3], input[3]);
        }

        input_bytes += bytes;

        // input buffer is empty
        const __m128i *ptr128 = reinterpret_cast<const __m128i *>(key);
        while (bytes >= sizeof(block)) {
            block[0] = _mm_aesenc_si128(block[0], _mm_loadu_si128(ptr128++));
            block[1] = _mm_aesenc_si128(block[1], _mm_loadu_si128(ptr128++));
            block[2] = _mm_aesenc_si128(block[2], _mm_loadu_si128(ptr128++));
            block[3] = _mm_aesenc_si128(block[3], _mm_loadu_si128(ptr128++));
            bytes -= sizeof(block);
        }

        // load remaining bytes into input buffer
        if (bytes) memcpy(input, ptr128, bytes);
    }

    // Generate hash from hashing object state. After finalization, the hashing
    // object is in an undefined state and must be initialized before any
    // subsequent calls on the object.
    __m128i Finalize() {
        assert(input_bytes != FINALIZED);
        if (input_bytes < sizeof(block)) {
            __m128i hash = SmallKeyAlgorithm(reinterpret_cast<uint8_t *>(input), input_bytes, initialize);
            input_bytes = FINALIZED;
            return hash;
        } else {
            // process remaining AES blocks
            if (input_bytes & 32) {
                block[0] = _mm_aesenc_si128(block[0], input[0]);
                block[1] = _mm_aesenc_si128(block[1], input[1]);
            }

            if (input_bytes & 16) {
                block[2] = _mm_aesenc_si128(block[2], input[2]);
            }

            // AES sub-block processor
            const uint8_t *ptr8 = reinterpret_cast<const uint8_t *>(&input[3]);
            if (input_bytes & 8) {
                __m128i b =
                    _mm_set_epi64x(*reinterpret_cast<const uint64_t *>(ptr8), Constants::Constants::CONSTANT_64_1);
                block[3] = _mm_aesenc_si128(block[3], b);
                ptr8 += 8;
            }

            if (input_bytes & 4) {
                __m128i b = _mm_set_epi32(Constants::CONSTANT_32_1, Constants::CONSTANT_32_2,
                                          *reinterpret_cast<const uint32_t *>(ptr8), Constants::CONSTANT_32_3);
                block[0] = _mm_aesenc_si128(block[0], b);
                ptr8 += 4;
            }

            if (input_bytes & 2) {
                __m128i b = _mm_set_epi16(Constants::CONSTANT_16_1, Constants::CONSTANT_16_2, Constants::CONSTANT_16_3,
                                          Constants::CONSTANT_16_4, Constants::CONSTANT_16_5, Constants::CONSTANT_16_6,
                                          *reinterpret_cast<const uint16_t *>(ptr8), Constants::CONSTANT_16_7);
                block[1] = _mm_aesenc_si128(block[1], b);
                ptr8 += 2;
            }

            if (input_bytes & 1) {
                __m128i b =
                    _mm_set_epi8(Constants::CONSTANT_8_01, Constants::CONSTANT_8_02, Constants::CONSTANT_8_03,
                                 Constants::CONSTANT_8_04, Constants::CONSTANT_8_05, Constants::CONSTANT_8_06,
                                 Constants::CONSTANT_8_07, Constants::CONSTANT_8_08, Constants::CONSTANT_8_09,
                                 Constants::CONSTANT_8_10, Constants::CONSTANT_8_11, Constants::CONSTANT_8_12,
                                 Constants::CONSTANT_8_13, Constants::CONSTANT_8_14, *ptr8, Constants::CONSTANT_8_15);
                block[2] = _mm_aesenc_si128(block[2], b);
            }

            // indirectly mix hashing lanes
            const __m128i mix = _mm_xor_si128(_mm_xor_si128(block[0], block[1]), _mm_xor_si128(block[2], block[3]));
            block[0] = _mm_aesenc_si128(block[0], mix);
            block[1] = _mm_aesenc_si128(block[1], mix);
            block[2] = _mm_aesenc_si128(block[2], mix);
            block[3] = _mm_aesenc_si128(block[3], mix);

            // reduction from 512-bit block size to 128-bit hash
            __m128i hash = _mm_aesenc_si128(_mm_aesenc_si128(block[0], block[1]), _mm_aesenc_si128(block[2], block[3]));

            // this algorithm construction requires no less than 1 round to finalize
            input_bytes = FINALIZED;
            return _mm_aesenc_si128(hash,
                                    _mm_set_epi64x(Constants::CONSTANT_64_9, Constants::Constants::CONSTANT_64_10));
        }
    }
};
