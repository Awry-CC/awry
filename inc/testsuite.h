#ifndef AWRY_TESTSUITE_H
#define AWRY_TESTSUITE_H 1

#define EXPECTED_FAILURES 25

/*
#include "minitest/extensions.h"

typedef struct ExpectExtStruct {
  int value;
} ExpectExt;

mt_setup_expect_forwards(
  mt_expect_forward(extstruct, ExpectExt*)
  mt_expect_array_forward(extstructarr, ExpectExt*)
)

#define MT_EXPECT_EXTENSIONS mt_setup_expect_extensions( \
  mt_expect_extension(extstruct, ExpectExt*)             \
  mt_expect_extension(extstructarr, ExpectExt**)         \
)
*/

#include "awry/awry.h"

#endif