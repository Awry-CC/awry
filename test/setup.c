#include "testsuite.h"

int __format_extstruct(ExpectExt* extstruct) {
  return extstruct->value;
}

int __assert_array_extstructarr(ExpectExt* arr_1[], ExpectExt* arr_2[], size_t s1, size_t s2) {
  if (s1/sizeof(ExpectExt*) != s2/sizeof(ExpectExt*)) { return 0; }
  return 1;
}

AWRY_expect_extension(extstruct, ExpectExt*, (actual->value == expected->value), "%i");
AWRY_expect_array_extension_default(extstructarr, ExpectExt*, __assert_array_extstructarr(expected, actual, actual_size, expected_size), NULL);