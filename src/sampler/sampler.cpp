#include "sampler.hpp"
thread_local HaltonSampler halton_sampler(0, RandomStrategy::None);
