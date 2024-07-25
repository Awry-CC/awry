#ifndef MODULE_AWRY_COMMON_H
#define MODULE_AWRY_COMMON_H 1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <setjmp.h>

typedef struct AwryStruct AwryModule;

typedef enum { 
  AWRY_EXPECT_EQUAL_FLAG,
  AWRY_EXPECT_GT_FLAG,
  AWRY_EXPECT_LT_FLAG,
  AWRY_EXPECT_GTE_FLAG,
  AWRY_EXPECT_LTE_FLAG,
  AWRY_EXPECT_RANGE_FLAG,
  AWRY_EXPECT_SIGNAL_FLAG
} Awry_expect_flags;

// =======================================
//         Constants / Settings
// =======================================

#ifndef AWRY_MAX_ASSERTION_BUFFER
#define AWRY_MAX_ASSERTION_BUFFER 0x400
#endif

#ifndef AWRY_EXPECT_EPSILON
#define AWRY_EXPECT_EPSILON 0.000001
#endif

#define AWRY_STDIO 0x01
#define AWRY_XML 0x02

#define AWRY_LOG_NONE  99
#define AWRY_LOG_DEVELOPMENT 6
#define AWRY_LOG_FATAL 5
#define AWRY_LOG_ERROR 4
#define AWRY_LOG_WARN  3
#define AWRY_LOG_INFO  2
#define AWRY_LOG_DEBUG 1
#define AWRY_LOG_ALL   0

#define AWRY_SIGNONE 0
#define AWRY_SIGABRT 1
#define AWRY_SIGFPE  2
#define AWRY_SIGILL  4
#define AWRY_SIGINT  8
#define AWRY_SIGSEGV 16
#define AWRY_SIGTERM 32

#define AWRY_N_SIGNALS 6
#define AWRY_SIGNALS_ARRAY {SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM}

#define AWRY_TEMPLATE_VALUE "%s"

#define AWRY_expect_forward(suffix, type) \
  void __expect_##suffix(AwryModule *awry, type actual, size_t as, int negated, type expected, size_t es, type max_range, size_t ms, Awry_expect_flags flag);

#define AWRY_expect_array_forward(suffix, type) \
  void __expect_##suffix(AwryModule *awry, type actual[], size_t as, int negated, type expected[], size_t es, type max_range[], size_t ms, Awry_expect_flags flag);

#endif