#include "testsuite.h"

int main() {
  Awry.signals = AWRY_SIGFPE | AWRY_SIGSEGV;
  Awry.log_level = AWRY_LOG_ALL;

  Awry.run();

  int result = EXPECTED_FAILURES - Awry.failures;

  Awry.clear(&Awry);

  return result;
}