#pragma once
#include <array>
#include <memory.h>
#include <vector>
// class DigitPermutation {
// public:
//   DigitPermutation(int base, uint32_t seed, Allocator alloc) : base(base) {
//     nDigits = 0;
//     float invBase = (float)1 / (float)base, invBaseM = 1;
//     while (1 - (base - 1) * invBaseM < 1) {
//       ++nDigits;
//       invBaseM *= invBase;
//     }

//     permutations = alloc.allocate_object<uint16_t>(nDigits * base);
//     for (int digitIndex = 0; digitIndex < nDigits; ++digitIndex) {
//       uint64_t dseed = Hash(base, digitIndex, seed);
//       for (int digitValue = 0; digitValue < base; ++digitValue) {
//         int index = digitIndex * base + digitValue;
//         permutations[index] = PermutationElement(digitValue, base, dseed);
//       }
//     }
//   }
//   int permute(int digitIndex, int digitValue) const {
//     return permutations[digitIndex * base + digitValue];
//   }

// private:
//   int base, nDigits;
//   uint16_t *permutations;
// };