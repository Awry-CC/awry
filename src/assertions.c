#include "assertions.h"

int __double_equal(double actual, double expected, double epsilon) {
  return fabs(actual - expected) < epsilon;
}

int __float_equal(float actual, float expected, float epsilon) {
  return fabs(actual - expected) < epsilon;
}

AWRY_expect(int,    int,    __expect_assert_int(actual,expected,max_range,flag), "%i");
AWRY_expect(char,   char,   __expect_assert_char(actual,expected,max_range,flag), "%c");
AWRY_expect(short,  short,  __expect_assert_short(actual,expected,max_range,flag), "%i");
AWRY_expect(long,   long,   __expect_assert_long(actual,expected,max_range,flag), "%ld");

AWRY_expect(double, double, (__double_equal(actual,expected,AWRY_EXPECT_EPSILON)), "%f");
AWRY_expect(float,  float,  (__float_equal(actual,expected,AWRY_EXPECT_EPSILON)),  "%f");

AWRY_expect(ptr,    void*,  __expect_assert_ptr(actual,expected,max_range,flag), "%p");
AWRY_expect(str,    char*,  (strcmp(actual,expected)==0), "%s");

// also unsigned long
AWRY_expect(sizet,  size_t, __expect_assert_sizet(actual,expected,max_range,flag), "%zu");

AWRY_expect(uint,     unsigned int,    __expect_assert_uint(actual,expected,max_range,flag),    "%u");
AWRY_expect(ushort,   unsigned short,  __expect_assert_ushort(actual,expected,max_range,flag),  "%u");
AWRY_expect(uchar,    unsigned char,   __expect_assert_uchar(actual,expected,max_range,flag),   "%d");

//
// array expectations
//
AWRY_assert_array(int, int, (arr_1[i] == arr_2[i]));
AWRY_expect_array(intarr, int, __assert_array_int(expected, actual, actual_size, expected_size), NULL);

AWRY_assert_array(short, short, (arr_1[i] == arr_2[i]));
AWRY_expect_array(shortarr, short, __assert_array_short(expected, actual, actual_size, expected_size), NULL);

AWRY_assert_array(long, long, (arr_1[i] == arr_2[i]));
AWRY_expect_array(longarr, long, __assert_array_long(expected, actual, actual_size, expected_size), NULL);

AWRY_assert_array(double, double, (__double_equal(arr_1[i],arr_2[i],AWRY_EXPECT_EPSILON)) );
AWRY_expect_array(doublearr, double, __assert_array_double(expected, actual, actual_size, expected_size), NULL);

AWRY_assert_array(float, float, (__float_equal(arr_1[i],arr_2[i],AWRY_EXPECT_EPSILON)) );
AWRY_expect_array(floatarr, float, __assert_array_float(expected, actual, actual_size, expected_size), NULL);

void __set_expect_noop(int valid, ...) {}