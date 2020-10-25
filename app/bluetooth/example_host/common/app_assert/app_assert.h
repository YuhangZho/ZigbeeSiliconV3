#ifndef APP_ASSERT_H
#define APP_ASSERT_H

#include <assert.h>

#define app_assert(expr, ...)    \
  do {                           \
    if (!(expr)) {               \
      printf("[%s] ", __func__); \
      printf(__VA_ARGS__);       \
    }                            \
    assert(expr);                \
  } while (0)

#endif // APP_ASSERT_H
