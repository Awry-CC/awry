#include "awry.h"

// ============================
//          Forwards
// ============================

static void register_suite(AwryModule *awry, const char *name, void *test_case);
static void step_back(AwryModule *awry);
static void register_block(int test_type, AwryModule *awry, const char *name);

static void run_suite(AwryTestSuite *suite, AwryModule *awry);
static void run_blocks(AwryModule *awry, AwryBlockArray *blocks);
static void run_it_blocks(AwryModule *awry, int depth, AwryBlockArray *blocks);

static void run_before_fixtures(AwryModule *awry, AwryBlock* current_block);
static void run_after_fixtures(AwryModule *awry, AwryBlock* current_block);

static void init_block_array(AwryBlockArray *a, size_t initialSize);
static void insert_block_array(AwryBlockArray *a, AwryBlock *block);
static void clear(AwryModule *awry);
static void free_suite(AwryTestSuite *suite);
static void free_block(AwryBlock *block);

static void init_signals(AwryModule *awry);
static void terminate_signals(AwryModule *awry);
static void capture_signal(int signal);

// ============================
//        Implementation
// ============================

static void init_block_array(AwryBlockArray *a, size_t initialSize) {
  a->array = (malloc(initialSize * sizeof(AwryBlock*)));
  a->used = 0;
  a->size = initialSize;
}

static void insert_block_array(AwryBlockArray *a, AwryBlock *block) {
  if (a->used == a->size) {
    a->size = (a->size * 3) / 2 + 8;
    a->array = (realloc(a->array, a->size * sizeof(AwryBlock*)));
    AwryLogger.log_debug("Block array resized \n\t\t<size: %d>", a->size);
  }
  a->array[a->used++] = block;
  AwryLogger.log_info("Block inserted into array buffer (%d/%d)", a->used, a->size);
}

static void register_suite(AwryModule *awry, const char *name, void *test_case) {
  AwryLogger.log_debug("Initializing suite \n\t\t<name: \"%s\">", name);
  AwryTestSuite *suite = malloc(sizeof(AwryTestSuite));
  suite->name = malloc(strlen(name) + 1);
  strcpy(suite->name, name);

  suite->suite = test_case;
  suite->block_type = AWRY_DESCRIBE_TYPE;
  suite->next = NULL;
  suite->it_flag = 0;
  suite->current_assertion = NULL;
  suite->subject = NULL;

  init_block_array(&(suite->blocks), 1);

  AwryBlock *root_block = malloc(sizeof(AwryBlock));
  root_block->block_type = AWRY_ROOT_TYPE;
  root_block->before = NULL;
  root_block->after = NULL;

  insert_block_array(&(suite->blocks), root_block);

  suite->current_block = suite->blocks.array[0];

  if (awry->current == NULL) {
    awry->current = suite;
    awry->suites = suite;
  } else {
    awry->current->next = suite;
    awry->current = suite;
  }
}

static void run_before_fixtures(AwryModule *awry, AwryBlock* current_block) {
  AwryBlock* block = current_block;
  void (*before_fixtures[AWRY_MAX_FIXTURES])(void**);

  int index = 0;

  do {
    if (block->before != NULL) {
      before_fixtures[index] = block->before;
      index++;
      AwryLogger.log_debug("Running before fixture \n\t\t<address: %p> \n\t\t<block: \"%s\">", block->before, block->name == NULL ? "Root" :  block->name);
    }
    block = block->previous;
  } while(block != NULL);

  for(int i = index-1; i >= 0; i--) {
    before_fixtures[i](&(awry->current->subject));
  }
}

static void run_after_fixtures(AwryModule *awry, AwryBlock* current_block) {
  AwryBlock* block = current_block;
  do {
    if (block->after != NULL) {
      (block->after)(&(awry->current->subject));
      AwryLogger.log_debug("Running after fixture \n\t\t<address: %p> \n\t\t<block: \"%s\">", block->after, block->name == NULL ? "Root" :  block->name);
    }
    block = block->previous;
  } while(block != NULL);
}

static void register_block(int test_type, AwryModule *awry, const char *name) {
  if (!awry->current) {
    AwryLogger.log_fatal("Block \"%s\" was created outside a describe context. All blocks must have a parent describe block.", name);
    return;
  }

  AwryBlock *block = malloc(sizeof(AwryBlock));
  block->name = malloc(strlen(name) + 1);
  strcpy(block->name, name);
  block->it_blocks.size = 0;
  block->it_blocks.used = 0;
  block->children.used = 0;
  block->assert_message = NULL;
  block->before = NULL;
  block->after = NULL;

  AwryLogger.log_debug("Initializing block \n\t\t<address: %p> \n\t\t<name: \"%s\">", block, block->name);

  if (test_type == AWRY_IT_TYPE) {
    awry->test_cases += 1;
    awry->current->current_assertion = block;
    awry->current->it_flag = 1;
    if(!awry->current->current_block->it_blocks.size) {
      init_block_array(&(awry->current->current_block->it_blocks), 1);
    }
    block->block_type = AWRY_IT_TYPE;
    block->assert_result = AWRY_TEST_PENDING;
    insert_block_array(&(awry->current->current_block->it_blocks), block);
    run_before_fixtures(awry, awry->current->current_block);
  } else {
    block->block_type = test_type;
    block->previous   = awry->current->current_block;
    block->depth      = awry->current->current_block->depth + 1;

    if(!awry->current->current_block->children.size) {
      init_block_array(&(awry->current->current_block->children), 1);
    }

    insert_block_array(&(awry->current->current_block->children), block);

    awry->current->current_block = block;
  }
}

static void step_back(AwryModule *awry) {
  if (awry->current) {
    if (awry->current->it_flag) {
      awry->current->it_flag = 0;
      run_after_fixtures(awry, awry->current->current_block);

      awry->expected_signal = 0;
      awry->captured_signal = 0;

      return;
    }

    if (awry->current->current_block && awry->current->current_block->previous) {
      awry->current->current_block = awry->current->current_block->previous;
    }
  }
}

static void run_it_blocks(AwryModule *awry, int depth, AwryBlockArray *blocks) {
  if (!blocks->size) { return; }

  for (int i = 0; i < blocks->used; i++) {
    AwryBlock *block = blocks->array[i];

    char *color = block->assert_result == AWRY_TEST_PENDING ? AWRY_CONSOLE_CYAN : block->assert_result == AWRY_TEST_PASS ? AWRY_CONSOLE_GREEN : AWRY_CONSOLE_RED;
    char *bullet = block->assert_result == AWRY_TEST_PENDING ? AWRY_PENDING_BULLET : block->assert_result == AWRY_TEST_PASS ? AWRY_SUCCESS_BULLET : AWRY_FAILURE_BULLET;

    AwryLogger.log_info(
      "Running Assertion \n\t\t<name: \"%s\"> \n\t\t<result: %s>",
      block->name,
      (block->assert_result == AWRY_TEST_PENDING ? "Pending" : block->assert_result == AWRY_TEST_PASS ? "Passed" : "Failed")
    );

    awry_format_it_prologue(depth, color, bullet, block->name);
    awry_format_it_value(depth, color, bullet, block->name);

    if (block->assert_result == AWRY_TEST_FAILURE) {
      awry_format_assert_failure_value(depth, color, block->assert_message);
    }

    awry_format_it_epilogue(depth, color, bullet, block->name);
  }
}

static void run_blocks(AwryModule *awry, AwryBlockArray *blocks) {
  if (!blocks->size) { return; }

  AwryLogger.log_info("Running blocks <count: %d>", blocks->used);

  for (int i = 0; i < blocks->used; i++) {
    AwryBlock *block = blocks->array[i];

    awry_format_block_prologue(block->depth, block->block_type, block->name);

    if(block->block_type != AWRY_ROOT_TYPE) {
      AwryLogger.log_info("Running block \n\t\t<name: \"%s\"> \n\t\t<type: %d> \n\t\t<depth: %d>", block->name, block->block_type, block->depth);
      awry_format_block_value(block->depth, block->block_type, block->name);
    }

    run_it_blocks(awry, block->depth+1, &(block->it_blocks));

    run_blocks(awry, &(block->children));

    awry_format_block_epilogue(block->depth, block->block_type, block->name);
  }
}

static void run_suite(AwryTestSuite *suite, AwryModule *awry) {
  awry->current = suite;

  if (!suite) { return; }

  AwryLogger.log_info("Running suite \n\t\t<name: \"%s\">", suite->name);

  awry_format_suite_prologue(suite->name);
  awry_format_suite_value(suite->name);

  suite->suite(awry);

  run_blocks(awry, &(suite->blocks));

  awry_format_suite_epilogue(suite->name);

  run_suite(suite->next, awry);
}

static void run() {
  awry_format_suites_prologue();
  awry_format_suites_value(NULL);

  Awry.init_signals(&Awry);
  run_suite(Awry.suites, &Awry);
  Awry.terminate_signals(&Awry);

  awry_format_summary(&Awry);
  awry_format_suites_epilogue();
}

static void free_block(AwryBlock *block) {
  if (block == NULL) { return; }

  for(int i = 0; i < block->it_blocks.used; i++) {
    free_block(block->it_blocks.array[i]);
    block->it_blocks.array[i] = NULL;
  }

  for(int i = 0; i < block->children.used; i++) {
    free_block(block->children.array[i]);
    block->children.array[i] = NULL;
  }

  free(block->name);
  if(block->assert_message) {
    free(block->assert_message);
  }
  free(block);

  block = NULL;
}

static void free_suite(AwryTestSuite *suite) {
  if (suite == NULL) { return; }

  free_suite(suite->next);

  for(int i = 0; i < suite->blocks.used; i++) {
    free_block(suite->blocks.array[i]);
  }

  free(suite->name);

  suite->current_block = NULL;
  suite->current_assertion = NULL;
  suite->next = NULL;
  suite->previous = NULL;

  free(suite);

  suite = NULL;
}

static void clear(AwryModule *awry) {
  free_suite(awry->suites);
  awry->suites = NULL;
  awry->current = NULL;
  awry->assertions = 0;
  awry->test_cases = 0;
  awry->passes = 0;
  awry->failures = 0;
}

static void capture_signal(int signal) {
  AwryLogger.log_info(
    "Signal \"%d\" was captured: \n\t Suite: %s \n\t Context: %s \n\t Test Case: %s",
    signal,
    Awry.current->name,
    Awry.current->current_block->name,
    Awry.current->current_assertion->name
  );

  if (Awry.expected_signal != 0 && signal == Awry.expected_signal) {
    Awry.captured_signal = signal;
    longjmp(Awry.capture_buffer, 1);
  } else {
    Awry.failures += 1;
    Awry.current->current_assertion->assert_result = AWRY_TEST_FAILURE;
  
    char *template = awry_assert_template(1, AWRY_TEMPLATE_VALUE, AWRY_EXPECT_SIGNAL_FLAG);
    Awry.current->current_assertion->assert_message = malloc(AWRY_MAX_ASSERTION_BUFFER);
  
    char sig_str[3];
    sprintf(sig_str, "%d", signal);
  
    snprintf(                                                                         
      Awry.current->current_assertion->assert_message, AWRY_MAX_ASSERTION_BUFFER,
      template, "Signal", sig_str, ""
    );
  
    free(template);
  
    longjmp(Awry.signal_buffer, 1);
  }
}

static void init_signals(AwryModule *awry) {
  if (awry->signals == AWRY_SIGNONE) { return; }

  int signals[] = AWRY_SIGNALS_ARRAY;
  for (int i = 0; i < AWRY_N_SIGNALS; i++) {
    if (awry->signals & (1 << i)) {
      signal(signals[i], capture_signal);
    }
  }
}

static void terminate_signals(AwryModule *awry) {
  if (awry->signals == AWRY_SIGNONE) { return; }

  int signals[] = AWRY_SIGNALS_ARRAY;
  for (int i = 0; i < AWRY_N_SIGNALS; i++) {
    if (awry->signals & (1 << i)) {
      signal(signals[i], SIG_DFL);
    }
  }
}

AwryModule Awry = {
  .assertions = 0,
  .test_cases = 0,
  .passes = 0,
  .failures = 0,
  .expected_signal = 0,
  .captured_signal = 0,
  .log_level = AWRY_LOG_ERROR,
  .output_format = AWRY_STDIO,
  .suites = NULL,
  .current = NULL,
  .register_suite = register_suite,
  .register_block = register_block,
  .step_back = step_back,
  .run = run,
  .clear = clear,
  .signals = AWRY_SIGNONE,
  .init_signals = init_signals,
  .terminate_signals = terminate_signals
};