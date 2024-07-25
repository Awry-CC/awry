#ifndef AWRY_TESTSUITE_H
#define AWRY_TESTSUITE_H 1

#define EXPECTED_FAILURES 25

#include "awry/extensions.h"

typedef struct ExpectExtStruct {
  int value;
} ExpectExt;

AWRY_extension_forwards(
  AWRY_expect_forward(extstruct, ExpectExt*)
  AWRY_expect_array_forward(extstructarr, ExpectExt*)
)

#define AWRY_EXTENSIONS AWRY_setup_extensions( \
  AWRY_extension(extstruct, ExpectExt*)        \
  AWRY_extension(extstructarr, ExpectExt**)    \
)

#include "awry/awry.h"

#endif