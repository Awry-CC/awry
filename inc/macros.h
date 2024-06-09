#ifndef AWRY_MACROS_H
#define AWRY_MACROS_H 1

#define AWRY_FAILURE_BULLET      "✖"
#define AWRY_SUCCESS_BULLET      "✔"
#define AWRY_PENDING_BULLET      "•"

#define AWRY_TEST_FAILURE  0
#define AWRY_TEST_PASS     1
#define AWRY_TEST_PENDING -1

#define AWRY_CONSOLE_DEFAULT "\033[0m"
#define AWRY_CONSOLE_YELLOW  "\033[1;33m"
#define AWRY_CONSOLE_CYAN    "\033[0;36m"
#define AWRY_CONSOLE_GREEN   "\033[0;32m"
#define AWRY_CONSOLE_RED     "\033[0;31m"

#define AWRY_ROOT_TYPE 0x00
#define AWRY_DESCRIBE_TYPE 0x01
#define AWRY_IT_TYPE 0x02
#define AWRY_CONTEXT_TYPE 0x03
#define AWRY_GIVEN_TYPE 0x04
#define AWRY_AND_TYPE 0x05
#define AWRY_WHEN_TYPE 0x06

#define AWRY_JMP_HANDLE_0x01
#define AWRY_JMP_HANDLE_0x02 if (!setjmp(minitest.signal_buffer))
#define AWRY_JMP_HANDLE_0x03
#define AWRY_JMP_HANDLE_0x04
#define AWRY_JMP_HANDLE_0x05
#define AWRY_JMP_HANDLE_0x06
#define AWRY_JMP_HANDLE(type) AWRY_JMP_HANDLE_##type

#define AWRY_ATTR_SUPPORTED 1

//
// __attribute__ support
//
#if !defined(__GNUC__) && !defined(__GNUG__) && !defined(__clang__)
  #undef AWRY_ATTR_SUPPORTED
  #define AWRY_ATTR_SUPPORTED 0
  #define __attribute__(name) awry_unsuported_##name##__COUNTER__
#endif

#if !defined(AWRY_MAX_FIXTURES)
  #define AWRY_MAX_FIXTURES 25
#endif

#define AWRY_TO_STRING( str ) #str
#define AWRY_CONSTRUCTOR_NAME( TestName ) __construct_ ## TestName
#define AWRY_CREATE_UNIQUE_NAME( name, line ) name##line
#define AWRY_DEFINE_UNIQUE_NAME( name, line ) AWRY_CREATE_UNIQUE_NAME( name, line )
#define AWRY_UNIQUE_NAME( name ) AWRY_DEFINE_UNIQUE_NAME( name, __COUNTER__ )

//#define do  {
#define end } Awry.step_back(&Awry); }

//
// DESCRIBE blocks
//
#define AWRY_REGISTER_SUITE( suite, handle, TestName, ... ) \
  static void handle(AwryModule* awry); \
  static void __attribute__((constructor)) AWRY_CONSTRUCTOR_NAME(handle)(void) {\
    Awry.register_suite(&Awry, suite, handle); \
  } \
  static void handle(AwryModule* awry) { void* current_expect = NULL; AWRY_JMP_HANDLE(0x01) {

#define AWRY_DEFINE_SUITE( suite, handle, ... ) \
  AWRY_REGISTER_SUITE( suite, handle, AWRY_UNIQUE_NAME( AWRY_ ), __VA_ARGS__ )
#define AWRY_SUITE( suite, handle, ... ) AWRY_DEFINE_SUITE( suite, handle, __VA_ARGS__ )

#if AWRY_ATTR_SUPPORTED
  #define describe( suite, handle, ... ) AWRY_SUITE( suite, handle, __VA_ARGS__ )
#else
  #define describe( handle, ... ) AWRY_SUITE( "Awry Default", handle, __VA_ARGS__ )
#endif

//
// behavior blocks
//
#define AWRY_REGISTER_TEST_CASE( test_type, test_case, TestName, ... ) \
  Awry.register_block(test_type, &Awry, test_case); { AWRY_JMP_HANDLE(test_type) {
#define AWRY_DEFINE_TEST_CASE( test_type, test_case, ... ) \
  AWRY_REGISTER_TEST_CASE( test_type, test_case, AWRY_UNIQUE_NAME( AWRY_ ), __VA_ARGS__ )
#define AWRY_TEST_CASE( test_case_type, testcase, ... ) AWRY_DEFINE_TEST_CASE( test_case_type, testcase, __VA_ARGS__ )

#define context( testcase, ... ) AWRY_TEST_CASE( AWRY_CONTEXT_TYPE, testcase, __VA_ARGS__ )
#define it( testcase, ... ) AWRY_TEST_CASE( AWRY_IT_TYPE, testcase, __VA_ARGS__ )
#define given( testcase, ... ) AWRY_TEST_CASE( AWRY_GIVEN_TYPE, testcase, __VA_ARGS__ )
#define and( testcase, ... ) AWRY_TEST_CASE( AWRY_AND_TYPE, testcase, __VA_ARGS__ )
#define when( testcase, ... ) AWRY_TEST_CASE( AWRY_WHEN_TYPE, testcase, __VA_ARGS__ )

#define AWRY_fixture_before(id) static void __before_fixture_##id (void** subject)
#define AWRY_fixture_after(id) static void __after_fixture_##id (void** subject)
#define AWRY_define_fixture_type(fixture_type, id) AWRY_fixture_##fixture_type(id)
#define define_fixture(...) AWRY_define_fixture_type(__VA_ARGS__)

#define AWRY_set_fixture_type_name(t, id) __##t##_fixture_##id
#define AWRY_set_fixture_type_after(id) Awry.current->current_block->after = AWRY_set_fixture_type_name(after, id);
#define AWRY_set_fixture_type_before(id) Awry.current->current_block->before = AWRY_set_fixture_type_name(before, id);
#define AWRY_set_fixture_type(fixture_type, id) AWRY_set_fixture_type_##fixture_type(id)
#define set_fixture(...) AWRY_set_fixture_type(__VA_ARGS__)

#ifndef before
  #define before(id) set_fixture(before, id)
#endif

#ifndef after
  #define after(id) set_fixture(after, id)
#endif

#define subject(t) Awry.current->subject

#endif