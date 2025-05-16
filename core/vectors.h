#ifndef VECTORS_H
#define VECTORS_H

#include "base.h"

// TODO: Add MSVC like vector macros
#define VEC_TYPE(typeName, valueType)                                                                                                                                                                                                          \
  typedef struct {                                                                                                                                                                                                                             \
    valueType *data;                                                                                                                                                                                                                           \
    u64 length;                                                                                                                                                                                                                                \
    u64 capacity;                                                                                                                                                                                                                              \
  } typeName;

#define VecPush(vector, value)                                                                                                                                                                                                                 \
  ({                                                                                                                                                                                                                                           \
    if (vector.length >= vector.capacity) {                                                                                                                                                                                                    \
      if (vector.capacity == 0) vector.capacity = 128;                                                                                                                                                                                         \
      else vector.capacity *= 2;                                                                                                                                                                                                               \
      vector.data = realloc(vector.data, vector.capacity * sizeof(*vector.data));                                                                                                                                                              \
    }                                                                                                                                                                                                                                          \
    vector.data[vector.length++] = value;                                                                                                                                                                                                      \
    &vector.data[vector.length - 1];                                                                                                                                                                                                           \
  })

#define VecPop(vector)                                                                                                                                                                                                                         \
  ({                                                                                                                                                                                                                                           \
    assert(vector.length > 0 && "Cannot pop from empty vector");                                                                                                                                                                               \
    typeof(vector.data[0]) value = vector.data[vector.length - 1];                                                                                                                                                                             \
    vector.length--;                                                                                                                                                                                                                           \
    &value;                                                                                                                                                                                                                                    \
  })

#define VecShift(vector)                                                                                                                                                                                                                       \
  ({                                                                                                                                                                                                                                           \
    assert(vector.length != 0 && "Length should at least be >= 1");                                                                                                                                                                            \
    typeof(vector.data[0]) value = vector.data[0];                                                                                                                                                                                             \
    memmove(&vector.data[0], &vector.data[1], (vector.length - 1) * sizeof(*vector.data));                                                                                                                                                     \
    vector.length--;                                                                                                                                                                                                                           \
    &value;                                                                                                                                                                                                                                    \
  })

#define VecUnshift(vector, value)                                                                                                                                                                                                              \
  ({                                                                                                                                                                                                                                           \
    if (vector.length >= vector.capacity) {                                                                                                                                                                                                    \
      if (vector.capacity == 0) vector.capacity = 2;                                                                                                                                                                                           \
      else vector.capacity *= 2;                                                                                                                                                                                                               \
      vector.data = realloc(vector.data, vector.capacity * sizeof(*vector.data));                                                                                                                                                              \
    }                                                                                                                                                                                                                                          \
                                                                                                                                                                                                                                               \
    if (vector.length > 0) {                                                                                                                                                                                                                   \
      memmove(&vector.data[1], &vector.data[0], vector.length * sizeof(*vector.data));                                                                                                                                                         \
    }                                                                                                                                                                                                                                          \
                                                                                                                                                                                                                                               \
    vector.data[0] = value;                                                                                                                                                                                                                    \
    vector.length++;                                                                                                                                                                                                                           \
    &value;                                                                                                                                                                                                                                    \
  })

#define VecInsert(vector, value, index)                                                                                                                                                                                                        \
  ({                                                                                                                                                                                                                                           \
    assert(index <= vector.length && "Index out of bounds for insertion");                                                                                                                                                                     \
    if (vector.length >= vector.capacity) {                                                                                                                                                                                                    \
      if (vector.capacity == 0) vector.capacity = 2;                                                                                                                                                                                           \
      else vector.capacity *= 2;                                                                                                                                                                                                               \
      vector.data = realloc(vector.data, vector.capacity * sizeof(*vector.data));                                                                                                                                                              \
    }                                                                                                                                                                                                                                          \
    memmove(&vector.data[index + 1], &vector.data[index], (vector.length - index) * sizeof(*vector.data));                                                                                                                                     \
    vector.data[index] = value;                                                                                                                                                                                                                \
    vector.length++;                                                                                                                                                                                                                           \
    &value;                                                                                                                                                                                                                                    \
  })

#define VecAt(vector, index)                                                                                                                                                                                                                   \
  ({                                                                                                                                                                                                                                           \
    assert(index < vector.length && "Index out of bounds");                                                                                                                                                                      \
    &vector.data[index];                                                                                                                                                                                                                       \
  })

#define VecFree(vector)                                                                                                                                                                                                                        \
  ({                                                                                                                                                                                                                                           \
    assert(vector.data != NULL && "Vector data should never be NULL");                                                                                                                                                                         \
    free(vector.data);                                                                                                                                                                                                                         \
    vector.data = NULL;                                                                                                                                                                                                                        \
  })

#endif
