#include "awry.h"

static char* type_to_string(int t);

static char* type_to_string(int t) {
  switch(t) {
    case AWRY_IT_TYPE:
      return "it";
    case AWRY_CONTEXT_TYPE:
      return "context";
    case AWRY_GIVEN_TYPE:
      return "given";
    case AWRY_AND_TYPE:
      return "and";
    case AWRY_WHEN_TYPE:
      return "when";
    case AWRY_ROOT_TYPE:
      return "root";
    default:
      return "undefined";
  }
}

char* awry_expect_flag_to_string(Awry_expect_flags flag) {
  switch(flag) {
    case AWRY_EXPECT_GT_FLAG:
      return "be greater than";
    case AWRY_EXPECT_LT_FLAG:
      return "be less than";
    case AWRY_EXPECT_GTE_FLAG:
      return "be greater than or equal to";
    case AWRY_EXPECT_LTE_FLAG:
      return "be less than or equal to";
    case AWRY_EXPECT_RANGE_FLAG:
      return "be in range";
    case AWRY_EXPECT_SIGNAL_FLAG:
      return "have been captured";
    default:
      return "equal";
  }
}

char* awry_assert_template(int neg, char* format, Awry_expect_flags flag) {
  char* template = malloc(AWRY_MAX_ASSERTION_BUFFER);
  memset(template, '\0', AWRY_MAX_ASSERTION_BUFFER);

  char* range_str = (char*)malloc(15);
  memset(range_str, '\0', 15);

  if (flag == AWRY_EXPECT_RANGE_FLAG) {
    sprintf(range_str, "-(" AWRY_TEMPLATE_VALUE ")", format);
  }

  sprintf(
    template,
    "Expected (" AWRY_TEMPLATE_VALUE ") %s %s (" AWRY_TEMPLATE_VALUE ")%s",
    format,
    !neg ? "to" : "to not",
    awry_expect_flag_to_string(flag),
    format,
    range_str
  );

  free(range_str);

  return template;
}

//
// Suites Prologue, output, epilogue
//
char* awry_format_suites_prologue() {
  switch(Awry.output_format) {
    case AWRY_XML:
      printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
      printf("<testsuites>");
      return NULL;
    default:
      return NULL;
  }
}

char* awry_format_suites_value(char* data) {
  return NULL;
}

char* awry_format_suites_epilogue() {
  switch(Awry.output_format) {
    case AWRY_XML:
      printf("</testsuites>");
      return NULL;
    default:
      return NULL;
  }
}

//
// Suite Prologue, output, epilogue
//
char* awry_format_suite_prologue(char* name) {
  switch(Awry.output_format) {
    case AWRY_XML:
      printf("<testsuite name=\"%s\">", name);
      return NULL;
    default:
      return NULL;
  }
}

char* awry_format_suite_value(char* name) {
  switch(Awry.output_format) {
    case AWRY_XML:
      printf("%s describe %s %s:\n", AWRY_CONSOLE_YELLOW, AWRY_CONSOLE_DEFAULT, name);
      return NULL;
    default:
      return NULL;
  }
}

char* awry_format_suite_epilogue(char* name) {
  switch(Awry.output_format) {
    case AWRY_XML:
      printf("</testsuite>");
      return NULL;
    default:
      return NULL;
  }
}

//
// Block Prologue, output, epilogue
//
char* awry_format_block_prologue(int block_depth, int block_type, char* name) {
  if (name == NULL) { return NULL; }
  switch(Awry.output_format) {
    case AWRY_XML:
      printf("<testsuite id=\"%s\" name=\"%s\">", type_to_string(block_type), name);
      return NULL;
    default:
      return NULL;
  }
}

char* awry_format_block_value(int block_depth, int block_type, char* name) {
  switch(Awry.output_format) {
    case AWRY_STDIO:
      printf("%*c %s %s %s %s\n", block_depth*2, ' ', AWRY_CONSOLE_GREEN, type_to_string(block_type), AWRY_CONSOLE_DEFAULT, name);
      return NULL;
    default:
      return NULL;
  }
}

char* awry_format_block_epilogue(int block_depth, int block_type, char* name) {
  if (name == NULL) { return NULL; }
  switch(Awry.output_format) {
    case AWRY_XML:
      printf("</testsuite>");
      return NULL;
    default:
      return NULL;
  }
}

//
// It Prologue, output, epilogue
//

char* awry_format_it_prologue(int block_depth, char* color, char* bullet, char* name) {
  switch(Awry.output_format) {
    case AWRY_XML:
      printf("<testcase name=\"%s\">", name);
      return NULL;
    default:
      return NULL;
  }
}

char* awry_format_it_value(int block_depth, char* color, char* bullet, char* name) {
  switch(Awry.output_format) {
    case AWRY_STDIO:
      printf("%*c %s %s it %s %s\n", block_depth*2, ' ', color, bullet, name, AWRY_CONSOLE_DEFAULT);
      return NULL;
    default:
      return NULL;
  }
}

char* awry_format_it_epilogue(int block_depth, char* color, char* bullet, char* name) {
  switch(Awry.output_format) {
    case AWRY_XML:
      printf("</testcase>");
      return NULL;
    default:
      return NULL;
  }
}

//
// Assertion Result
//

char* awry_format_assert_failure_value(int block_depth, char* color, char* assert_message) {
  switch(Awry.output_format) {
    case AWRY_STDIO:
      printf("%*c %s %s %s\n", block_depth*2, ' ', color, assert_message, AWRY_CONSOLE_DEFAULT);
      return NULL;
    case AWRY_XML:
      printf("<failure>%s</failure>", assert_message);
      return NULL;
    default:
      return NULL;
  }
}

//
// Summary
//

char* awry_format_summary(AwryModule *awry) {
  switch(awry->output_format) {
    case AWRY_STDIO:
      printf(
        "\n%d tests, %d assertions, %s%d failures%s\n\n",
        awry->test_cases,
        awry->assertions,
        AWRY_CONSOLE_RED,
        awry->failures,
        AWRY_CONSOLE_DEFAULT
      );
      return NULL;
    default:
      return NULL;
  }
}