#ifndef MODULE_AWRY_H
#define MODULE_AWRY_H 1

typedef struct AwryBlockStruct AwryTestBlock;
typedef struct AwryStruct AwryModule;
typedef struct AwryLoggerStruct AwryLoggerModule;

extern AwryModule Awry;
extern AwryLoggerModule AwryLogger;

#include "common.h"
#include "macros.h"
#include "assertions.h"

typedef struct AwryLoggerStruct {
  void (*log_dev)(const char*,...);
  void (*log_fatal)(const char*,...);
  void (*log_error)(const char*,...);
  void (*log_warn)(const char*,...);
  void (*log_info)(const char*,...);
  void (*log_debug)(const char*,...);
} AwryLoggerModule;

typedef struct AwryBlockArrayStruct {
  AwryTestBlock **array;
  size_t used;
  size_t size;
} AwryBlockArray;

typedef struct AwryBlockStruct {
  char *name;
  int block_type;
  int depth;
  int assert_result;
  char *assert_message;

  AwryBlockArray it_blocks;
  AwryBlockArray children;

  void (*before)(void**);
  void (*after)(void**);

  struct AwryBlockStruct *previous;
} AwryBlock;

typedef struct AwrySuiteStruct {
  char *name;
  int block_type;
  int it_flag;
  void* subject;

  AwryBlockArray blocks;
  AwryBlock     *current_block;

  AwryBlock *current_assertion;

  void (*suite)(struct AwryStruct*);

  struct AwrySuiteStruct *next;
  struct AwrySuiteStruct *previous;
} AwryTestSuite;

typedef struct AwryStruct {
  unsigned int assertions;
  unsigned int test_cases;
  unsigned int passes;
  unsigned int failures;
  unsigned int signals;

  int output_format;
  int log_level;

  jmp_buf signal_buffer;
  jmp_buf capture_buffer;

  int expected_signal;
  int captured_signal;

  AwryTestSuite *suites;
  AwryTestSuite *current;

  void (*register_suite)(struct AwryStruct*, const char*, void*);
  void (*step_back)(struct AwryStruct*);
  void (*register_block)(int, struct AwryStruct*, const char*);
  void (*run)();
  void (*clear)(struct AwryStruct*);
  void (*init_signals)(struct AwryStruct*);
  void (*terminate_signals)(struct AwryStruct*);

} AwryModule;

char* awry_format_suites_prologue();
char* awry_format_suites_value(char* data);
char* awry_format_suites_epilogue();

char* awry_format_suite_prologue(char* name);
char* awry_format_suite_value(char* name);
char* awry_format_suite_epilogue(char* name);

char* awry_format_block_prologue(int block_depth, int block_type, char* name);
char* awry_format_block_value(int block_depth, int block_type, char* name);
char* awry_format_block_epilogue(int block_depth, int block_type, char* name);

char* awry_format_it_prologue(int block_depth, char* color, char* bullet, char* name);
char* awry_format_it_value(int block_depth, char* color, char* bullet, char* name);
char* awry_format_it_epilogue(int block_depth, char* color, char* bullet, char* name);

char* awry_format_assert_failure_value(int block_depth, char* color, char* assert_message);

char* awry_format_summary(AwryModule *awry);

#endif