#pragma once
#include <array>
#include <cassert>
#include <cstdint>
#include <memory.h>
#include <vector>

namespace gl::pbrt {
// PBRT Ref.
// https://github.com/explosion/murmurhash/blob/master/murmurhash/MurmurHash2.cpp
inline uint64_t murmurHash64A(const unsigned char *key, size_t len,
                              uint64_t seed) {
  const uint64_t m = 0xc6a4a7935bd1e995ull;
  const int r = 47;

  uint64_t h = seed ^ (len * m);

  const unsigned char *end = key + 8 * (len / 8);

  while (key != end) {
    uint64_t k;
    std::memcpy(&k, key, sizeof(uint64_t));
    key += 8;

    k *= m;
    k ^= k >> r;
    k *= m;

    h ^= k;
    h *= m;
  }

  switch (len & 7) {
  case 7:
    h ^= uint64_t(key[6]) << 48;
  case 6:
    h ^= uint64_t(key[5]) << 40;
  case 5:
    h ^= uint64_t(key[4]) << 32;
  case 4:
    h ^= uint64_t(key[3]) << 24;
  case 3:
    h ^= uint64_t(key[2]) << 16;
  case 2:
    h ^= uint64_t(key[1]) << 8;
  case 1:
    h ^= uint64_t(key[0]);
    h *= m;
  };

  h ^= h >> r;
  h *= m;
  h ^= h >> r;

  return h;
}

// Hashing Inline Functions
// http://zimbry.blogspot.ch/2011/09/better-bit-mixing-improving-on.html
inline uint64_t mixBits(uint64_t v);

inline uint64_t mixBits(uint64_t v) {
  v ^= (v >> 31);
  v *= 0x7fb5d329728ea185;
  v ^= (v >> 27);
  v *= 0x81dadef4bc2dd44d;
  v ^= (v >> 33);
  return v;
}

template <typename T>
inline uint64_t hashBuffer(const T *ptr, size_t size, uint64_t seed = 0) {
  return murmurHash64A((const unsigned char *)ptr, size, seed);
}

template <typename... Args> inline uint64_t hash(Args... args);

template <typename... Args> inline void hashRecursiveCopy(char *buf, Args...);

template <> inline void hashRecursiveCopy(char *buf) {}

template <typename T, typename... Args>
inline void hashRecursiveCopy(char *buf, T v, Args... args) {
  memcpy(buf, &v, sizeof(T));
  hashRecursiveCopy(buf + sizeof(T), args...);
}

template <typename... Args> inline uint64_t hash(Args... args) {
  // C++, you never cease to amaze: https://stackoverflow.com/a/57246704
  constexpr size_t sz = (sizeof(Args) + ... + 0);
  constexpr size_t n = (sz + 7) / 8;
  uint64_t buf[n];
  hashRecursiveCopy((char *)buf, args...);
  return murmurHash64A((const unsigned char *)buf, sz, 0);
}

template <typename... Args> inline float hashFloat(Args... args) {
  return uint32_t(hash(args...)) * 0x1p-32f;
}
} // namespace gl::pbrt