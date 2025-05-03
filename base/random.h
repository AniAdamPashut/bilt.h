#ifndef RANDOM_H
#define RANDOM_H

#ifndef BASE_H
# include "base.h"
#endif

#ifdef BASE_IMPLEMENTATION

void RandomInit() {
  i64 seed = TimeNow();
  srand(seed);
}

i32 RandomInteger(i32 min, i32 max) {
  assert(min <= max && "min should always be less than or equal to max");
  assert(max - min <= INT32_MAX - 1 && "range too large");

  i32 range = max - min + 1;

  // Calculate scaling factor to avoid modulo bias
  u32 buckets = RAND_MAX / range;
  u32 limit = buckets * range;

  // Reject numbers that would create bias
  u32 r;
  do {
    r = rand();
  } while (r >= limit);

  return min + (r / buckets);
}

f32 RandomFloat(f32 min, f32 max) {
  assert(min <= max && "min must be less than or equal to max");
  f32 normalized = (f32)rand() / RAND_MAX;
  return min + normalized * (max - min);
}

#endif

#endif