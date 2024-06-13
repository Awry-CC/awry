#ifndef AWRY_ASSERTIONS_H
#define AWRY_ASSERTIONS_H 1

#include "awry.h"

typedef enum { 
  AWRY_EXPECT_EQUAL_FLAG,
  AWRY_EXPECT_GT_FLAG,
  AWRY_EXPECT_LT_FLAG,
  AWRY_EXPECT_GTE_FLAG,
  AWRY_EXPECT_LTE_FLAG,
  AWRY_EXPECT_RANGE_FLAG,
  AWRY_EXPECT_SIGNAL_FLAG
} Awry_expect_flags;

char* awry_assert_template(int neg, char* format, Awry_expect_flags flag);
char* awry_expect_flag_to_string(Awry_expect_flags flag);

#define AWRY_expect_forward(suffix, type) \
  void __expect_##suffix(AwryModule *awry, type actual, size_t as, int negated, type expected, size_t es, type max_range, size_t ms, Awry_expect_flags flag);

#define AWRY_expect_array_forward(suffix, type) \
  void __expect_##suffix(AwryModule *awry, type actual[], size_t as, int negated, type expected[], size_t es, type max_range[], size_t ms, Awry_expect_flags flag);

#define AWRY_expect_epilogue if(current_expect != NULL) { current_expect = NULL; }

#define be
#define have
#define to    0
#define not   +1
#define equal(expected)      ,expected, sizeof(expected), expected, sizeof(expected), AWRY_EXPECT_EQUAL_FLAG); AWRY_expect_epilogue;

#define be_null  ,NULL, 0,0,0, AWRY_EXPECT_EQUAL_FLAG); AWRY_expect_epilogue;
#define be_false ,0, 0,0,0, AWRY_EXPECT_EQUAL_FLAG); AWRY_expect_epilogue;
#define be_true  ,1, 0,0,0, AWRY_EXPECT_EQUAL_FLAG); AWRY_expect_epilogue;
#define been_called ,0,0,0,0, AWRY_EXPECT_BEEN_CALLED_FLAG); AWRY_expect_epilogue;
#define been_captured ,Awry.captured_signal,0,0,0, AWRY_EXPECT_EQUAL_FLAG); AWRY_expect_epilogue;

#define greater_than(expected) ,expected,0,expected,0,AWRY_EXPECT_GT_FLAG); AWRY_expect_epilogue;
#define less_than(expected) ,expected,0,expected,0,AWRY_EXPECT_LT_FLAG); AWRY_expect_epilogue;
#define greater_than_or_equal_to(expected) ,expected,0,expected,0,AWRY_EXPECT_GTE_FLAG); AWRY_expect_epilogue;
#define less_than_or_equal_to(expected) ,expected,0,expected,0,AWRY_EXPECT_LTE_FLAG); AWRY_expect_epilogue;

#define in_range(min_range, max_range) ,min_range,0,max_range,0,AWRY_EXPECT_RANGE_FLAG); AWRY_expect_epilogue;

#define expect_result Awry.current->current_assertion->assert_result

#define AWRY_default_format_handle(suffix, type, arr, tf) AWRY_default_format_handle_definition_##tf(suffix, type, arr)
#define AWRY_default_format_handle_definition_NONE(suffix, type, arr) void* __format_##suffix(type value arr) { return NULL; }
#define AWRY_default_format_handle_definition_SINGLE(suffix, type, arr) type  __format_##suffix(type value arr) { return value; }
#define AWRY_default_format_handle_definition_ARRAY(suffix, type, arr) type* __format_##suffix(type value arr) { return value; }
#define AWRY_default_format_handle_definition_EXTENSION(suffix, type, arr)

#define AWRY_expect_definition(suffix, type, arr, comparator, format, handle_type) \
                                                                                 \
  int __expect_assert_##suffix(type actual arr, type expected arr, type max_range arr, Awry_expect_flags flag) { \
    AwryLogger.log_debug("Running Assertion \n\t\t<type: %s> \n\t\t<assertion: \"%s\">", #type, #comparator);    \
    switch(flag) {                                                                           \
      case AWRY_EXPECT_GT_FLAG:                                                              \
        return (actual > expected);                                                          \
      case AWRY_EXPECT_LT_FLAG:                                                              \
        return (actual < expected);                                                          \
      case AWRY_EXPECT_GTE_FLAG:                                                             \
        return (actual >= expected);                                                         \
      case AWRY_EXPECT_LTE_FLAG:                                                             \
        return (actual <= expected);                                                         \
      case AWRY_EXPECT_RANGE_FLAG:                                                           \
        return ( (actual >= expected) && (actual <= max_range) );                            \
      default:                                                                               \
        return (actual == expected);                                                         \
    }                                                                                        \
  }                                                                                          \
  AWRY_expect_handle(suffix, type, type, type, arr, comparator, format, handle_type)         \

#define AWRY_expect_handle(suffix, actual_type, expected_type, range_type, arr, comparator, format, handle_type) \
  AWRY_default_format_handle(suffix, expected_type, arr, handle_type)                                                 \
                                                                                                                 \
  void __expect_##suffix(AwryModule *awry, actual_type actual arr, size_t actual_size, int negated, expected_type expected arr, size_t expected_size, range_type max_range arr, size_t max_range_size, Awry_expect_flags flag) {   \
    AwryLogger.log_debug("Expect \n\t\t<type: %s%s> \n\t\t<negated: %d>", #actual_type, #arr, negated);   \
    awry->assertions += 1;                                                            \
    if (awry->current->current_assertion->assert_result == AWRY_TEST_FAILURE) { return; }  \
    int result = negated ? !(comparator) : (comparator);                            \
    if (result) {                                                                   \
      awry->passes += 1;                                                              \
      awry->current->current_assertion->assert_result = AWRY_TEST_PASS;                    \
    } else {                                                                        \
      awry->failures += 1;                                                            \
      awry->current->current_assertion->assert_result = AWRY_TEST_FAILURE;                 \
      if(format == NULL) { return; }                                                    \
      awry->current->current_assertion->assert_message = malloc(AWRY_MAX_ASSERTION_BUFFER); \
      char *template = awry_assert_template(negated, format, flag);                       \
      snprintf(                                                                         \
        awry->current->current_assertion->assert_message, AWRY_MAX_ASSERTION_BUFFER,        \
        template, __format_##suffix (actual), __format_##suffix (expected),             \
                  __format_##suffix (max_range)                                         \
      ); free(template);                                                                \
    }                                                                                   \
  }                                                                                     \

#define AWRY_assert_array(suffix, type, comparator) \
  int __compare_array_##suffix(const void * a, const void *b) {                         \
    type fa = *(const type*) a;                                                         \
    type fb = *(const type*) b;                                                         \
    return (fa > fb) - (fa < fb);                                                       \
  }                                                                                     \
  int __assert_array_##suffix(type arr_1[], type arr_2[], size_t s1, size_t s2) {       \
    AwryLogger.log_debug("Running Assertion \n\t\t<type: %s[]> \n\t\t<assertion: \"__assert_array_%s\">", #type, #suffix); \
    if (s1/sizeof(type) != s2/sizeof(type)) { return 0; }                               \
    qsort(arr_1, s1/sizeof(type), sizeof(type), __compare_array_##suffix);              \
    qsort(arr_2, s2/sizeof(type), sizeof(type), __compare_array_##suffix);              \
    int result = 1;                                                                     \
    for(int i = 0; i < s1/sizeof(type); i++) {                                          \
      if(!comparator) { result = 0; break; }                                            \
    }                                                                                   \
    return result;                                                                      \
  }                                                                                     \

#define AWRY_expect(suffix, type, comparator, format) AWRY_expect_definition(suffix, type,, comparator, format, SINGLE)
#define AWRY_expect_array(suffix, type, comparator, format) AWRY_expect_definition(suffix, type, [], comparator, format, ARRAY)

AWRY_expect_forward(int,    int);
AWRY_expect_forward(char,   char);
AWRY_expect_forward(short,  short);
AWRY_expect_forward(long,   long);
AWRY_expect_forward(double, double);
AWRY_expect_forward(float,  float);
AWRY_expect_forward(ptr,    void*);
AWRY_expect_forward(str,    char*);

// also unsigned long
AWRY_expect_forward(sizet,  size_t);
AWRY_expect_forward(uint,     unsigned int);
AWRY_expect_forward(ushort,   unsigned short);
AWRY_expect_forward(uchar,    unsigned char);

AWRY_expect_array_forward(intarr, int);
AWRY_expect_array_forward(shortarr, short);
AWRY_expect_array_forward(longarr, long);
AWRY_expect_array_forward(doublearr, double);
AWRY_expect_array_forward(floatarr, float);

#define expect_generic(actual) _Generic(actual,                  \
                                        int: __expect_int,       \
                                        char: __expect_char,     \
                                        short: __expect_short,   \
                                        long: __expect_long,     \
                                        double: __expect_double, \
                                        float: __expect_float,   \
                                        void*: __expect_ptr,     \
                                        char*: __expect_str,     \
                                        size_t: __expect_sizet,  \
                                        unsigned int: __expect_uint,     \
                                        unsigned short: __expect_ushort, \
                                        unsigned char: __expect_uchar,   \
                                        int*: __expect_intarr,         \
                                        short*: __expect_shortarr,     \
                                        long*: __expect_longarr,       \
                                        double*: __expect_doublearr,   \
                                        float*: __expect_floatarr      \
                                      ) \

#define __expect_call(awry, actual) expect_generic(actual)(awry, (actual), (sizeof(actual)),
#define expect(actual)               \
  __expect_call(&Awry, actual)   \

#endif