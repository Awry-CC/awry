#include "testsuite.h"

int main() {
  Awry.signals = AWRY_SIGFPE | AWRY_SIGSEGV;

  Awry.run();

  int result = EXPECTED_FAILURES - Awry.failures;

  Awry.clear(&Awry);

  return result;
}